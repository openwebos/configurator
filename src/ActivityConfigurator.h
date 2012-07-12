// @@@LICENSE
//
//      Copyright (c) 2009-2012 Hewlett-Packard Development Company, L.P.
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

#ifndef ACTIVITYCONFIGURATOR_H_
#define ACTIVITYCONFIGURATOR_H_

#include "Configurator.h"

class ActivityConfigurator : public Configurator
{
public:
	ActivityConfigurator(const std::string& id, ConfigType confType, RunType type, BusClient& busClient, std::string configDirectory);
	virtual ~ActivityConfigurator();

protected:
	virtual MojErr ProcessConfig(const std::string& filePath, MojObject& params);
	virtual MojErr ProcessConfigRemoval(const std::string &filePath, MojObject &json);

	virtual const char* ConfiguratorName() const;
	virtual const char* ServiceName() const;
	virtual ConfiguratorCallback* CreateCallback(const std::string &filePath);
	virtual bool CanCacheConfiguratorStatus(const std::string &confFile) const;

private:
	static const char* const ACTIVITYMGR_BUS_ADDRESS;
	static const char* const ACTIVITYMGR_CREATE_METHOD;
	static const char* const ACTIVITYMGR_REMOVE_METHOD;
	static const char* const ACTIVITY;
	static const char* const CREATOR;
	static const char* const APPLICATION_ID;
	static const char* const SERVICE_ID;
	static const char* const NAME;
	static const char* const ACTIVITY_NAME;
	static const char* const FIRST_USE_SAFE;
	static const char* const APP_DIR;
	static const char* const SERVICE_DIR;
	static const char* const FIRST_USE_FLAG;
	static const char* const FIRST_USE_PROFILE_FLAG;

	bool m_firstUseOnly;

	friend class ActivityConfigureResponse;
};

#endif /* ACTIVITYCONFIGURATOR_H_ */
