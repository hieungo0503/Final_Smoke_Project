#ifndef VIETTELSDK_DEVICE_MANAGEMENT_H_
#define VIETTELSDK_DEVICE_MANAGEMENT_H_

#include "BM22S2021_1.h"

#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "time.h"

#include "ViettelSDK_Constants.h"
#include "ViettelSDK_Collections.h"
#include "ViettelSDK_ATCommandList.h"
#include "ViettelSDK_Utils.h"

#include "ViettelSDK_Peripherals_Driver.h"
#include "ViettelSDK_Memory_Management.h"
#include "ViettelSDK_OTA.h"
#include "ViettelSDK_Security.h"
#include "ViettelSDK_Data_Management.h"
#include "ViettelSDK_Module_Driver.h"
#include "ViettelSDK_Debugger.h"
#include "ViettelSDK_Network_Management.h"
#include "ViettelSDK_Power_Management.h"

/* Static variable */
static char static_null_string[1] =
{ 0 }; // intentionally left unused

/* Extern variables */
//extern IWDG_HandleTypeDef hiwdg;		//before WdT
extern TIM_HandleTypeDef htim2;

struct ViettelSDK
{
	char deviceID[20];
	char deviceID_Hex[20];

	bool testCase;

	/* Sleep */
	bool sleep;
	bool StopMode;
	char PSM_TAU_WAKEUP[PSM_RECEIVE_TAU_BUFFER_SIZE]; //+QNBIOTEVENT:  "EXIT PSM"
	bool psm_registed;

	uint16_t voltage;

	/* Stage */
	uint8_t stage;

	/* Delay */
	uint32_t general_delay_timer;

	/* Power Pin Wake Up */
	uint8_t warming_up_counter;
	GPIO_InitTypeDef GPIO_InitStruct;
	struct Pin power_pin;
	RTC_HandleTypeDef rtc_timer;
	RTC_TimeTypeDef sTime;
	RTC_AlarmTypeDef sAlarm;

	/* Passive response */
	uint32_t receiver_subscribe_timer;
	uint32_t psm_timer;
	bool passively_listen;

	/* Header/Footer */
	bool run_first_time;
	uint16_t attempt_count;
	uint32_t period_timer;
	bool successfully_initialized;

	/* Commands */
	struct CommandResponse command_response;
	char response_receive_buffer[RESPONSE_RECEIVE_BUFFER_SIZE];
	char *response_main_buffer;
	uint16_t old_position;
	uint16_t new_position;
	uint32_t command_timer;
	bool response_received;
	char *command;
	char *token;
	char delimiter[DELIMITER_SIZE];

	/* Command Attributes */
	Cell cell_data;
	char ccid[CCID_MAX_LENGTH + 1];
	char imei[IMEI_MAX_LENGTH + 1];
	char imsi[IMEI_MAX_LENGTH + 1];
	char ipAddress[20];
	char ta_revision_id[TA_REVISION_ID_MAX_LENGTH + 1];
	uint8_t n;
	uint8_t stat;
	bool echo_mode;
	bool psm_mode, eDRX_mode;
	char requestedPeriodicTAU[9], requestedActiveTime[9], requestedeDRXCycle[9],
			requestedPTW[9];
	char returnedPeriodicTAU[9], returnedActiveTime[9], returnedeDRXCycle[9],
			returnedPTW[9];
	MQTTParams mqtt_params;
	COAPParams coap_params;

	/* Data */
	bool addDataSuccessfully;
	uint32_t previous_pulse;
	struct DataNode *data_list;

	/* Debugger */
	bool detail_log;
	char *log_content;

	/* Peripherals */
	UART_HandleTypeDef *debugger_uart;
	UART_HandleTypeDef *module_uart;
	DMA_HandleTypeDef *module_uart_hdma;
};

void initializeSDK(struct ViettelSDK *self, UART_HandleTypeDef *debugger_uart,
		UART_HandleTypeDef *module_uart, DMA_HandleTypeDef *module_uart_hdma,
		GPIO_TypeDef *pwr_pin_GPIOx, uint16_t pwr_pin_GPIO_Pin);

void connectToPlatform(struct ViettelSDK *self, struct Smoke_Data *smoke_hler);

void setupMQTTParameters(struct ViettelSDK *self, char server[], char port[],
		uint16_t command_timeout_ms, char client_id[], uint8_t version,
		char mqtt_username[], char mqtt_password[]);

struct CommandResponse sendCommand(struct ViettelSDK *self, char *command,
		uint8_t count, uint32_t timeout);

void clearCommand(struct ViettelSDK *self);

void clearMainBuffer(struct ViettelSDK *self);

void saveResponse(struct ViettelSDK *self, UART_HandleTypeDef *huart,
		uint16_t size);

void mainFlow(struct ViettelSDK *self, struct Smoke_Data *smoke_hler);

void setupCOAP_Parameters(struct ViettelSDK *self,char server[], uint16_t port, uint8_t client_id);

#endif /* VIETTELSDK_DEVICE_MANAGEMENT_H_ */
