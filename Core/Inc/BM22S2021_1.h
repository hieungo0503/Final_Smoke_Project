/*
 * BM22S2021_1.h
 *
 *  Created on: Apr 28, 2024
 *      Author: HieuNgo
 */

#ifndef INC_BM22S2021_1_H_
#define INC_BM22S2021_1_H_

#ifndef RECEIVE_BUFFER_SIZE
#define RECEIVE_BUFFER_SIZE (41U)
#endif

#include "main.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

struct Smoke_Data{
	uint16_t old_position;
	uint16_t new_position;
	char *log_content;
	uint16_t smokeA_Data, smokeB_Data;
	float temperature, humidity;
	char response_main_buffer[RECEIVE_BUFFER_SIZE];
	char response_receive_buffer[RECEIVE_BUFFER_SIZE];
	uint8_t recLen, checkCode, failCnt;
	bool isHeader;
	uint16_t threshold;
	bool AlarmSatus;

	GPIO_TypeDef *Status_Port;
	uint16_t Status_Pin;

	UART_HandleTypeDef *Somke_uart;
	DMA_HandleTypeDef *Somke_uart_hdma;
};

void saveSmokeRespond(struct Smoke_Data *self, UART_HandleTypeDef *huart, uint16_t size);

void initialSomke(struct Smoke_Data *self, UART_HandleTypeDef *huart, DMA_HandleTypeDef *h_DMA,
					uint8_t recLen,GPIO_TypeDef *StatusPort, uint16_t StatusPin, uint16_t threshold);

void resetDMAInterrupt_BM22(struct Smoke_Data *self);

#endif /* INC_BM22S2021_1_H_ */
