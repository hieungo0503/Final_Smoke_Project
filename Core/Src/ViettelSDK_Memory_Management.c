#include "ViettelSDK_Memory_Management.h"

void readFLASHPreviousFlowData(struct ViettelSDK *self, uint32_t address)
{
	self->previous_pulse = *(__IO uint32_t*) (address);
	sprintf(self->log_content, "Read flow data from FLASH memory: %lu",
			self->previous_pulse);
	writeLog(self, LOG_INFO, self->log_content, true);
}

void writeFLASHFlowData(struct ViettelSDK *self, uint32_t start_page,
		uint32_t end_page, uint32_t data32)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInit;
	EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInit.Page = start_page;												//FIXED
	EraseInit.NbPages = (end_page - start_page) / FLASH_PAGE_SIZE;
	uint32_t page_error = 0;
	HAL_FLASHEx_Erase(&EraseInit, &page_error);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, start_page, data32);		//FIXED
	sprintf(self->log_content, "Write flow data from FLASH memory: %lu\n",
			data32);
	writeLog(self, LOG_INFO, self->log_content, true);
	HAL_FLASH_Lock();
}
