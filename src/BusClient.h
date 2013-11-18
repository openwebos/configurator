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

#ifndef BUSCLIENT_H_
#define BUSCLIENT_H_

#define MOJ_DEBUG_LOGGING
#ifndef MOJ_DEBUG
#define MOJ_DEBUG
#endif

#include "core/MojReactorApp.h"
#include "core/MojGmainReactor.h"
#include "db/MojDbServiceClient.h"
#include "luna/MojLunaService.h"
#include "Configurator.h"
#include "Flags.h"
#include <vector>

class BusClient : public MojReactorApp<MojGmainReactor>
{
public:
	enum ScanType {
		DBKINDS      = 1 << 0,
		FILECACHE    = 1 << 1,
		DBPERMISSIONS = 1 << 2,
		/*WATCHES      = 1 << 2,*/ // deprecated
		ACTIVITIES   = 1 << 3,
	};
	DECLARE_FLAGS(ScanTypes, ScanType);

	typedef enum {
		None                 = 0,
		DeprecatedDbKind,
	} AdditionalFileType;
	DECLARE_FLAGS(AdditionalFileTypes, AdditionalFileType);

	BusClient();
	virtual ~BusClient();

	MojDbClient&						GetDbClient();
	MojLogger&							GetLogger();
	MojRefCountedPtr<MojServiceRequest>	CreateRequest();
	MojRefCountedPtr<MojServiceRequest>	CreateRequest(const char *forgedAppId);
	virtual MojErr						open();
	virtual MojErr						handleArgs(const StringVec& args);
	void								ConfiguratorComplete(Configurator *configurator);
	void								ConfiguratorComplete(int configuratorIndex);

private:
	typedef enum {
		Application,
		Service,
	} PackageType;

	typedef enum {
		System,
		ThirdParty,
	} PackageLocation;

	typedef enum {
		ForceRescan, /// force all the configurators to run
		LazyScan, /// only run those configurators that haven't run yet
	} ConfigurationMode;

	class BusMethods : public MojService::CategoryHandler
	{
	public:
		BusMethods(BusClient& client, MojLogger& log);

	private:
		bool WorkEnqueued(Callback callback, MojServiceMessage *msg, MojObject& payload);

		MojErr Run(MojServiceMessage* msg, MojObject& payload);
		MojErr Rescan(MojServiceMessage* msg, MojObject& payload);
		MojErr Scan(MojServiceMessage* msg, MojObject& payload);
		MojErr ScanRequest(MojServiceMessage* msg, MojObject& payload, ConfigurationMode confmode);
		MojErr Unconfigure(MojServiceMessage* msg, MojObject& payload);

		BusClient& m_client;
		MojLogger& m_log;
	};

	struct PendingWork {
		BusMethods *instance;
		MojService::CategoryHandler::Callback callback;
		MojRefCountedPtr<MojServiceMessage> msg;
		MojObject payload;
	};

	typedef std::vector<PendingWork> PendingWorkCollection;

	static const char* const SERVICE_NAME;
	static const char* const ROOT_BASE_DIR;
	static const char* const OLD_DB_KIND_DIR; //deprecated
	static const char* const DB_KIND_DIR;
	static const char* const DB_PERMISSIONS_DIR;
    static const char* const MEDIADB_KIND_DIR;
    static const char* const MEDIADB_PERMISSIONS_DIR;
	static const char* const TEMPDB_KIND_DIR;
	static const char* const TEMPDB_PERMISSIONS_DIR;
	static const char* const FILE_CACHE_CONFIG_DIR;
	static const char* const ACTIVITY_CONFIG_DIR;
	static const char* const FIRST_BOOT_FILE;
	static const char* const BASE_CRYPTOFS;
	static const char* const BASE_ROOT;
	static const char* const BASE_PALM_OFFSET;
	static const char* const APPS_DIR;
	static const char* const SERVICES_DIR;
	static const char* const CONF_SUBDIR;

	typedef MojReactorApp<MojGmainReactor> Base;
	typedef MojRefCountedPtr<Configurator> ConfiguratorPtr;
	typedef std::vector<ConfiguratorPtr> ConfiguratorCollection;

	std::string appConfDir(const MojString& appId, PackageType type, PackageLocation location);

	static Configurator::ConfigType PackageTypeToConfigType(PackageType type)
	{
		switch(type) {
		case Application:
			return Configurator::ConfigApplication;
		case Service:
			return Configurator::ConfigService;
		default:
			assert(false);
			return Configurator::ConfigUnknown;
		}
	}

	void Run(ScanTypes bitmask);
	void Scan(ConfigurationMode confmode, const MojString& appid, PackageType type, PackageLocation location);
	void ScanDir(const MojString& id, Configurator::RunType scanType, const std::string &dirBase, ScanTypes bitmask, Configurator::ConfigType configType, AdditionalFileTypes types = None);
	void Unconfigure(const MojString& appId, PackageType type, PackageLocation location, ScanTypes bitmask);

	void RunNextConfigurator();
	void ScheduleShutdown();

	static gboolean IterateConfiguratorsCallback(gpointer data);
	static gboolean ShutdownCallback(gpointer data);

	void ConfiguratorComplete(ConfiguratorCollection::iterator configurator);

	MojLogger					 m_log;
	MojLunaService				 m_service;
	MojDbServiceClient			 m_dbClient;
	MojDbServiceClient			 m_mediaDbClient;
    MojDbServiceClient           m_tempDbClient;
	ConfiguratorCollection m_configurators;
	size_t m_configuratorsCompleted;
	MojRefCountedPtr<BusMethods> m_methods;
	bool						 m_launchedAsService;
	MojRefCountedPtr<MojServiceMessage> m_msg;
	bool m_shuttingDown;
	bool m_wrongAplication;
	PendingWorkCollection m_pending;
	unsigned int m_timerTimeout;
};

DECLARE_OPERATORS_FOR_FLAGS(BusClient::ScanTypes)
DECLARE_OPERATORS_FOR_FLAGS(BusClient::AdditionalFileTypes)

#endif /* BUSCLIENT_H_ */
