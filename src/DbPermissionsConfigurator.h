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

#ifndef DBPERMISSIONSCONFIGURATOR_H_
#define DBPERMISSIONSCONFIGURATOR_H_

#include "db/MojDbClient.h"
#include "Configurator.h"

class DbPermissionsConfigurator : public Configurator
{
public:
	DbPermissionsConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, MojDbClient& dbClient, std::string configDirectory);
	virtual ~DbPermissionsConfigurator();

protected:
	virtual MojErr ProcessConfig(const std::string& filePath, MojObject& permission);
	virtual MojErr ProcessConfigRemoval(const std::string &filePath, MojObject &json);

	virtual const char* ConfiguratorName() const;
	virtual const char* ServiceName() const;

private:
	MojDbClient& m_dbClient;
};

class TempDbPermissionsConfigurator : public DbPermissionsConfigurator
{
public:
	TempDbPermissionsConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, MojDbClient& dbClient, std::string configDirectory);

protected:
	virtual const char* ServiceName() const;

	virtual bool CanCacheConfiguratorStatus(const std::string& confFile) const;
};

#endif /* DBPERMISSIONSCONFIGURATOR_H_ */
