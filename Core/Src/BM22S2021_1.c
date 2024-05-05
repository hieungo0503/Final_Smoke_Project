/*
 * BM22S2021_1.c
 *
 *  Created on: Apr 28, 2024
 *      Author: HieuNgo
 */
#include "BM22S2021_1.h"

void resetDMAInterrupt_BM22(struct Smoke_Data *self) {
	HAL_UARTEx_ReceiveToIdle_DMA(self->Somke_uart,
			(uint8_t*)self->response_receive_buffer,
			RECEIVE_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(self->Somke_uart_hdma, DMA_IT_HT);
}

void clearBuffer(struct Smoke_Data *self) {
	for (int i = 0; i < RECEIVE_BUFFER_SIZE; i++) {
		self->response_main_buffer[i] = 0;
	}
	self->old_position = 0;
	self->new_position = 0;
	self->recLen = self->recLen;
	self->checkCode = 0;
    self->isHeader= 0;
	}

void saveSmokeRespond(struct Smoke_Data *self, UART_HandleTypeDef *huart, uint16_t size)
{
	if (self->Somke_uart->Instance == huart->Instance) {
			//header of detector
			uint8_t header[5] = {0xAA, self->recLen , 0x11, 0x01, 0xAC};
			// Update the last position before copying new data
			self->old_position = self->new_position;

			/* If the data in large and it is about to exceed the buffer size, Debugger indicates error */
			if (self->old_position + size > RECEIVE_BUFFER_SIZE) {
				sprintf(self->log_content,
						"Response too large! Response Receive Buffer and Response Main Buffer will be cleared!");
				clearBuffer(self);
			}
			/* If the current position + new data size is less than the main buffer
			 * we will simply copy the data into the buffer and update the position
			 */

			if(self->response_receive_buffer[RECEIVE_BUFFER_SIZE-1] != 0)
			{
				//check header
				for(int i = 0; i<5; i++)
				{
					if(self->response_receive_buffer[i] == header[i])
						self->isHeader = 1;
					else{
						clearBuffer(self);
						self->failCnt ++;
						break;
					}
				}
				if(self->isHeader == 1)
				{
					for(int i=0; i< self->recLen; i++)
						self->checkCode += self->response_receive_buffer[i];
					self->checkCode = self->checkCode - self->response_receive_buffer[self-> recLen - 1];
					self->checkCode = ~(self->checkCode)+1;
					if(self->checkCode == self->response_receive_buffer[self-> recLen - 1])
					{
						memcpy((uint8_t*) self->response_main_buffer + self->old_position,
								self->response_receive_buffer, size);

						self->new_position = self->old_position + size;
						//
						self->smokeA_Data = (self->response_main_buffer[17] << 8) | self->response_main_buffer[16];
						self->smokeB_Data = (self->response_main_buffer[19] << 8) | self->response_main_buffer[18];

						if(self->smokeB_Data > self->threshold)
							self->AlarmSatus = 1;
						else
							self->AlarmSatus = 0;
					}
					else
					{
						clearBuffer(self);
						self->failCnt ++;
					}
				}
			}
			//==========================
			/* Start thce DMA again */
			resetDMAInterrupt_BM22(self);
	}

}

void initialSomke(struct Smoke_Data *self, UART_HandleTypeDef *huart, DMA_HandleTypeDef *h_DMA, uint8_t recLen,GPIO_TypeDef *StatusPort, uint16_t StatusPin, uint16_t threshold)
{
	self->Somke_uart = huart;
	self->Somke_uart_hdma = h_DMA;
	self->recLen = recLen;
	self->Status_Port = StatusPort;
	self->Status_Pin = StatusPin;
	self->threshold = threshold;
	printf("initial Smoke Success \n");
}


