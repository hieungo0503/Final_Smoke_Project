#ifndef VIETTELSDK_POWER_MANAGEMENT_H_
#define VIETTELSDK_POWER_MANAGEMENT_H_

#include "ViettelSDK_Device_Management.h"

/* Forward declaration */
struct ViettelSDK;

/* MCU */
void setupSleepTimer(struct ViettelSDK *self);

void sleepMCU(struct ViettelSDK *self, uint32_t period_in_seconds);

void Enter_Stop1Mode(struct ViettelSDK *self, UART_HandleTypeDef *huart);

void Exit_Stop1Mode(struct ViettelSDK *self, UART_HandleTypeDef *huart1, UART_HandleTypeDef *huart2);

/* Module */
StatusType configureWakeupIndication(struct ViettelSDK *self, bool type);

StatusType configureEPSNetworkRegistration(struct ViettelSDK *self,
		uint8_t type);

StatusType configureSlowClock(struct ViettelSDK *self, uint8_t type);

StatusType configurePSM(struct ViettelSDK *self, uint8_t type,
		char requestedPeriodicTAU[], char requestedActiveTime[]);

StatusType TurnOffPSM(struct ViettelSDK *self, uint8_t type);

#endif /* VIETTELSDK_POWER_MANAGEMENT_H_ */
