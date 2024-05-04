#ifndef VIETTELSDK_PERIPHERALS_DRIVER_H_
#define VIETTELSDK_PERIPHERALS_DRIVER_H_

#include "ViettelSDK_Device_Management.h"

/* Forward declaration */
struct ViettelSDK;
struct Pin;

void resetMCU(struct ViettelSDK *self);

void pullPin(struct ViettelSDK *self, struct Pin pin, bool up);

void noPullPin(struct ViettelSDK *self, struct Pin pin);

void resetDMAInterrupt(struct ViettelSDK *self);

bool validateUARTPorts(struct ViettelSDK *self);

#endif /* VIETTELSDK_PERIPHERALS_DRIVER_H_ */
