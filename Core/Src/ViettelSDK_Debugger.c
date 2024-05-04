#include "ViettelSDK_Debugger.h"
#include "main.h"
void writeLog(struct ViettelSDK *self, enum LogType log_type,
		char log_content[], bool default_log)
{

	if ((!default_log) && (!self->detail_log))
	{
		return;
	}

	if (strlen(log_content) >= LOG_CONTENT_SIZE)
	{
		strcpy(self->log_content, (const char*) "Log content is too long!");
		if(HW_DEBUG){
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG ERROR: ",
				(uint16_t) strlen("LOG ERROR: "), LOG_TIMEOUT_MS);
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) self->log_content,
				(uint16_t) strlen(self->log_content), LOG_TIMEOUT_MS);
		HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "\n",
				(uint16_t) strlen("\n"), LOG_TIMEOUT_MS);
		}
		if(SWO_DEBUG)
		{
//			initialise_monitor_handles();
			printf("LOG ERROR: %s",(uint8_t*) self->log_content);
//			initialise_monitor_handles();
			printf("\n");
		}
		for (int i = 0; i < strlen(self->log_content); i++)
		{
			self->log_content[i] = 0;
		}
		return;
	}

	strcpy(self->log_content, log_content);
	if (log_type == LOG_INFO)
	{
		if(HW_DEBUG)
		{
					HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG INFO: ",
							(uint16_t) strlen("LOG INFO: "), LOG_TIMEOUT_MS);
		}

		if(SWO_DEBUG)
		{
//			initialise_monitor_handles();
			printf("LOG INFO: %s",(uint8_t*) self->log_content);
//			initialise_monitor_handles();
			printf("\n");
		}
	}
	else if (log_type == LOG_WARNING)
	{
		if(HW_DEBUG)
		{
			HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG WARNING: ",
					(uint16_t) strlen("LOG WARNING: "), LOG_TIMEOUT_MS);
		}

		if(SWO_DEBUG)
		{
//			initialise_monitor_handles();
			printf("LOG INFO: %s",(uint8_t*) self->log_content);
//			initialise_monitor_handles();
			printf("\n");
		}
	}
	else if (log_type == LOG_ERROR)
	{
		if(HW_DEBUG)
		{
			HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG ERROR: ",
					(uint16_t) strlen("LOG ERROR: "), LOG_TIMEOUT_MS);
		}

		if(SWO_DEBUG)
		{
//			initialise_monitor_handles();
			printf("LOG ERROR: %s",(uint8_t*) self->log_content);
//			initialise_monitor_handles();
			printf("\n");
		}
	}
	else if (log_type == LOG_DEBUG)
	{
		if(HW_DEBUG)
		{
			HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG DEBUG: ",
					(uint16_t) strlen("LOG DEBUG: "), LOG_TIMEOUT_MS);
		}

		if(SWO_DEBUG)
		{
//			initialise_monitor_handles();
			printf("LOG DEBUG: %s",(uint8_t*) self->log_content);
//			initialise_monitor_handles();
			printf("\n");
		}
	}
	else if (log_type == LOG_EMPTY)
	{
		if(HW_DEBUG)
			{
			HAL_UART_Transmit(self->debugger_uart, (uint8_t*) self->log_content,
					(uint16_t) strlen(self->log_content), LOG_TIMEOUT_MS);
			HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "\n",
					(uint16_t) strlen("\n"), LOG_TIMEOUT_MS);
			}

		if(SWO_DEBUG){
	//		initialise_monitor_handles();
			printf("%s",(uint8_t*) self->log_content);
	//		initialise_monitor_handles();
			printf("\n");
		}
	}
	else
	{
		if(HW_DEBUG)
		{
			HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "LOG ERROR: ",
					(uint16_t) strlen("LOG ERROR: "), LOG_TIMEOUT_MS);
			HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "Invalid LOG TYPE!",
					(uint16_t) strlen("Invalid LOG TYPE!"), LOG_TIMEOUT_MS);
			HAL_UART_Transmit(self->debugger_uart, (uint8_t*) "\n",
					(uint16_t) strlen("\n"), LOG_TIMEOUT_MS);
		}

		if(SWO_DEBUG)
		{
//			initialise_monitor_handles();
			printf("LOG ERROR: Invalid LOG TYPE!");
//			initialise_monitor_handles();
			printf("\n");
		}
		return;
	}

}

void logStartBanner(struct ViettelSDK *self)
{
	sprintf(self->log_content,
			"\n\n\n==================== STARTING ====================");
	writeLog(self, LOG_EMPTY, self->log_content, true);
}
