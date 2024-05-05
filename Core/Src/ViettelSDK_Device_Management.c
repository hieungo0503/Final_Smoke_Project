#include "ViettelSDK_Device_Management.h"

void initializeSDK(struct ViettelSDK *self, UART_HandleTypeDef *debugger_uart,
		UART_HandleTypeDef *module_uart, DMA_HandleTypeDef *module_uart_hdma,
		GPIO_TypeDef *pwr_pin_GPIOx, uint16_t pwr_pin_GPIO_Pin)
{
	self->detail_log = DETAIL_LOG_SETUP;
	self->data_list = NULL;
	self->addDataSuccessfully = true;
	self->sleep = false;
	self->StopMode = 0;
	self->mqtt_params.receiveSubcribeTimeout = RECEIVE_SUBSCRIBE_TIMEOUT;
	self->warming_up_counter = WARMING_UP_COUNT;
	self->passively_listen = false;
	self->successfully_initialized = false;
	self->attempt_count = 1;
	strcpy(self->delimiter, ",\r\n");

	self->debugger_uart = debugger_uart;
	self->module_uart = module_uart;
	self->module_uart_hdma = module_uart_hdma;

	/* Initialize Power Pin Wake Up */
	self->power_pin.pin_GPIOx = pwr_pin_GPIOx;
	self->power_pin.pin_GPIO_Pin = pwr_pin_GPIO_Pin;
	self->GPIO_InitStruct.Pin = self->power_pin.pin_GPIO_Pin;
	self->GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	self->GPIO_InitStruct.Pull = GPIO_NOPULL;
	self->GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/* Create dynamic array log content*/
	self->log_content = (char*) malloc(LOG_CONTENT_SIZE * sizeof(char));
	if (!self->log_content)
	{
		while (1);
	}

	/* Create dynamic array command */
	self->command = (char*) malloc(COMMAND_SIZE * sizeof(char));
	if (!self->command)
	{
		while (1);
	}

	self->response_main_buffer = (char*) malloc(
	RESPONSE_MAIN_BUFFER_SIZE * sizeof(char));
	if (!self->response_main_buffer)
	{
		while (1);
	}

	self->mqtt_params.message_buffer = (char*) malloc(
	MESSAGE_SIZE * sizeof(char));
	if (!self->mqtt_params.message_buffer)
	{
		while (1);
	}

	self->mqtt_params.message = (char*) malloc(MESSAGE_SIZE  * 2 * sizeof(char));
	if (!self->mqtt_params.message)
	{
		while (1);
	}
	self->coap_params.message_buffer = (char*) malloc(
	MESSAGE_SIZE * sizeof(char));
	if (!self->coap_params.message_buffer)
	{
		while (1);
	}

	self->coap_params.message = (char*) malloc(MESSAGE_SIZE * 2 * sizeof(char));
	if (!self->coap_params.message)
	{
		while (1);
	}

	/* Print start banner */
	logStartBanner(self);

	readFLASHPreviousFlowData(self, FLASH_USER_START_ADDR);
	if (WATCHDOG_TIMER)
	{
//		HAL_IWDG_Init(&hiwdg);
	}

	if (!validateUARTPorts(self))
	{
		writeLog(self, LOG_ERROR, "SDK is NOT initialized.", true);
	}
	else
	{
		writeLog(self, LOG_INFO, "SDK is initialized.", true);
		self->successfully_initialized = true;
	}

	self->period_timer = HAL_GetTick();
	self->successfully_initialized = true;
	//setupSleepTimer(self);					//Chekcout
	self->run_first_time = true;
	self->stage = 1;
}

void connectToPlatform(struct ViettelSDK *self, struct Smoke_Data *smoke_hler)
{

	if (self->run_first_time
			|| HAL_GetTick() - self->period_timer >= CONNECTING_PERIOD_MS)
	{
		/* Header */
		writeLog(self, LOG_EMPTY, "\n", true);
		sprintf(self->log_content, "Start connecting to Platform. Attempt: %u.",
				self->attempt_count);
		writeLog(self, LOG_INFO, self->log_content, true);
		self->period_timer = HAL_GetTick();

		/* Check whether data are properly added */
		if (!self->addDataSuccessfully)
		{
			resetMCU(self);
		}

		/* Main Flow */
		if (!self->successfully_initialized)
		{
			writeLog(self, LOG_ERROR,
					"SDK is NOT initialized. MCU will be reset...", true);
			HAL_Delay(3000);
			resetMCU(self);
		}
		else
		{
//			wakeUpModule(self);
			mainFlow(self, smoke_hler);
		}

		/* Footer */
		self->attempt_count++;
		writeLog(self, LOG_EMPTY, "\n\n", true);
		self->period_timer = HAL_GetTick();
		self->run_first_time = false;
	}
}

void setupMQTTParameters(struct ViettelSDK *self, char server[], char port[],
		uint16_t command_timeout_ms, char client_id[], uint8_t version,
		char mqtt_username[], char mqtt_password[])
{

	if (strlen(server) > MQTT_SERVER_MAX_LENGTH)
	{
		sprintf(self->log_content, "<server> length exceeds limitations (%u)",
		MQTT_SERVER_MAX_LENGTH);
		writeLog(self, LOG_ERROR, self->log_content, true);
		while (1);
	}
	strcpy(self->mqtt_params.server, server);

	if (atoi(port) < 0 || atoi(port) > 65535)
	{
		sprintf(self->log_content,
				"<port> length exceeds limitations (0 - 65535)");
		writeLog(self, LOG_ERROR, self->log_content, true);
		while (1);
	}
	strcpy(self->mqtt_params.port, port);

	if (command_timeout_ms
			< 0|| command_timeout_ms > MQTT_MAX_COMMAND_TIMEOUT_MS)
	{
		sprintf(self->log_content,
				"<command_timeout_ms> length exceeds limitations (0 - %u)",
				MQTT_MAX_COMMAND_TIMEOUT_MS);
		writeLog(self, LOG_ERROR, self->log_content, true);
		while (1);
	}
	self->mqtt_params.command_timeout_ms = command_timeout_ms;

	self->mqtt_params.bufsize = MQTT_BUFFER_SIZE;

	if (strlen(client_id) > CLIENT_MAX_LENGTH)
	{
		sprintf(self->log_content,
				"<client_id> length exceeds limitations (%u)",
				CLIENT_MAX_LENGTH);
		writeLog(self, LOG_ERROR, self->log_content, true);
		while (1);
	}
	strcpy(self->mqtt_params.client_id, client_id);

	if (!(version == 3 || version == 4))
	{
		sprintf(self->log_content,
				"<version> value must be 3 (MQTT 3.1) or 4 (MQTT 3.1.1)");
		writeLog(self, LOG_ERROR, self->log_content, true);
		while (1);
	}
	self->mqtt_params.version = version;

	if (strlen(mqtt_username) > MQTT_USERNAME_MAX_LENGTH)
	{
		sprintf(self->log_content,
				"<mqtt_username> length exceeds limitations (%u)",
				MQTT_USERNAME_MAX_LENGTH);
		writeLog(self, LOG_ERROR, self->log_content, true);
		while (1);
	}
	strcpy(self->mqtt_params.mqtt_username, mqtt_username);

	if (strlen(mqtt_password) > MQTT_PASSWORD_MAX_LENGTH)
	{
		sprintf(self->log_content,
				"<mqtt_password> length exceeds limitations (%u)",
				MQTT_PASSWORD_MAX_LENGTH);
		writeLog(self, LOG_ERROR, self->log_content, true);
		while (1);
	}
	strcpy(self->mqtt_params.mqtt_password, mqtt_password);
}

void setupCOAP_Parameters(struct ViettelSDK *self,char server[], uint16_t port, uint8_t client_id){
	strcpy(self->coap_params.server, server);
	self->coap_params.port = port;
	self->coap_params.client_id = client_id;
}
struct CommandResponse sendCommand(struct ViettelSDK *self, char *command,
		uint8_t count, uint32_t timeout)
{
	uint8_t max_count = count;
	count = 0;
	while (count++ < max_count)
	{
		if (WATCHDOG_TIMER)
		{
//			HAL_IWDG_Refresh(&hiwdg);
		}

		self->command_response.status = STATUS_UNKNOWN;
		clearMainBuffer(self);
		self->passively_listen = false;
		self->psm_timer = HAL_GetTick();

		/* Start the DMA again */
		resetDMAInterrupt(self);

		sprintf(self->log_content,
				"\n\n========= SEND COMMAND: %s | Attempt: %u/%u =========",
				command, count, max_count);
		writeLog(self, LOG_EMPTY, self->log_content, true);

		strcpy(self->command_response.response, "");
		HAL_UART_Transmit(self->module_uart, (uint8_t*) command,
				(uint16_t) strlen(command), timeout);
		HAL_UART_Transmit(self->module_uart, (uint8_t*) "\r\n",
				(uint16_t) strlen("\r\n"), timeout);

		self->command_response.status = STATUS_TIMEOUT;
		self->response_received = false;
		self->command_timer = HAL_GetTick();

		while (HAL_GetTick() - self->command_timer <= timeout)
		{
			if (self->response_received == true)
			{
				break;
			}
		}

		if (self->command_response.status == STATUS_SUCCESS)
		{
			break;
		}
		clearMainBuffer(self);
		HAL_Delay(COMMAND_DELAY_MS);

	}

	return self->command_response;
}

void clearCommand(struct ViettelSDK *self)
{
	for (int i = 0; i < COMMAND_SIZE; i++)
	{
		self->command[i] = 0;
	}
}

void clearMainBuffer(struct ViettelSDK *self)
{
	for (int i = 0; i < RESPONSE_MAIN_BUFFER_SIZE; i++)
	{
		self->response_main_buffer[i] = 0;
	}
	self->old_position = 0;
	self->new_position = 0;
}

void saveResponse(struct ViettelSDK *self, UART_HandleTypeDef *huart,
		uint16_t size)
{
	/* Check whether MCU is sleep */

	if (self->module_uart->Instance == huart->Instance)
	{
		char *ptr;

		// Update the last position before copying new data
		self->old_position = self->new_position;

		/* If the data in large and it is about to exceed the buffer size, Debugger indicates error */
		if (self->old_position + size > RESPONSE_MAIN_BUFFER_SIZE)
		{
			sprintf(self->log_content,
					"Response too large! Response Receive Buffer and Response Main Buffer will be cleared!");
			writeLog(self, LOG_ERROR, self->log_content, false);
			clearMainBuffer(self);
		}

		/* If the current position + new data size is less than the main buffer
		 * we will simply copy the data into the buffer and update the position
		 */
		else
		{
			memcpy((uint8_t*) self->response_main_buffer + self->old_position,
					self->response_receive_buffer, size);

			self->new_position = self->old_position + size;
		}

		/* Start thce DMA again */
		resetDMAInterrupt(self);

		/* Checking passively listen */
		if (self->passively_listen)
		{
			HAL_PWR_DisableSleepOnExit();
			for (int i = 0; i < PASSIVE_RESPONSE_LENGTH; i++)
			{
				ptr = strstr(self->response_main_buffer,
						PASSIVE_RESPONSE_SIGN[i]);
				if (ptr != NULL)
				{
					writeLog(self, LOG_EMPTY, self->response_main_buffer,
					true);
					sprintf(self->log_content, "Received an PASSIVE command!");
					writeLog(self, LOG_INFO, self->log_content, true);
					self->command_response.status = STATUS_SUCCESS;
					clearMainBuffer(self);
					self->response_received = true;

					if (i == 0)
					{
						sprintf(self->log_content, "Power off module!");
					}
					else if (i == 1)
					{
						sprintf(self->log_content, "Entered PSM mode!");
						self->sleep = true;
					}
					else if (i == 2)
					{
						sprintf(self->log_content, "Exited PSM mode!");
						self->sleep = false;
					}
					else if (i == 3)
					{
						sprintf(self->log_content,
								"Received subcribe messages from platform!");
						self->mqtt_params.receiveSubcribe = true;
					}
					writeLog(self, LOG_INFO, self->log_content, true);
					self->passively_listen = false;
					return;
				}
			}
			clearMainBuffer(self);
			return;
		}

		for (int i = 0; i < SUCCESS_RESPONSE_LENGTH; i++)
		{
			ptr = strstr(self->response_main_buffer, SUCCESS_COMMAND_SIGN[i]);
			if (ptr != NULL)
			{
				self->command_response.status = STATUS_SUCCESS;
				strcpy(self->command_response.response,
						self->response_main_buffer);
				writeLog(self, LOG_EMPTY, self->response_main_buffer,
				true);
				sprintf(self->log_content, "Received an OK command!");
				writeLog(self, LOG_INFO, self->log_content, false);
				clearMainBuffer(self);
				self->response_received = true;
				return;
			}
		}

		/* Checking completion of command */
		for (int i = 0; i < ERROR_RESPONSE_LENGTH; i++)
		{
			ptr = strstr(self->response_main_buffer, ERROR_COMMAND_SIGN[i]);
			if (ptr != NULL)
			{
				self->command_response.status = STATUS_ERROR;
				writeLog(self, LOG_EMPTY, self->response_main_buffer,
				true);
				sprintf(self->log_content, "Received an ERROR command!");
				writeLog(self, LOG_ERROR, self->log_content, false);
				clearMainBuffer(self);
				self->response_received = true;
				return;
			}
		}

		return;
	}
}

void mainFlow(struct ViettelSDK *self, struct Smoke_Data *smoke_hler)
{
	int8_t try;

	/* Stage 1 */
	while (self->stage == 1)
	{

		/* AT */
		if (checkModule(self) != STATUS_SUCCESS)
		{
//			wakeUpModule(self);
			continue;
		}

		powerOffModule(self,false);

		/* ATEx */
		if (setCommandEchoMode(self, false) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CPIN? */
		if (checkSIM(self) != STATUS_SUCCESS)
		{
//			powerOffModule(self, false);
//
//			/* Sleep */
//			sleepMCU(self, SLEEP_INTERVAL);
//			resetMCU(self);
			continue;
		}

		/* Deregister PSM with cell */
		/* AT+QNBIOTEVENT */
		if (configureWakeupIndication(self, 0) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CEREG */
		if (configureEPSNetworkRegistration(self, 2) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+QSCLK */
		{
		if (configureSlowClock(self, 0) != STATUS_SUCCESS)
			continue;
		}
		/* Deregister PSM with cell */

//		if (setControlTheDataOutputFormat(self) != STATUS_SUCCESS)
//		{
//			continue;
//		}

		/* AT+CMEE */
		if (setReportMobileEquipmentError(self, 2) != STATUS_SUCCESS)
		{
			continue;
		}

		/* ATI */
		if (displayProductID(self) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CGMI */
		if (requestManufacturerID(self) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CGMM */
		if (requestModelID(self) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CGMR */
		if (requestTARevisionIdentification(self) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CGSN */
		if (requestIMEI(self) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CIMI */
		if (requestInternationalMobileSubscriberIdentity(self)
				!= STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+QCCID */
		if (readCCID(self) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+QBAND */
		if (getAndSetMobileOperationBand(self) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CSQ */
		if (readSignalQualityReport(self) != STATUS_SUCCESS)
		{
			continue;
		}


		HAL_Delay(STAGE_DELAY_MS);
		self->stage = 2;
	}

	/* Stage 2 */
	try = 3;
	while (self->stage == 2)
	{
		while (try--)
		{
			if (try < 2)
			{
				//wakeUpModule(self);
				HAL_Delay(1000);
			}

			/* AT+CEREG? */
			if ((readEPSNetworkRegistrationStatus(self, true) == STATUS_SUCCESS)
					&& (self->stat == 1))
			{
				self->stage = 3;
				break;
			}
			HAL_Delay(5000);
		}

		if (try == -1)
		{
			/* Cannot register automatically. Try to register manually 3 times  */
			try = 3;
			while (try--)
			{
				sprintf(self->log_content,
						"Cannot register automatically. Try to register manually 3 times. Attempt: %u/3.\n",
						3 - try);
				writeLog(self, LOG_WARNING, self->log_content, true);

				/* AT+CFUN= */
				if (setPhoneFunctionality(self, 0) != STATUS_SUCCESS)
				{
					continue;
				}

				/* AT+QCGDEFCONT */
				if (setDefaultPSDConnection(self) != STATUS_SUCCESS)
				{
					continue;
				}

				/* AT+CFUN= */
				if (setPhoneFunctionality(self, 1) != STATUS_SUCCESS)
				{
					continue;
				}

				HAL_Delay(3000);

				/* AT+CEREG? */
				if ((readEPSNetworkRegistrationStatus(self, true)
						== STATUS_SUCCESS) && (self->stat == 1))
				{
					self->stage = 3;
					break;
				}
			}

			if (try == -1)
			{
				/* Fail to register manually */
				sprintf(self->log_content, "Fail to register manually!");
				writeLog(self, LOG_WARNING, self->log_content, true);
				powerOffModule(self, false);

				/* Sleep */
				sleepMCU(self, SLEEP_INTERVAL);
				resetMCU(self);
			}
		}
	}

	/* Stage 3 */
	while (self->stage == 3)
	{

		try = 3;
		while (try--)
		{
			/* AT+CGDCONT= */
			if (readDynamicParamPDPContext(self, false) != STATUS_SUCCESS)
			{
				continue;
			}

//			/* AT+CTZU= */
//			if (setAutoTimeUpdate(self) != STATUS_SUCCESS)
//			{
//				continue;
//			}

			/* AT+CLCK=? */
			if (readClock(self, false) != STATUS_SUCCESS) 			//take time here
			{
				continue;
			}

			/* AT+CBC */
			if (readVoltage(self) != STATUS_SUCCESS)
			{
				continue;
			}

			/* AT+QENG? */
			if (readReportNetworkState(self) == STATUS_SUCCESS)				//check to fix at this line
			{
				break;
			}

		}

		if (try == -1)
		{
			powerOffModule(self, false);
			sleepMCU(self, SLEEP_INTERVAL);
			resetMCU(self);
		}

		/* Register PSM with cell */
		/* AT+QNBIOTEVENT */
		if (configureWakeupIndication(self, 1) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CEREG */
		if (configureEPSNetworkRegistration(self, 4) != STATUS_SUCCESS)
		{
			continue;
		}

		/* AT+CSCLK */
		if (configureSlowClock(self, 1) != STATUS_SUCCESS)			// check this line
		{
			continue;
		}

		/* AT+CPSMS */
		if (configurePSM(self, 1, "01011111", "00000001") != STATUS_SUCCESS)
		{
			continue;
		}

		HAL_Delay(3000);

		try = 3;
		while (try--)
		{
			if (try < 2)
			{
				wakeUpModule(self);
				HAL_Delay(1000);
			}

			/* AT+CEREG? */
			if (readEPSNetworkRegistrationStatus(self, true) == STATUS_SUCCESS)
			{
				break;
			}
		}

		if (try == -1)
		{
			powerOffModule(self, false);
			sleepMCU(self, SLEEP_INTERVAL);
			resetMCU(self);
		}

		self->stage = 4;
	}


	if(self->run_first_time)
	{
	addSensorData(self, smoke_hler);

	addBatteryVoltage(self);

	addSimProfile(self);

	addNetworkData(self);

	packData(self);
	}
	else
	{
		changeSensorData(self, smoke_hler);
		changeBatteryVoltage(self);
		changeNetworkData(self);
		packData(self);
	}

	/* Stage 4 */
	while (self->stage == 4)
	{

//		/* AT+CMQNEW? */
//		if(checkCoapConnecttion(self) != STATUS_SUCCESS)
//		{
//			continue;
//		}
//
//		if (self->coap_params.connected)
//		{
//			/* AT+CMQDISCON */
//			if (COAPDisconnect(self) != STATUS_SUCCESS)
//			{
//				continue;
//			}
//		}

//		try = 3;
//		while (try--)
//		{
//			/* AT+QCOAPOPEN */
//			if (newCOAP(self) == STATUS_SUCCESS)
//			{
//				break;
//			}
//		}
//
//		if (try == -1)
//		{
//			/* Fail to create MQTT connection */
//			powerOffModule(self, false);
//			sleepMCU(self, SLEEP_INTERVAL);
//			resetMCU(self);
//		}

		/* AT+CMQCON */
		if (CreateCoAPContex(self) != STATUS_SUCCESS)
		{
			continue;
		}
		if (ConfigCoApMessageOpt(self, "device") != STATUS_SUCCESS)
		{
			continue;
		}

		try = 3;
		while (try--)
		{
			/* AT+CMQPUB */
//			if (sendMQTTPub(self,
//					"messages/ef2885df-341c-44e6-9fbf-a185a36a8f1a/gps", 1, 0,
//					0, self->mqtt_params.message) != STATUS_SUCCESS)
//			{
//				wakeUpModule(self);
//				continue;
//			}

			if (sendCoAPMessage(self, self->coap_params.message, 1, 2) != STATUS_SUCCESS)
			{
				wakeUpModule(self);
				continue;
			}
//			self->passively_listen = true;
//			self->receiver_subscribe_timer = HAL_GetTick();
//			while (!self->mqtt_params.receiveSubcribe)
//			{
//				if (HAL_GetTick() - self->receiver_subscribe_timer
//						>= RECEIVE_SUBSCRIBE_TIMEOUT
//						|| self->mqtt_params.receiveSubcribe)
//				{
//					break;
//				}
//			}
//
//			if (self->mqtt_params.receiveSubcribe)
//			{
//				HAL_Delay(2000);
//				break;
//			}
		}

		if (try == -1)
		{
			/* Not receive response from platform after 3 attempts */
			/* AT+CEREG? */
			if (readEPSNetworkRegistrationStatus(self, true) != STATUS_SUCCESS)
			{
				continue;
			}

			if (self->stat == 1)
			{
				/* Bad signal. Cannot publish messages */
				sprintf(self->log_content,
						"Bad signal. Cannot publish messages");
				writeLog(self, LOG_WARNING, self->log_content, true);
			}
			else if (self->stat == 2)
			{
				sprintf(self->log_content, "Cannot register. Clear PSM.");
				writeLog(self, LOG_WARNING, self->log_content, true);

				/* Clear PSM */
				/* AT+CEREG= */
				if (setEPSNetworkRegistrationStatus(self, true)
						!= STATUS_SUCCESS)
				{
					continue;
				}

				/* AT+CPSMS */
				if (configurePSM(self, 0, "", "") != STATUS_SUCCESS)
				{
					continue;
				}

				/* AT+CSCLK */
				if (configureSlowClock(self, 0) != STATUS_SUCCESS)
				{
					continue;
				}

				/* AT+CPOWD */
				powerOffModule(self, false);

				sleepMCU(self, SLEEP_INTERVAL);
				resetMCU(self);

			}
			else
			{
				sprintf(self->log_content,
						"Unsupport code of EPS Network Registration Status: %u",
						self->stat);
				writeLog(self, LOG_INFO, self->log_content, true);
			}
		}

		/* AT+CMQDISCON */
		if (COAPDisconnect(self) != STATUS_SUCCESS)
		{
			continue;
		}

		self->stage = 1;
	}

	/* Wait module to go to sleep */
//	if (WATCHDOG_TIMER)
//	{
////		HAL_IWDG_Refresh(&hiwdg);
//	}
	self->passively_listen = true;
	self->sleep = false;
	self->psm_timer = HAL_GetTick();
	while (HAL_GetTick() - self->psm_timer <= WAIT_FOR_PSM_MODE)
	{
		if (self->sleep || (HAL_GetTick() - self->psm_timer) > WAIT_FOR_PSM_MODE)
		{
			break;
		}
	}

	//sleepMCU(self, SLEEP_INTERVAL);    //check here

	//Enter STOP mode Here.
	Enter_Stop1Mode(self, self->module_uart, smoke_hler->Somke_uart);

	return; 		//this will return to stage 0

	try = 3;
	while (try--)
	{
		//wakeUpModule(self);
//		SystemClock_Config();
//		HAL_ResumeTick();

		HAL_Delay(1000);
		if (checkModule(self) == STATUS_SUCCESS)
		{
			break;
		}
	}

	if (try == -1)
	{
		/* Fail to connect to module */
		resetMCU(self);
	}

	try = 3;
	while (try--)
	{
		/* AT+CEREG= */
		if (setEPSNetworkRegistrationStatus(self, true) == STATUS_SUCCESS
				|| self->stat == 1)
		{
			/* Still conenct to cell */
			self->stage = 3;
			return;
		}
	}

	if (try == -1)
	{
		/* Clear PSM */
		try = 3;
		while (try--)
		{
			/* AT+CEREG= */
			if (setEPSNetworkRegistrationStatus(self, true) != STATUS_SUCCESS)
			{
				continue;
			}

			/* AT+CPSMS */
			if (configurePSM(self, 0, "", "") != STATUS_SUCCESS)
			{
				continue;
			}

			/* AT+CSCLK */
			if (configureSlowClock(self, 0) != STATUS_SUCCESS)
			{
				continue;
			}

			/* AT+CPOWD */
			powerOffModule(self, false);

			/* Sleep */
			//sleepMCU(self, SLEEP_INTERVAL);
			resetMCU(self);
		}

		if (try == -1)
		{
			/* Fail to clear PSM. Reset MCU. */
			resetMCU(self);
		}

	}
}
