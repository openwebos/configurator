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
#include "ActivityConfigurator.h"
#include "BusClient.h"
#include "core/MojServiceMessage.h"

#include <string.h>

using namespace std;

const char* const ActivityConfigurator::ACTIVITYMGR_BUS_ADDRESS   = "com.palm.activitymanager";
const char* const ActivityConfigurator::ACTIVITYMGR_CREATE_METHOD = "create";
const char* const ActivityConfigurator::ACTIVITYMGR_REMOVE_METHOD = "cancel";
const char* const ActivityConfigurator::ACTIVITY				  = "activity";
const char* const ActivityConfigurator::CREATOR					  = "creator";
const char* const ActivityConfigurator::APPLICATION_ID				  = "appId";
const char* const ActivityConfigurator::SERVICE_ID				  = "serviceId";
const char* const ActivityConfigurator::NAME		  = "name";
const char* const ActivityConfigurator::ACTIVITY_NAME		  = "activityName";
const char* const ActivityConfigurator::FIRST_USE_SAFE		  = "firstUseSafe";
const char* const ActivityConfigurator::APP_DIR		  = "/etc/palm/activities/applications/";
const char* const ActivityConfigurator::SERVICE_DIR		  = "/etc/palm/activities/services/";
const char* const ActivityConfigurator::FIRST_USE_FLAG    = "/var/luna/preferences/ran-first-use";
const char* const ActivityConfigurator::FIRST_USE_PROFILE_FLAG = "/var/luna/preferences/first-use-profile-created";

class ActivityConfigureResponse : public ConfiguratorCallback {
public:
	ActivityConfigureResponse(ActivityConfigurator *conf, const string& path)
		: ConfiguratorCallback(conf, path)
	{
	}

	~ActivityConfigureResponse()
	{
	}

	MojErr Response(MojObject& response, MojErr err)
	{
		bool success = true;
		response.get("returnValue", success);

		if (err || !success) {
			MojInt64 errorCode;
			if (response.get("errorCode", errorCode)) {
				if (errorCode == MojErrExists) {
					LOG_DEBUG("caching ok negative response for %s", m_config.c_str());
					bool found = false;
					response.del("errorCode", found);
					response.del("errorText", found);
					err = response.putBool("returnValue", true);
					MarkConfigured();
				} else {
					LOG_WARNING(MSGID_ACTIVITY_CONFIGURATOR_WARNING, 1,
							PMLOGKS("errorCode", (int)errorCode),
							"Unrecognized errorCode %d", (int)errorCode);
				}
			} else {
				LOG_WARNING(MSGID_ACTIVITY_CONFIGURATOR_WARNING, 0, "errorCode not provided in request failure");
			}
		}
		return DelegateResponse(response, err);
	}
};

const char* ActivityConfigurator::ConfiguratorName() const
{
	 return "ActivityConfigurator";
}

const char* ActivityConfigurator::ServiceName() const
{
	return ACTIVITYMGR_BUS_ADDRESS;
}

ConfiguratorCallback* ActivityConfigurator::CreateCallback(const std::string &filePath)
{
	return new ActivityConfigureResponse(this, filePath);
}

ActivityConfigurator::ActivityConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, string configDirectory)
	: Configurator(id, confType, type, busClient, configDirectory),
	  m_firstUseOnly(true)
{
	int err;
	struct stat buf;
	err = stat(FIRST_USE_FLAG, &buf);
	if (err == 0) {
		err = stat(FIRST_USE_PROFILE_FLAG, &buf);
		if (err == 0) {
			LOG_DEBUG("Fist Use has completed, installing all Activities");
			m_firstUseOnly = false;
		}
	}
#ifdef WEBOS_TARGET_MACHINE_STANDALONE
	//Set the m_firstUseOnly flag to false as there is no first use run in Desktop.
 	m_firstUseOnly = false;
#endif
}

ActivityConfigurator::~ActivityConfigurator()
{

}

bool ActivityConfigurator::CanCacheConfiguratorStatus(const std::string&) const
{
	LOG_TRACE("Entering function %s", __FUNCTION__);
	return false;
}

static void RemoveKey(MojObject& object, const MojChar *key)
{
	bool keydeleted;
	object.del(key, keydeleted);
}

static bool StartsWith(const std::string& s1, const std::string& prefix)
{
	return prefix.length() <= s1.length() && memcmp(s1.c_str(), prefix.c_str(), prefix.length()) == 0;
}

MojErr ActivityConfigurator::ProcessConfig(const string& filePath, MojObject& params)
{
	LOG_TRACE("Entering function %s", __FUNCTION__);

	const std::string& creator = ParentId(filePath);

	if (creator.empty()) {
		LOG_ERROR(MSGID_ACTIVITY_CONFIGURATOR_ERROR, 0, "Service id for activity '%s' is empty - needs to be in an appropriate subdirectory or given as part of the service call", filePath.c_str());
		return MojErrInvalidArg;
	}

	if (m_firstUseOnly) {
		bool firstUseSafe;
		if (!params.get(FIRST_USE_SAFE, firstUseSafe) || !firstUseSafe) {
			LOG_DEBUG("Running before first use but activity %s not marked as safe for configuration at this time", filePath.c_str());
			return MojErrInProgress;
		}
	}

	LOG_DEBUG("ActivityConfigurator creator for %s is %s\n", filePath.c_str(), creator.c_str());

	MojObject activity;
	MojErr err;
	err = params.getRequired(ACTIVITY, activity);
	MojErrCheck(err);

	ConfigType confType;

	if (m_confType == ConfigUnknown) {
		if (StartsWith(filePath, APP_DIR)) {
			confType = ConfigApplication;
		} else {
			if (!StartsWith(filePath, SERVICE_DIR)) {
				//Fix for DFISH-14510 turn down logging here
				//MojLogWarning(m_log, "Deprecated usage: %s not in application or service subdirectory as required - assumed to be service", filePath.c_str());
			}
			confType = ConfigService;
		}
	} else {
		confType = m_confType;
	}

	MojObject creatorObj;
	switch (confType) {
	case ConfigApplication:
		err = creatorObj.putString(APPLICATION_ID, creator.c_str());
		break;
	case ConfigService:
		err = creatorObj.putString(SERVICE_ID, creator.c_str());
		break;
	case ConfigUnknown:
		// impossible case
		assert(false);
		break;
	}

	MojErrCheck(err);

	err = activity.put(CREATOR, creatorObj);
	MojErrCheck(err);
	err = params.put(ACTIVITY, activity);
	MojErrCheck(err);

	// strip configurator-specific keys
	// so that the schema on activity manager isn't violated
	RemoveKey(params, FIRST_USE_SAFE);

	return m_busClient.CreateRequest()->send(CreateCallback(filePath)->m_slot, ServiceName(), ACTIVITYMGR_CREATE_METHOD, params);
}

MojErr ActivityConfigurator::ProcessConfigRemoval(const string& filePath, MojObject& params)
{
	LOG_TRACE("Entering function %s", __FUNCTION__);
	MojErr err;

	MojObject activity;
	MojString activityName;

	err = params.getRequired(ACTIVITY, activity); MojErrCheck(err);
	err = activity.getRequired(NAME, activityName); MojErrCheck(err);

	MojObject request(MojObject::TypeObject);
	const std::string& creator = ParentId(filePath);

	if (creator.empty()) {
		LOG_ERROR(MSGID_ACTIVITY_CONFIGURATOR_ERROR, 0, "Service id for activity '%s' is empty - needs to be in an appropriate subdirectory or given as part of the service call", filePath.c_str());
		return MojErrInvalidArg;
	}

	// {"activityName": "..."}
	err = request.putString(ACTIVITY_NAME, activityName); MojErrCheck(err);
	// {"activityName": "...", "creator": "..."}
	err = request.putString(CREATOR, creator.c_str()); MojErrCheck(err);

	return m_busClient.CreateRequest()->send(CreateCallback(filePath)->m_slot, ServiceName(), ACTIVITYMGR_REMOVE_METHOD, request);
}
