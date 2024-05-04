#include "ViettelSDK_Peripherals_Driver.h"

void resetMCU(struct ViettelSDK *self)
{
	getSensorData(self);
	sprintf(self->log_content, "MCU will be reset now.\n\n\n");
	writeLog(self, LOG_INFO, self->log_content, true);
	NVIC_SystemReset();
}

void pullPin(struct ViettelSDK *self, struct Pin pin, bool up)
{
	if (up)
	{
		self->GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(pin.pin_GPIOx, &self->GPIO_InitStruct);
		HAL_GPIO_WritePin(pin.pin_GPIOx, pin.pin_GPIO_Pin, GPIO_PIN_SET);
	}
	else
	{
		self->GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(pin.pin_GPIOx, &self->GPIO_InitStruct);
		HAL_GPIO_WritePin(pin.pin_GPIOx, pin.pin_GPIO_Pin, GPIO_PIN_RESET);
	}
}

void noPullPin(struct ViettelSDK *self, struct Pin pin)
{
	self->GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(pin.pin_GPIOx, &self->GPIO_InitStruct);
}

void resetDMAInterrupt(struct ViettelSDK *self)
{
	HAL_UARTEx_ReceiveToIdle_DMA(self->module_uart,
			self->response_receive_buffer, RESPONSE_RECEIVE_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(self->module_uart_hdma, DMA_IT_HT);
}

bool validateUARTPorts(struct ViettelSDK *self)
{
	bool validated = true;
	if (self->debugger_uart == self->module_uart)
	{
		self->module_uart = 0x00;
		validated = false;
		writeLog(self, LOG_WARNING,
				"Debugger has the same UART port as the Connector. UART port will be used for Debugger.",
				true);
	}
	else
	{
		writeLog(self, LOG_INFO, "Module UART port is initialized.", true);
	}
	return validated;
}
