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
#include "DbKindConfigurator.h"
#include "BusClient.h"

using namespace std;

static const char *MOJODB_DB_BUS_ADDRESS = "com.palm.db";
static const char *MOJODB_TEMPDB_BUS_ADDRESS = "com.palm.tempdb";
static const char *MOJODB_MEDIADB_BUS_ADDRESS = "com.webos.mediadb";
static const char *MOJODB_PUTKIND_METHOD = "putKind";
static const char *MOJODB_DELKIND_METHOD = "delKind";

const char* DbKindConfigurator::ConfiguratorName() const
{
	 return "DbKindConfigurator";
}

const char* DbKindConfigurator::ServiceName() const
{
	 return MOJODB_DB_BUS_ADDRESS;
}

typedef MojServiceRequest::ReplySignal::Slot<Configurator> ReplySlot;

class DbKindConfiguratorResponse : public ConfiguratorCallback
{
public:
	DbKindConfiguratorResponse(Configurator *configurator, const std::string& configFile)
		: ConfiguratorCallback(configurator, configFile)
	{
	}

	~DbKindConfiguratorResponse()
	{
	}

	MojErr Response(MojObject& response, MojErr err)
	{
		return MojErrNone;
	}
};

DbKindConfigurator::DbKindConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, MojDbClient& dbClient, string configDirectory)
: Configurator(id, confType, type, busClient, configDirectory),
  m_dbClient(dbClient)
{
}

DbKindConfigurator::~DbKindConfigurator()
{

}

MojErr DbKindConfigurator::CheckOwner(const std::string& filePath, MojObject &params, std::string& ownerid) const
{
	ownerid = ParentId(filePath);
	if (!ownerid.empty()) {
		MojString owner;
		if (params.getRequired("owner", owner) == MojErrNone) {
			// Fix for DFISH-14510 turn off logging here
			// MojLogWarning(m_log, "Deprecated usage - owner should not be specified in %s (%s overridden to %s)", filePath.c_str(), owner.data(), ownerid.c_str());
			// todo - return error once we disallow deprecated usage
		} else {
			LOG_DEBUG("setting owner for %s (%s)", filePath.c_str(), ownerid.c_str());
		}
		params.putString("owner", ownerid.c_str());
	} else {
		// todo - return error once we disallow deprecated usage
		// an owner should always be available as a property of the filePath
		MojErr err;
		MojString owner;
		err = params.getRequired("owner", owner);
		if (err != MojErrNone) {
			LOG_ERROR(MSGID_DB_KIND_CONFIG_ERROR, 1,
					PMLOGKS("file", filePath.c_str()),
					"Cannot determine owner of db kind for %s", filePath.c_str());
			return MojErrInvalidSchema;
		} else {
			// Fix for DFISH-14510 turn off logging here
			// MojLogWarning(m_log, "Deprecated usage - owner should not be specified but instead be a subdirectory or a third-party package in %s", filePath.c_str());
		}
		ownerid = owner.data();
	}
	return MojErrNone;
}

MojErr DbKindConfigurator::ProcessConfig(const string& filePath, MojObject& params)
{
	LOG_TRACE("Entering function %s", __FUNCTION__);
	MojErr err;
	std::string owner;

	err = CheckOwner(filePath, params, owner);
	MojErrCheck(err);

	return m_busClient.CreateRequest(owner.c_str())->send(CreateCallback(filePath)->m_slot, ServiceName(), MOJODB_PUTKIND_METHOD, params);
}

MojErr DbKindConfigurator::ProcessConfigRemoval(const string& filePath, MojObject& params)
{
	LOG_TRACE("Entering function %s", __FUNCTION__);
	MojErr err;
	MojString id;
	MojObject delKind;
	std::string owner;

	err = CheckOwner(filePath, params, owner);
	MojErrCheck(err);

	err = params.getRequired("id", id);
	if (err != MojErrNone) {
		const std::string creator = ParentId(filePath);

		MojErrThrowMsg(err, "Failed to remove db kind for %s - id is missing", creator.c_str());
	}

	err = delKind.putString("id", id);
	MojErrCheck(err);

	return m_busClient.CreateRequest(owner.c_str())->send(CreateCallback(filePath)->m_slot, ServiceName(), MOJODB_DELKIND_METHOD, delKind);
}

////////////////////////////////////////////////
// mediaDB
MediaDbKindConfigurator::MediaDbKindConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, MojDbClient& dbClient, string configDirectory)
    : DbKindConfigurator(id, confType, type, busClient, dbClient, configDirectory)
{
}

const char* MediaDbKindConfigurator::ServiceName() const
{
     return MOJODB_MEDIADB_BUS_ADDRESS;
}

////////////////////////////////////////////////
// tempDB
TempDbKindConfigurator::TempDbKindConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, MojDbClient& dbClient, string configDirectory)
	: DbKindConfigurator(id, confType, type, busClient, dbClient, configDirectory)
{
}

const char* TempDbKindConfigurator::ServiceName() const
{
	 return MOJODB_TEMPDB_BUS_ADDRESS;
}

bool TempDbKindConfigurator::CanCacheConfiguratorStatus(const std::string& confFile) const
{
	return false;
}

