#ifndef VIETTELSDK_DEBUGGER_H_
#define VIETTELSDK_DEBUGGER_H_

#include "ViettelSDK_Device_Management.h"

void writeLog(struct ViettelSDK *self, enum LogType log_type,
		char log_content[], bool default_log);

void logStartBanner(struct ViettelSDK *self);

#endif /* VIETTELSDK_DEBUGGER_H_ */
