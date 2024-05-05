#include "ViettelSDK_Data_Management.h"

void addData(struct ViettelSDK *self, char key[], void *value,
		enum ValueType value_type)
{

	/* Fail add previous data, then not add more data */
	if (!self->addDataSuccessfully)
	{
		return;
	}

	DataNode *new_node = malloc(sizeof(DataNode));

	if (!new_node || (strlen(key) > KEY_MAX_LENGTH))
	{
		self->addDataSuccessfully = false;
		return;
	}

	strcpy(new_node->pair.key, key);

	switch (value_type)
	{
	case VALUE_BOOL:
		new_node->pair.value.bool_value = (bool*) value;
		new_node->pair.value_type = VALUE_BOOL;
		break;
	case VALUE_CHAR:
		new_node->pair.value.char_value = (char*) value;
		new_node->pair.value_type = VALUE_CHAR;
		break;
	case VALUE_UNSIGNED_CHAR:
		new_node->pair.value.unsigned_char_value = (unsigned char*) value;
		new_node->pair.value_type = VALUE_UNSIGNED_CHAR;
		break;
	case VALUE_UINT8_T:
		new_node->pair.value.uint8_value = (uint8_t*) value;
		new_node->pair.value_type = VALUE_UINT8_T;
		break;
	case VALUE_SHORT:
		new_node->pair.value.short_value = (short*) value;
		new_node->pair.value_type = VALUE_SHORT;
		break;
	case VALUE_UNSIGNED_SHORT:
		new_node->pair.value.unsigned_short_value = (unsigned short*) value;
		new_node->pair.value_type = VALUE_UNSIGNED_SHORT;
		break;
	case VALUE_UINT16_T:
		new_node->pair.value.uint16_value = (uint16_t*) value;
		new_node->pair.value_type = VALUE_UINT16_T;
		break;
	case VALUE_INT:
		new_node->pair.value.int_value = (int*) value;
		new_node->pair.value_type = VALUE_INT;
		break;
	case VALUE_LONG:
		new_node->pair.value.long_value = (long*) value;
		new_node->pair.value_type = VALUE_LONG;
		break;
	case VALUE_UINT32_T:
		new_node->pair.value.uint32_value = (uint32_t*) value;
		new_node->pair.value_type = VALUE_UINT32_T;
		break;
	case VALUE_FLOAT:
		new_node->pair.value.float_value = (float*) value;
		new_node->pair.value_type = VALUE_FLOAT;
		break;
	case VALUE_CHAR_ARRAY:
		new_node->pair.value.char_array_value = (char*) value;
		new_node->pair.value_type = VALUE_CHAR_ARRAY;
		break;
	default:
		return;
	}

	/* Set the next node to NULL */
	new_node->next = NULL;

	/* If the list is empty, set the new node as the head */
	if (!self->data_list)
	{
		self->data_list = new_node;
	}
	/* Otherwise, traverse the list to the end and append the new node */
	else
	{
		DataNode *current_node = self->data_list;
		while (current_node->next)
		{
			current_node = current_node->next;
		}
		current_node->next = new_node;
	}

}

void changeCurrentData(struct ViettelSDK *self, char key[], void *value, enum ValueType value_type)
{
	DataNode *current_node = self->data_list;
	while (current_node)
	{
		if(strstr(current_node->pair.key, key))
		{
			switch (value_type)
				{
				case VALUE_BOOL:
					current_node->pair.value.bool_value = (bool*) value;
					current_node->pair.value_type = VALUE_BOOL;
					break;
				case VALUE_CHAR:
					current_node->pair.value.char_value = (char*) value;
					current_node->pair.value_type = VALUE_CHAR;
					break;
				case VALUE_UNSIGNED_CHAR:
					current_node->pair.value.unsigned_char_value = (unsigned char*) value;
					current_node->pair.value_type = VALUE_UNSIGNED_CHAR;
					break;
				case VALUE_UINT8_T:
					current_node->pair.value.uint8_value = (uint8_t*) value;
					current_node->pair.value_type = VALUE_UINT8_T;
					break;
				case VALUE_SHORT:
					current_node->pair.value.short_value = (short*) value;
					current_node->pair.value_type = VALUE_SHORT;
					break;
				case VALUE_UNSIGNED_SHORT:
					current_node->pair.value.unsigned_short_value = (unsigned short*) value;
					current_node->pair.value_type = VALUE_UNSIGNED_SHORT;
					break;
				case VALUE_UINT16_T:
					current_node->pair.value.uint16_value = (uint16_t*) value;
					current_node->pair.value_type = VALUE_UINT16_T;
					break;
				case VALUE_INT:
					current_node->pair.value.int_value = (int*) value;
					current_node->pair.value_type = VALUE_INT;
					break;
				case VALUE_LONG:
					current_node->pair.value.long_value = (long*) value;
					current_node->pair.value_type = VALUE_LONG;
					break;
				case VALUE_UINT32_T:
					current_node->pair.value.uint32_value = (uint32_t*) value;
					current_node->pair.value_type = VALUE_UINT32_T;
					break;
				case VALUE_FLOAT:
					current_node->pair.value.float_value = (float*) value;
					current_node->pair.value_type = VALUE_FLOAT;
					break;
				case VALUE_CHAR_ARRAY:
					current_node->pair.value.char_array_value = (char*) value;
					current_node->pair.value_type = VALUE_CHAR_ARRAY;
					break;
				default:
					return;
				}
			break;
		}
		current_node = current_node->next;
	}

}

void addCENGData(struct ViettelSDK *self)
{

	addData(self, "sc_earfcn", &self->cell_data.serving_cell.sc_earfcn,
			VALUE_UINT32_T);
	addData(self, "sc_earfcn_offset",
			&self->cell_data.serving_cell.sc_earfcn_offset, VALUE_UINT8_T);
	addData(self, "sc_pci", &self->cell_data.serving_cell.sc_pci,
			VALUE_UINT16_T);
	addData(self, "sc_cellid", &self->cell_data.serving_cell.sc_cellid,
			VALUE_CHAR_ARRAY);
	addData(self, "sc_rsrp", &self->cell_data.serving_cell.sc_rsrp,
			VALUE_SHORT);
	addData(self, "sc_rsrq", &self->cell_data.serving_cell.sc_rsrq,
			VALUE_SHORT);
	addData(self, "sc_rssi", &self->cell_data.serving_cell.sc_rssi,
			VALUE_SHORT);
	addData(self, "sc_snr", &self->cell_data.serving_cell.sc_snr, VALUE_SHORT);
	addData(self, "sc_band", &self->cell_data.serving_cell.sc_band,
			VALUE_UINT8_T);
	addData(self, "sc_tac", &self->cell_data.serving_cell.sc_tac,
			VALUE_CHAR_ARRAY);
	addData(self, "sc_ecl", &self->cell_data.serving_cell.sc_ecl,
			VALUE_UINT8_T);
	addData(self, "sc_tx_pwr", &self->cell_data.serving_cell.sc_tx_pwr,
			VALUE_SHORT);
	addData(self, "sc_re_rsrp", &self->cell_data.serving_cell.sc_re_rsrp,
			VALUE_SHORT);
}

void addDeviceID(struct ViettelSDK *self, char ID[])
{
	addData(self,"testId","MAIN_MCU_MODULE_SIGNAL",VALUE_CHAR_ARRAY);
	addData(self,"details","",VALUE_CHAR);
	addData(self,"deviceId",ID,VALUE_CHAR_ARRAY);
}

void addBatteryVoltage(struct ViettelSDK *self)
{
	addData(self,"testId","BATTERY_VOLTAGE",VALUE_CHAR_ARRAY);
	addData(self,"details","",VALUE_CHAR);
	addData(self,"voltage",&self->voltage,VALUE_UINT16_T);
}
void changeBatteryVoltage(struct ViettelSDK *self)
{
	changeCurrentData(self,"voltage",&self->voltage,VALUE_UINT16_T);
}

void addSimProfile(struct ViettelSDK *self)
{
	addData(self,"testId","CELLULAR_SIM",VALUE_CHAR_ARRAY);
	addData(self,"details","",VALUE_CHAR);
	addData(self,"imsi",&self->imsi,VALUE_CHAR_ARRAY);
}

uint32_t PSM_timerDecodeT3412(uint8_t  binaryValue)
{
	// Extracting bits 5 to 1
	unsigned char timerValue = binaryValue & 0b00011111;

	// Extracting bits 8 to 6
	unsigned char unitValue = (binaryValue >> 5) & 0b00000111;

	// Calculate time based on the defined rules
	uint32_t timeValue;
	switch (unitValue) {
		case 0b000:  // value is incremented in multiples of 10 minutes
			timeValue = timerValue * 10 * 60;
			break;
		case 0b001:  // value is incremented in multiples of 1 hour
			timeValue = timerValue * 3600;
			break;
		case 0b010:  // value is incremented in multiples of 10 hours
			timeValue = timerValue * 10* 3600;
			break;
		case 0b011:  // value is incremented in multiples of 2 seconds
			timeValue = timerValue * 2;
			break;
		case 0b100:  // value is incremented in multiples of 30 seconds
			timeValue = timerValue * 30;
			break;
		case 0b101:  // value is incremented in multiples of 1 minute
			timeValue = timerValue * 60;
			break;
		case 0b110:  // value is incremented in multiples of 320 hours
			timeValue = timerValue * 320 * 3600;
			break;
		default:
			printf("Invalid unit value\n");
			return 1;
	}
	return timeValue;
}
uint32_t PSM_timerDecodeT3324(uint8_t  binaryValue)
{
	// Extracting bits 5 to 1
	unsigned char timerValue = binaryValue & 0b00011111;

	// Extracting bits 8 to 6
	unsigned char unitValue = (binaryValue >> 5) & 0b00000111;

	// Calculate time based on the defined rules
	uint32_t timeValue;
	switch (unitValue) {
		case 0b000:  // value is incremented in multiples of 2 sec
			timeValue = timerValue * 2;
			break;
		case 0b001:  // value is incremented in multiples of 1 min
			timeValue = timerValue * 60;
			break;
		case 0b010:  // value is incremented in multiples of 6 min
			timeValue = timerValue * 6 *60;
			break;
		default:
			printf("Invalid unit value\n");
			return 1;
	}
	return timeValue;
}

void addNetworkData(struct ViettelSDK *self)
{
	addData(self,"testId","CELLULAR_SIGNAL",VALUE_CHAR_ARRAY);
	addData(self,"details","",VALUE_CHAR);
	addData(self,"Operator","VIETTEL",VALUE_CHAR_ARRAY);
	addData(self,"band",&self->cell_data.serving_cell.sc_band,VALUE_UINT8_T);
	addData(self,"EARFCN",&self->cell_data.serving_cell.sc_earfcn,VALUE_UINT32_T);
	addData(self, "PCI", &self->cell_data.serving_cell.sc_pci, VALUE_UINT16_T);
	addData(self,"connectionStatus",&self->stat,VALUE_UINT8_T);
	addData(self,"ipAddress",&self->ipAddress,VALUE_CHAR_ARRAY);
	addData(self,"RSRP",&self->cell_data.serving_cell.sc_rsrp,VALUE_SHORT);
	addData(self,"RSSI",&self->cell_data.serving_cell.sc_rssi,VALUE_SHORT);
	addData(self,"RSRQ",&self->cell_data.serving_cell.sc_rsrq,VALUE_SHORT);
	addData(self,"T3324",&self->cell_data.serving_cell.T3324,VALUE_UINT32_T);	//check
	addData(self,"T3412",&self->cell_data.serving_cell.T3412,VALUE_UINT32_T);//check
	addData(self,"tac",&self->cell_data.serving_cell.sc_tac,VALUE_CHAR_ARRAY);
}
void changeNetworkData(struct ViettelSDK *self)
{
	changeCurrentData(self,"band",&self->cell_data.serving_cell.sc_band,VALUE_UINT8_T);
	changeCurrentData(self,"EARFCN",&self->cell_data.serving_cell.sc_earfcn,VALUE_UINT32_T);
	changeCurrentData(self, "PCI", &self->cell_data.serving_cell.sc_pci, VALUE_UINT16_T);
	changeCurrentData(self,"connectionStatus",&self->stat,VALUE_UINT8_T);
	changeCurrentData(self,"ipAddress",&self->ipAddress,VALUE_CHAR_ARRAY);
	changeCurrentData(self,"RSRP",&self->cell_data.serving_cell.sc_rsrp,VALUE_SHORT);
	changeCurrentData(self,"RSSI",&self->cell_data.serving_cell.sc_rssi,VALUE_SHORT);
	changeCurrentData(self,"RSRQ",&self->cell_data.serving_cell.sc_rsrq,VALUE_SHORT);
	changeCurrentData(self,"T3324",&self->cell_data.serving_cell.T3324,VALUE_UINT32_T);	//check
	changeCurrentData(self,"T3412",&self->cell_data.serving_cell.T3412,VALUE_UINT32_T);//check
	changeCurrentData(self,"tac",&self->cell_data.serving_cell.sc_tac,VALUE_CHAR_ARRAY);
}
void packData(struct ViettelSDK *self)
{
	if (!self->data_list)
	{
		/* Not add data yet, use sample data */
		char sample_data[] = "sample_data";
		addData(self, "sample_data", &sample_data, VALUE_CHAR_ARRAY);
	}

	for (int i = 0; i < strlen(self->coap_params.message_buffer); i++)
	{
		self->coap_params.message_buffer[i] = 0;
	}

	DataNode *current_node = self->data_list;

	strcat(self->coap_params.message_buffer, "[");

	bool first_testID = true;

	while (current_node)
	{
	    if(strstr(current_node->pair.key,"testId"))
	    {
	        if(first_testID == false)
	        {
	            strcat(self->coap_params.message_buffer, "}},");
	        }
	        strcat(self->coap_params.message_buffer, "{");
	        strcat(self->coap_params.message_buffer,"\"");
	       	strcat(self->coap_params.message_buffer, current_node->pair.key);
		    strcat(self->coap_params.message_buffer, "\"");
		    strcat(self->coap_params.message_buffer, ":");
    		/* add value */
    		strcat(self->coap_params.message_buffer,
    			castToString(current_node->pair.value,
    				current_node->pair.value_type));
    		first_testID = false;
	    }
	    else if(strstr(current_node->pair.key,"details"))
	    {
	        strcat(self->coap_params.message_buffer,"\"");
	       	strcat(self->coap_params.message_buffer, current_node->pair.key);
		    strcat(self->coap_params.message_buffer, "\"");
		    strcat(self->coap_params.message_buffer, ":");
    		strcat(self->coap_params.message_buffer, "{");
	    }
	    else
	    {
	        strcat(self->coap_params.message_buffer,"\"");
	       	strcat(self->coap_params.message_buffer, current_node->pair.key);
		    strcat(self->coap_params.message_buffer, "\"");
		    strcat(self->coap_params.message_buffer, ":");
    		/* add value */
    		strcat(self->coap_params.message_buffer,
    			castToString(current_node->pair.value,
    				current_node->pair.value_type));
	    }


// 		/* add key */
// 		strcat(message_buffer, "\"");
// 		strcat(message_buffer, current_node->pair.key);
// 		strcat(message_buffer, "\"");

// 		strcat(message_buffer, ":");

// 		/* add value */
// 		strcat(message_buffer,
// 			castToString(current_node->pair.value,
// 				current_node->pair.value_type));

		if (current_node->next)
		{
            if(strstr(current_node->next->pair.key,"testId"))
            {}
            else if(!strstr(current_node->pair.key,"details"))
			strcat(self->coap_params.message_buffer, ",");
		}
		current_node = current_node->next;
	}

	strcat(self->coap_params.message_buffer, "}}]");

//	if (strlen(self->mqtt_params.message_buffer) < 1000) {
//		for (int i = strlen(self->mqtt_params.message_buffer); i < strlen(self->mqtt_params.message_buffer) + 10; i++) {
//			self->mqtt_params.message_buffer[i] = 48;
//		}
//	}

	writeLog(self, LOG_INFO, self->coap_params.message_buffer, true);

	/* Change to hexa string */
	for (int i = 0; i < strlen(self->coap_params.message_buffer); i++)
	{
		sprintf(self->coap_params.message + 2 * i, "%02X",
				self->coap_params.message_buffer[i]); // Convert each character to its hex value
	}
}

void addSensorData(struct ViettelSDK *self, struct Smoke_Data *smoke_hler)
{
	  //Save Temp Data
		addData(self,"testId","SENSOR",VALUE_CHAR_ARRAY);
		addData(self,"details","",VALUE_CHAR);
		addData(self, "AlarmSatus", &smoke_hler->AlarmSatus, VALUE_BOOL);
		addData(self, "SmokeValue",&smoke_hler->smokeB_Data, VALUE_UINT16_T);
		addData(self, "Temperature", &smoke_hler->temperature, VALUE_FLOAT);
		addData(self, "Humidity", &smoke_hler->humidity, VALUE_FLOAT);
//		updateFLASHData(&sdk_handler);
}

void changeSensorData(struct ViettelSDK *self, struct Smoke_Data *smoke_hler)
{
	  //Save Temp Data
		changeCurrentData(self, "AlarmSatus", &smoke_hler->AlarmSatus, VALUE_BOOL);
		changeCurrentData(self, "SmokeValue",&smoke_hler->smokeB_Data, VALUE_UINT16_T);
		changeCurrentData(self, "Temperature", &smoke_hler->temperature, VALUE_FLOAT);
		changeCurrentData(self, "Humidity", &smoke_hler->humidity, VALUE_FLOAT);
//		updateFLASHData(&sdk_handler);
}


void getSensorData(struct ViettelSDK *self)
{
	sprintf(self->log_content, "Writing data to FLASH memory.\n");
	writeLog(self, LOG_INFO, self->log_content, true);
	writeFLASHFlowData(self, FLASH_USER_START_ADDR, FLASH_USER_END_ADDR,
			*self->data_list->pair.value.uint32_value);
}

void updateFLASHData(struct ViettelSDK *self)
{
	*self->data_list->pair.value.uint32_value = self->previous_pulse;
	sprintf(self->log_content, "Update current flow data: %lu",
			*self->data_list->pair.value.uint32_value);
	writeLog(self, LOG_INFO, self->log_content, true);
}



