// @@@LICENSE
//
//      Copyright (c) 2009-2013 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// LICENSE@@@

#include "core/MojObject.h"
#include "BusClient.h"
#include "FileCacheConfigurator.h"

using namespace std;

const char* const FileCacheConfigurator::FILECACHE_BUS_ADDRESS		 = "com.palm.filecache";
const char* const FileCacheConfigurator::FILECACHE_DEFINETYPE_METHOD = "DefineType";
const char* const FileCacheConfigurator::FILECACHE_DELETETYPE_METHOD = "DeleteType";
const char* const FileCacheConfigurator::FILECACHE_TYPENAME_KEY = "typeName";

static bool endsWith(const std::string& str, const std::string &suffix)
{
	size_t toSearch = str.length() - suffix.length();
	return toSearch >= 0 && string::npos != str.rfind(suffix, toSearch);
}

class FileCacheConfiguratorResponse : public ConfiguratorCallback {
	// copied from FileCacheError.h - unfortunately this is brittle,
	// but i'm only using it to cache NAK response for DefineType & I'm doing
	// a string comparison too to make sure the failure reason was what I expected,
	// so I believe this should be enough to keep configurator safe when FileCache changes
	// (only bad behaviour should be sub-optimal performance in that the configurator will try
	// to re-register file cache types that are already registered more frequently)
	enum FileCacheFailure {
		FCErrorNone = 0,

		FCInvalidParams = -200,
		FCExistsError,
		FCInUseError,
		FCDefineError,
		FCChangeError,
		FCDeleteError,
		FCResizeError,
		FCPermError,
		FCArgumentError,
		FCDirectoryError,
		FCCopyObjectError,
		FCUnknownError
	};

public:
	FileCacheConfiguratorResponse(FileCacheConfigurator *conf, const string& path)
		: ConfiguratorCallback(conf, path)
	{
	}

	~FileCacheConfiguratorResponse()
	{
	}

	MojErr Response(MojObject& response, MojErr err)
	{
		bool success = true;
		response.get("returnValue", success);

		if (err || !success) {
			// unfortunately file cache doesn't use regular C++ mojo error codes
			// & more unfortunately all the various failure reasons for define are hidden
			// behind 1 error code
			MojInt64 errorCode;
			if (response.get("errorCode", errorCode)) {
				FileCacheFailure failure = (FileCacheFailure)errorCode;
				// be generous in the error code that can be used
				if (failure == FCDefineError || failure == FCExistsError) {
					MojString mojErrorText;
					string errorText;
					std::string nakOkSuffix = "' already exists.";

					response.getRequired("errorText", mojErrorText);
					errorText = mojErrorText.data();

					if (endsWith(errorText, nakOkSuffix)) {
						LOG_DEBUG("caching negative response for %s", m_config.c_str());
						bool found = false;
						response.del("errorCode", found);
						response.del("errorText", found);
						err = response.putBool("returnValue", true);
						MarkConfigured();
					} else {
						LOG_WARNING(MSGID_FILE_CACHE_CONFIG_WARNING, 2,
								PMLOGKS("error", errorText.c_str()),
								PMLOGKS("suffix", nakOkSuffix.c_str()),
								"File cache failed to register type (`%s' doesn't end with `%s'", errorText.c_str(), nakOkSuffix.c_str());
					}
				} else {
					LOG_WARNING(MSGID_FILE_CACHE_CONFIG_WARNING, 1,
							PMLOGKFV("error", "%d", (int)errorCode),
							"Unrecognized errorCode %d", (int)errorCode);
				}
			} else {
				LOG_WARNING(MSGID_FILE_CACHE_CONFIG_WARNING, 0, "errorCode not provided in request failure");
			}
		} else {
			LOG_DEBUG("FileCacheConfigurator response for %s contained no problems", m_config.c_str());
		}
		return DelegateResponse(response, err);
	}
};

const char* FileCacheConfigurator::ConfiguratorName() const
{
	 return "FileCacheConfigurator";
}

const char* FileCacheConfigurator::ServiceName() const
{
	 return FILECACHE_BUS_ADDRESS;
}

FileCacheConfigurator::FileCacheConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, string configDirectory)
: Configurator(id, confType, type, busClient, configDirectory)
{

}

FileCacheConfigurator::~FileCacheConfigurator()
{

}

ConfiguratorCallback* FileCacheConfigurator::CreateCallback(const string& filePath)
{
	return new FileCacheConfiguratorResponse(this, filePath);
}

MojErr FileCacheConfigurator::ProcessConfig(const string& filePath, MojObject& params)
{
	LOG_TRACE("Entering function %s", __FUNCTION__);

	return m_busClient.CreateRequest()->send(CreateCallback(filePath)->m_slot, FILECACHE_BUS_ADDRESS, FILECACHE_DEFINETYPE_METHOD, params);
}

MojErr FileCacheConfigurator::ProcessConfigRemoval(const string& filePath, MojObject& params)
{
	LOG_TRACE("Entering function %s", __FUNCTION__);
	MojErr err;

	MojString typeName;
	err = params.getRequired(FILECACHE_TYPENAME_KEY, typeName); MojErrCheck(err);

	MojObject request(MojObject::TypeObject);
	err = request.putString(FILECACHE_TYPENAME_KEY, typeName); 	MojErrCheck(err);

	return m_busClient.CreateRequest()->send(CreateCallback(filePath)->m_slot, ServiceName(), FILECACHE_DELETETYPE_METHOD, request);
}
