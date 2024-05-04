#ifndef VIETTELSDK_MODULE_DRIVER_H_
#define VIETTELSDK_MODULE_DRIVER_H_

#include "ViettelSDK_Device_Management.h"

void wakeUpModule(struct ViettelSDK *self);

void powerOffModule(struct ViettelSDK *self, bool urgent);

StatusType checkModule(struct ViettelSDK *self);

StatusType setCommandEchoMode(struct ViettelSDK *self, bool echo_mode);

StatusType readCCID(struct ViettelSDK *self);

StatusType requestIMEI(struct ViettelSDK *self);

StatusType requestTARevisionIdentification(struct ViettelSDK *self);

StatusType readSignalQualityReport(struct ViettelSDK *self);

StatusType setEPSNetworkRegistrationStatus(struct ViettelSDK *self,
		uint8_t code);

StatusType readEPSNetworkRegistrationStatus(struct ViettelSDK *self,
bool save_data);

StatusType setOperatorSelection(struct ViettelSDK *self, uint8_t mode);

StatusType readOperatorSelection(struct ViettelSDK *self);

StatusType setReportMobileEquipmentError(struct ViettelSDK *self, uint8_t mode);

StatusType displayProductID(struct ViettelSDK *self);

StatusType requestManufacturerID(struct ViettelSDK *self);

StatusType getAndSetMobileOperationBand(struct ViettelSDK *self);

StatusType readReportNetworkState(struct ViettelSDK *self);

StatusType requestModelID(struct ViettelSDK *self);

StatusType requestInternationalMobileSubscriberIdentity(struct ViettelSDK *self);

StatusType readpreferredOperatorList(struct ViettelSDK *self);

StatusType setDefaultPSDConnection(struct ViettelSDK *self);

StatusType checkSIM(struct ViettelSDK *self);

StatusType setPhoneFunctionality(struct ViettelSDK *self, uint8_t fun);

StatusType readDynamicParamPDPContext(struct ViettelSDK *self, bool save_data);

StatusType setAutoTimeUpdate(struct ViettelSDK *self);

StatusType enableLocalTimestamp(struct ViettelSDK *self);

StatusType readClock(struct ViettelSDK *self, bool save_data);

StatusType setControlTheDataOutputFormat(struct ViettelSDK *self);

StatusType readVoltage(struct ViettelSDK *self);

#endif /* VIETTELSDK_MODULE_DRIVER_H_ */
