#include "ViettelSDK_Power_Management.h"

/* MCU */
void setupSleepTimer(struct ViettelSDK *self)
{

	/* Enable the LSI clock */
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/* Select the LSI as the RTC clock source */
	RCC_PeriphCLKInitTypeDef RCC_RTCCLKInitStruct;
	RCC_RTCCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	RCC_RTCCLKInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	if (HAL_RCCEx_PeriphCLKConfig(&RCC_RTCCLKInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/* Enable the RTC clock */
	__HAL_RCC_RTC_ENABLE();

	/* Initialize RTC Only */
	self->rtc_timer.Instance = RTC;
//	self->rtc_timer.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
//	self->rtc_timer.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
	if (HAL_RTC_Init(&self->rtc_timer) != HAL_OK)
	{
		Error_Handler();
	}

	sprintf(self->log_content, "Setup sleep timer successfully.");
	writeLog(self, LOG_INFO, self->log_content, true);
}


void InnitStop1Uart(UART_HandleTypeDef *huart)
{		/* make sure that no UART transfer is on-going */
	  while(__HAL_UART_GET_FLAG(huart, USART_ISR_BUSY) == SET);
	  /* make sure that UART is ready to receive
	   * (test carried out again later in HAL_UARTEx_StopModeWakeUpSourceConfig) */
	  while(__HAL_UART_GET_FLAG(huart, USART_ISR_REACK) == RESET);

	  /* set the wake-up event:
	   * specify wake-up on RXNE flag */
	  WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_READDATA_NONEMPTY;
	  if (HAL_UARTEx_StopModeWakeUpSourceConfig(huart, WakeUpSelection)!= HAL_OK)
	  {
	    Error_Handler();
	  }
	  /* Enable the UART Wake UP from STOP1 mode Interrupt */
	  __HAL_UART_ENABLE_IT(huart, UART_IT_WUF);
	  //Enable Receive IT
}

void Enter_Stop1Mode(struct ViettelSDK *self, UART_HandleTypeDef *huart1, UART_HandleTypeDef *huart2)
{
	  self->StopMode = 1;

	  InnitStop1Uart(huart1);
	  InnitStop1Uart(huart2);

	  HAL_UARTEx_EnableStopMode(huart1);
	  HAL_UARTEx_EnableStopMode(huart2);

	    HAL_DisableDBGStopMode();
		//HAL_EnableDBGStopMode();

	  resetDMAforPSM(self);


      HAL_PWR_EnableSleepOnExit();
	  /* enter STOP1 mode */
	  HAL_PWREx_EnterSTOP1Mode(PWR_STOPENTRY_WFI);
}

void Exit_Stop1Mode(struct ViettelSDK *self, UART_HandleTypeDef *huart1, UART_HandleTypeDef *huart2)
{
	SystemClock_Config();
	//	HAL_UARTEx_DisableStopMode(huart1);
	//HAL_UARTEx_DisableStopMode(huart2);
	HAL_PWR_DisableSleepOnExit();
}

void sleepMCU(struct ViettelSDK *self, uint32_t period_in_seconds)
{
	/* Set the RTC time to 0 seconds */
	self->sTime.Hours = 0;
	self->sTime.Minutes = 0;
	self->sTime.Seconds = 0;
	if (HAL_RTC_SetTime(&self->rtc_timer, &self->sTime, RTC_FORMAT_BIN)
			!= HAL_OK)
	{
		Error_Handler();
	}

	uint8_t hours, minutes, seconds;

	/* Parse seconds to hours, minutes and seconds */
	if (period_in_seconds < 86400)
	{
		hours = (period_in_seconds / 3600);
		minutes = (period_in_seconds - (3600 * hours)) / 60;
		seconds = (period_in_seconds - (3600 * hours) - (minutes * 60));
	}
	else
	{
		hours = 23;
		minutes = 59;
		seconds = 59;
	}

	/* Set the RTC alarm to trigger after 100 seconds */
	self->sAlarm.AlarmTime.Hours = hours;
	self->sAlarm.AlarmTime.Minutes = minutes;
	self->sAlarm.AlarmTime.Seconds = seconds;
	self->sAlarm.Alarm = RTC_ALARM_A;

	sprintf(self->log_content,
			"Triggered timer with period %u hours %u minutes %u seconds.\n",
			hours, minutes, seconds);
	writeLog(self, LOG_INFO, self->log_content, true);

	self->passively_listen = true;

	/* Sleep */
	sprintf(self->log_content, "MCU IS SLEEPING...\n");
	writeLog(self, LOG_INFO, self->log_content, true);

	if (WATCHDOG_TIMER)
	{
		HAL_IWDG_Refresh(&hiwdg);
//		HAL_TIM_Base_Start_IT(&htim1);
	}

	if (HAL_RTC_SetAlarm_IT(&self->rtc_timer, &self->sAlarm, RTC_FORMAT_BIN)
			!= HAL_OK)
	{
		Error_Handler();
	}

	HAL_SuspendTick();
	HAL_PWR_EnableSleepOnExit();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();

	if (WATCHDOG_TIMER)
	{
		HAL_TIM_Base_Stop_IT(&htim2);
	}

	sprintf(self->log_content, "MCU IS WAKING...\n");
	writeLog(self, LOG_INFO, self->log_content, true);
}

/* Module */
StatusType configureWakeupIndication(struct ViettelSDK *self, bool type)
{
	/* AT+QNBIOTEVENT */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%d,1", CONFIG_WAKEUP_INDICATION_AT_CMD, type);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Config Wakeup Indication is %s.",
			getStatusTypeString(output_status));
	if (type)
	{
		sprintf(self->log_content, "Turn ON wakeup indication.");
	}
	else
	{
		sprintf(self->log_content, "Turn OFF wakeup indication.");
	}

	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType configureEPSNetworkRegistration(struct ViettelSDK *self,
		uint8_t type)
{
	/* AT+CEREG */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%u", EPS_NETWORK_REGISTRATION_STATUS, type);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Config EPS Network Registration is %s.",
			getStatusTypeString(output_status));
	if (type == 0)
	{
		sprintf(self->log_content,
				"Disable network registration unsolicited result code.");
	}
	else if (type == 1)
	{
		sprintf(self->log_content,
				"Enable network registration unsolicited result code.");
	}
	else if (type == 2)
	{
		sprintf(self->log_content,
				"Enable network registration and location information unsolicited result code.");
	}
	else if (type == 3)
	{
		sprintf(self->log_content,
				"Enable network registration, location information and EMM cause value information unsolicited result code.");
	}
	else if (type == 4)
	{
		sprintf(self->log_content,
				"Enable network registration and location information unsolicited result code for UE wants PSM.");
	}
	else if (type == 5)
	{
		sprintf(self->log_content,
				"Enable network registration, location information and EMM cause value information unsolicited result code for UE wants PSM.");
	}

	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType configureSlowClock(struct ViettelSDK *self, uint8_t type)
{
	/* AT+QSCLK */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%u", CONFIG_SLOW_CLOCK_AT_CMD, type);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Config Slow Clock is %s.",
			getStatusTypeString(output_status));
	if (type == 0)
	{
		sprintf(self->log_content,
				"Disable slow clock, module will not enter sleep mode.");
	}
	else if (type == 1)
	{
		sprintf(self->log_content,
				"Enable light sleep and deep sleep, wakeup by PSM_EINT (at falling edge) and the MainUART.");
	}
	else if (type == 2)
	{
		sprintf(self->log_content,
				"Enable light sleep only, wakeup by the Main UART or PSM_EINT");
	}
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	return output_status;
}

StatusType configurePSM(struct ViettelSDK *self, uint8_t type,
		char requestedPeriodicTAU[], char requestedActiveTime[])
{
	/* AT+CPSMS */
	StatusType output_status = STATUS_UNKNOWN;
	if (type == 1)
	{
		sprintf(self->command, "%s=%u,,,\"%s\",\"%s\"", PSM_AT_CMD, type,
				requestedPeriodicTAU, requestedActiveTime);
		strcpy(self->requestedPeriodicTAU, requestedPeriodicTAU);
		strcpy(self->requestedActiveTime, requestedActiveTime);
	}
	else
	{
		sprintf(self->command, "%s=%u", PSM_AT_CMD, type);
	}
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Config PSM is %s.",
			getStatusTypeString(output_status));
	if (type == 0)
	{
		sprintf(self->log_content, "Disable PSM.");
	}
	else if (type == 1)
	{
		sprintf(self->log_content, "Enable PSM.");
	}
	else if (type == 2)
	{
		sprintf(self->log_content,
				"Disable PSM and discard all parameters for PSM.");
	}
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		writeLog(self, LOG_INFO, self->requestedPeriodicTAU, false);
		writeLog(self, LOG_INFO, self->requestedActiveTime, false);
	}
	return output_status;
}

StatusType TurnOffPSM(struct ViettelSDK *self, uint8_t type)
{
	/* AT+CPSMS */
	StatusType output_status = STATUS_UNKNOWN;
	if (type == 1)
	{
		sprintf(self->command, "%s=%u", PSM_AT_CMD, type);
	}
	else
	{
		sprintf(self->command, "%s=%u", PSM_AT_CMD, type);
	}
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Config PSM is %s.",
			getStatusTypeString(output_status));
	if (type == 0)
	{
		sprintf(self->log_content, "Disable PSM.");
	}
	return output_status;
}
