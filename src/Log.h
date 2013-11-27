#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <glib.h>

#include "PmLogLib.h"

/* Logging ********
 * The parameters needed are
 * msgid - unique message id
 * kvcount - count for key-value pairs
 * ... - key-value pairs and free text. key-value pairs are formed using PMLOGKS or PMLOGKFV e.g.)
 * LOG_CRITICAL(msgid, 2, PMLOGKS("key1", "value1"), PMLOGKFV("key2", "%d", value2), "free text message");
 */
#define LOG_CRITICAL(msgid, kvcount, ...) \
PmLogCritical(getactivitymanagercontext(), msgid, kvcount, ##__VA_ARGS__)

#define LOG_ERROR(msgid, kvcount, ...) \
PmLogError(getactivitymanagercontext(), msgid, kvcount,##__VA_ARGS__)

#define LOG_WARNING(msgid, kvcount, ...) \
PmLogWarning(getactivitymanagercontext(), msgid, kvcount, ##__VA_ARGS__)

#define LOG_INFO(msgid, kvcount, ...) \
PmLogInfo(getactivitymanagercontext(), msgid, kvcount, ##__VA_ARGS__)

#define LOG_DEBUG(...) \
PmLogDebug(getactivitymanagercontext(), ##__VA_ARGS__)

#define LOG_TRACE(...) \
PMLOG_TRACE(__VA_ARGS__);

#define MSGID_BUS_CLIENT_ERROR              "BUS_CLIENT_ERROR"
#define MSGID_ACTIVITY_CONFIGURATOR_ERROR   "ACTIVITY_CONFIGURATOR_ERROR"
#define MSGID_ACTIVITY_CONFIGURATOR_WARNING "ACTIVITY_CONFIGURATOR_WARNING"
#define MSGID_FILE_CACHE_CONFIG_WARNING     "FILE_CACHE_CONFIG_WARNING"
#define MSGID_DB_KIND_CONFIG_ERROR          "DB_KIND_CONFIG_ERROR"
#define MSGID_CONFIGURATOR_WARNING          "CONFIGURATOR_WARNING"
#define MSGID_CONFIGURATOR_ERROR            "CONFIGURATOR_ERROR"

extern PmLogContext getactivitymanagercontext();

#endif /* LOG_H_ */
