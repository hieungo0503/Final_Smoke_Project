#ifndef VIETTELSDK_MEMORY_MANAGEMENT_H_
#define VIETTELSDK_MEMORY_MANAGEMENT_H_

#include "ViettelSDK_Device_Management.h"

// Forward declaration of ViettelSDK struct
struct ViettelSDK;

void readFLASHPreviousFlowData(struct ViettelSDK *self, uint32_t address);

void writeFLASHFlowData(struct ViettelSDK *self, uint32_t start_page,
		uint32_t end_page, uint32_t data32);

#endif /* VIETTELSDK_MEMORY_MANAGEMENT_H_ */
