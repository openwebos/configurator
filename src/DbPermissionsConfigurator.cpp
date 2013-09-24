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
#include "DbPermissionsConfigurator.h"
#include "BusClient.h"

using namespace std;

static const char *MOJODB_DB_BUS_ADDRESS = "com.palm.db";
static const char *MOJODB_TEMPDB_BUS_ADDRESS = "com.palm.tempdb";
static const char *MOJODB_MEDIADB_BUS_ADDRESS = "com.webos.mediadb";
static const char *MOJODB_PUTPERMISSIONS_METHOD = "putPermissions";

const char* DbPermissionsConfigurator::ConfiguratorName() const
{
	 return "DbPermissionsConfigurator";
}

const char* DbPermissionsConfigurator::ServiceName() const
{
	 return MOJODB_DB_BUS_ADDRESS;
}

DbPermissionsConfigurator::DbPermissionsConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, MojDbClient& dbClient, string configDirectory)
: Configurator(id, confType, type, busClient, configDirectory),
  m_dbClient(dbClient)
{

}

DbPermissionsConfigurator::~DbPermissionsConfigurator()
{

}

MojErr DbPermissionsConfigurator::ProcessConfig(const string& filePath, MojObject& permissions)
{
	MojLogTrace(m_log);

	std::string owner;
	MojObject perms;

	owner = ParentId(filePath);
	perms.put("permissions", permissions);

	// for third-party packages, we set the appid on the service request
	// so that mojodb does things correctly.  root config files aren't split up
	// in a per-service/app directory way (though they should be like activitymanager)
	MojRefCountedPtr<MojServiceRequest> request;
	if (!owner.empty())
		request = m_busClient.CreateRequest(owner.c_str());
	else
		request = m_busClient.CreateRequest();
	return request->send(CreateCallback(filePath)->m_slot, ServiceName(), MOJODB_PUTPERMISSIONS_METHOD, perms);
}

MojErr DbPermissionsConfigurator::ProcessConfigRemoval(const string& filePath, MojObject& params)
{
	MojLogTrace(m_log);

	// This is the fix for NOV-129951.
	// This operation is a mojodb no-op since the permissions are removed when the associated
	// kind gets deleted in mojodb.  (Previously the code spit out a warning here and
	// returned an error to the caller).
	UnmarkConfigured(filePath);

	return MojErrInProgress;
}

////////////////////////////////////////////////
// mediaDB
MediaDbPermissionsConfigurator::MediaDbPermissionsConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, MojDbClient& dbClient, string configDirectory)
    : DbPermissionsConfigurator(id, confType, type, busClient, dbClient, configDirectory)
{
}

const char* MediaDbPermissionsConfigurator::ServiceName() const
{
     return MOJODB_MEDIADB_BUS_ADDRESS;
}


////////////////////////////////////////////////
// tempDB
TempDbPermissionsConfigurator::TempDbPermissionsConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, MojDbClient& dbClient, string configDirectory)
	: DbPermissionsConfigurator(id, confType, type, busClient, dbClient, configDirectory)
{
}

const char* TempDbPermissionsConfigurator::ServiceName() const
{
	 return MOJODB_TEMPDB_BUS_ADDRESS;
}

bool TempDbPermissionsConfigurator::CanCacheConfiguratorStatus(const std::string& confFile) const
{
	return false;
}

