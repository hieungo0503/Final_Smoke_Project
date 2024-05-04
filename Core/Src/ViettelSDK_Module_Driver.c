#include "ViettelSDK_Module_Driver.h"

void wakeUpModule(struct ViettelSDK *self)
{
	uint8_t count = WAKE_UP_MODULE_PUSH_COUNT;
	while (count--)
	{
		pullPin(self, self->power_pin, false);
		HAL_Delay(MODULE_PWR_PIN_PULL_DOWN_DELAY_MS);
		pullPin(self, self->power_pin, true);
		HAL_Delay(MODULE_PWR_PIN_PULL_UP_DELAY_MS);
	}
	noPullPin(self, self->power_pin);

	sprintf(self->log_content, "Woke up module...");
	writeLog(self, LOG_INFO, self->log_content, true);
}

void powerOffModule(struct ViettelSDK *self, bool urgent)
{
	/* AT+CPOWD */
	/* Send command */
	sprintf(self->command, "%s=%d", POWER_OFF, !urgent);
	self->passively_listen = true;
	self->command_response = sendCommand(self, self->command, 1,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
}

StatusType checkModule(struct ViettelSDK *self)
{
	/* AT */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
	self->command_response = sendCommand(self, "AT",
	RUN_COMMAND_COUNTER_DEFAULT + 6,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 1000); /* Configured for PSM mode, DO NOT CHANGE */

	/* Get output status */
	output_status = self->command_response.status;

	/* Log status of command */
	sprintf(self->log_content, "Checking Module Status is %s.",
			getStatusTypeString(output_status));

	/* Logical processing */
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
		/* What to do if status is NOT SUCCESS */
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		/* What to do if status is SUCCESS */

		/* Warm up module */
		sprintf(self->log_content, "Warming up module %u times.",
				self->warming_up_counter);
		writeLog(self, LOG_INFO, self->log_content, true);
		while (self->warming_up_counter--)
		{
			self->command_response = sendCommand(self, "AT", 1,
			RUN_COMMAND_TIMEOUT_MS_DEFAULT);
		}

		self->warming_up_counter = WARMING_UP_COUNT;
	}

	return output_status;
}

StatusType setCommandEchoMode(struct ViettelSDK *self, bool echo_mode)
{
	/* ATEx */
	StatusType output_status = STATUS_UNKNOWN;
	uint8_t echo_mode_code;
	if (echo_mode)
	{
		echo_mode_code = 1;
	}
	else
	{
		echo_mode_code = 0;
	}

	sprintf(self->command, "%s%u", COMMAND_ECHO_MODE, echo_mode_code);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	self->echo_mode = echo_mode;
	if (self->echo_mode)
	{
		sprintf(self->log_content, "Echo mode is ON");
	}
	else
	{
		sprintf(self->log_content, "Echo mode is OFF");
	}

	sprintf(self->log_content, "Set Command Echo Mode Status is %s.",
			getStatusTypeString(output_status));

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

StatusType readCCID(struct ViettelSDK *self)
{
	/* AT+CCID */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", SHOW_ICCID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Reading CCID Status is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
	}
	else
	{
		self->token = strstr(self->command_response.response, "\n");
		self->token = self->token + 1;
		self->token = strtok(self->token, self->delimiter);
		strcpy(self->ccid, self->token);
		sprintf(self->log_content, "CCID: %s.", self->ccid);
		writeLog(self, LOG_INFO, self->log_content, false);
	}

	return output_status;
}

StatusType requestIMEI(struct ViettelSDK *self)
{
	/* AT+CGSN */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=1", REQUEST_IMEI);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	sprintf(self->log_content, "Requesting IMEI is %s.",
			getStatusTypeString(output_status));
	output_status = self->command_response.status;
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		self->token = strstr(self->command_response.response, "\n");
		self->token = self->token + 1;
		self->token = strtok(self->token, self->delimiter);
		strcpy(self->imei, self->token);
		sprintf(self->log_content, "IMEI: %s.", self->imei);
		writeLog(self, LOG_INFO, self->log_content, false);

	}
	return output_status;
}

StatusType requestTARevisionIdentification(struct ViettelSDK *self)
{
	/* AT+CGMR */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", REQUEST_TA_REVISION_ID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 3,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Requesting TA Revision ID is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		self->token = strstr(self->command_response.response, "\n");
		self->token = self->token + 1;
		self->token = strtok(self->token, self->delimiter);
		strcpy(self->ta_revision_id, self->token);
		sprintf(self->log_content, "Request TA Revision ID: %s.",
				self->ta_revision_id);
		writeLog(self, LOG_INFO, self->log_content, false);

	}
	return output_status;
}

StatusType readSignalQualityReport(struct ViettelSDK *self)
{
	/* AT+CSQ */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", SIGNAL_QUALITY_REPORT);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Reading Signal Quality Report is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		int number_of_elements = 2;
		char *data_pointer;
		char *token_array[number_of_elements];
		uint8_t i = 0;

		data_pointer = strstr(self->command_response.response, ":");
		data_pointer = data_pointer + 2;

		uint8_t rssi_code;
		uint8_t ber;

		self->token = strtok(data_pointer, self->delimiter);
		while (self->token != NULL && i < number_of_elements)
		{
			token_array[i] = self->token;
			self->token = strtok(NULL, self->delimiter);
			i++;
		}

		rssi_code = atoi(token_array[0]);
		ber = atoi(token_array[1]);

		sprintf(self->log_content, "RSSI code: %u", rssi_code);
		writeLog(self, LOG_INFO, self->log_content, false);

		if (ber >= 0 && ber <= 7)
		{
			sprintf(self->log_content, "Bit error rate (ber): %u%%", ber);
		}
		else if (ber == 99)
		{
			sprintf(self->log_content,
					"Bit error rate (ber) is Not known or not detectable.");
		}
		else
		{
			sprintf(self->log_content, "Invalid Bit error rate (ber): %u",
					rssi_code);
		}
		writeLog(self, LOG_INFO, self->log_content, false);

	}
	return output_status;
}

StatusType setEPSNetworkRegistrationStatus(struct ViettelSDK *self,
		uint8_t code)
{
	/* AT+CEREG= */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%u", EPS_NETWORK_REGISTRATION_STATUS, code);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Setting EPS Network Registration Status is %s.",
			getStatusTypeString(output_status));
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

StatusType readEPSNetworkRegistrationStatus(struct ViettelSDK *self,
bool save_data)
{
	/* AT+CEREG? */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", EPS_NETWORK_REGISTRATION_STATUS);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 1,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Reading EPS Network Registration Status is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		int number_of_elements = 2;
		char *data_pointer =
		{ 0 };
		char *token_array[number_of_elements];
		char *token_array_PSM[9];
		uint8_t i = 0;

		for (uint8_t i = 0; i < RESPONSE_MAIN_BUFFER_SIZE; i++)
		{
			if (self->command_response.response[i] == ':')
			{
				data_pointer = &self->command_response.response[i] + 2;
				break;
			}
		}

//		data_pointer = strchr(self->command_response.response, ':');
		if (!data_pointer)
		{
			output_status = STATUS_INTERNAL_FAILURE;
			return output_status;
		}
//		data_pointer = data_pointer + 2;

		uint8_t n;
		uint8_t stat;
		n = atoi(data_pointer);

		switch (n)
		{
		case 0:
			self->token = strtok(data_pointer, self->delimiter);
			while (self->token != NULL && i < number_of_elements)
			{
				token_array[i] = self->token;
				self->token = strtok(NULL, self->delimiter);
				i++;
			}

			stat = atoi(token_array[1]);

			if (save_data)
			{
				self->n = atoi(token_array[0]);
				self->stat = atoi(token_array[1]);
			}

			break;

		case 2:
			self->token = strtok(data_pointer, self->delimiter);
			while (self->token != NULL && i < number_of_elements)
			{
				token_array[i] = self->token;
				self->token = strtok(NULL, self->delimiter);
				i++;
			}

			stat = atoi(token_array[1]);

			if (save_data)
			{
				self->n = atoi(token_array[0]);
				self->stat = atoi(token_array[1]);
			}

			break;

		case 4:
			self->token = strtok(data_pointer, self->delimiter);
			while (self->token != NULL && i < 9)
			{
				token_array_PSM[i] = self->token;
				self->token = strtok(NULL, self->delimiter);
				i++;
			}

			stat = atoi(token_array_PSM[1]);

			if (save_data)
			{
				self->n = atoi(token_array_PSM[0]);
				self->stat = atoi(token_array_PSM[1]);
				char T3324[9];
				char T3412[9];
				strcpy(T3324,token_array_PSM[5]);
				strcpy(T3412,token_array_PSM[6]);
				removeSubstring(T3412, "\"");
				removeSubstring(T3324, "\"");
				printf("%s, %s \n", T3324, T3412);
				uint8_t va = strtol(T3324, NULL,2);
				printf("%u", va);
				self->cell_data.serving_cell.T3324 = PSM_timerDecodeT3324((uint8_t)strtol(T3324,NULL,2));
				self->cell_data.serving_cell.T3412 = PSM_timerDecodeT3412((uint8_t)strtol(T3412,NULL,2));
				sprintf(self->log_content, "<T3324> = %lu, <T3412> = %lu", self->cell_data.serving_cell.T3324,
						self->cell_data.serving_cell.T3412);
				writeLog(self, LOG_INFO, self->log_content, false);
			}

			break;

		default:
			break;
		}

		sprintf(self->log_content, "<n> = %u", n);
		writeLog(self, LOG_INFO, self->log_content, false);

		switch (stat)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		}

		sprintf(self->log_content, "<stat> = %u", stat);
		writeLog(self, LOG_INFO, self->log_content, false);
//		if(n == 4)
//		{
//			char res[60];
//			strcpy(res,self->command_response.response);
//			removeSubstring(res, "+CEREG: ");
//			removeSubstring(res, "OK");
//			removeChars(res, "\"\r\n ");
//
//			char *ptr;
//			char *data_ptr;
//			uint8_t sign_count = 0;
//			uint8_t number_of_elements = 0;
//
//			/* Count number of comma since data may not be report */
//			/* Example: 3,"A794",0,,-56 */
//			ptr = strchr(res, ',');
//			while (ptr)
//			{
//				ptr += 1;
//				sign_count += 1;
//				ptr = strchr(ptr, ',');
//			}
//
//			/* Initialize elemtent array */
//			number_of_elements = sign_count + 1;
//			char *string_array[number_of_elements];
//			char clone_data[60];
//			uint8_t element_count = 0;
//
//			data_ptr = res;
//			strcpy(clone_data, data_ptr);
//			ptr = strchr(clone_data, ',');
//			sign_count = 0;
//			self->token = strtok(data_ptr, ",");
//			while (self->token && element_count < number_of_elements)
//			{
//				if (*(ptr + 1) == ',')
//				{
//					string_array[element_count] = static_null_string;
//					element_count++;
//				}
//				else
//				{
//					/* Element footer */
//					string_array[element_count] = self->token;
//					self->token = strtok(NULL, ",");
//					element_count++;
//				}
//
//				/* Delimiter footer */
//				if (sign_count != 0)
//				{
//					ptr += 1;
//					ptr = strchr(ptr, ',');
//				}
//				sign_count += 1;
//			}
//
//			//T3324 and T3412 is accept from network over here
//			char T3324[8],T3412[8];
//			strcpy(T3324, string_array[7]);
//			strcpy(T3412, string_array[8]);
//
//			self->cell_data.serving_cell.T3324 = PSM_timerDecode(strtol(T3324,NULL,2));
//			self->cell_data.serving_cell.T3412 = PSM_timerDecode(strtol(T3412,NULL,2));
//		}

	}
	return output_status;
}

StatusType setOperatorSelection(struct ViettelSDK *self, uint8_t mode)
{
	/* AT+COPS= */
	StatusType output_status = STATUS_UNKNOWN;
	if ((mode == 0) || (mode == 1) || (mode == 2) || (mode == 3))
	{
		switch (mode)
		{
		case 0:
			sprintf(self->command, "%s=%u", OPERATOR_SELECTION, mode);
			break;
		case 1:
			sprintf(self->command, "%s=%u,2,\"45204\"", OPERATOR_SELECTION,
					mode);
			break;
		case 2:
			sprintf(self->command, "%s=%u", OPERATOR_SELECTION, mode);
			break;
		case 3:
			sprintf(self->command, "%s=%u,2", OPERATOR_SELECTION, mode);
			break;
		}
		sprintf(self->log_content, "<mode> = %u", mode);
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	else
	{
		sprintf(self->log_content, "<mode> = %u is invalid.", mode);
		writeLog(self, LOG_ERROR, self->log_content, false);
		output_status = STATUS_BAD_PARAMETER;
		return output_status;
	}

	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 4,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 3000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Set Operator Selection is %s.",
			getStatusTypeString(output_status));
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

StatusType setControlTheDataOutputFormat(struct ViettelSDK *self) {
	/* AT+CREVHEX */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=1", CONTROL_THE_DATA_OUTPUT_FORMAT);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Set Control the Data Output Format is %s.",
			getStatusTypeString(output_status));
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

StatusType setReportMobileEquipmentError(struct ViettelSDK *self, uint8_t mode)
{
	/* AT+CMEE= */
	StatusType output_status = STATUS_UNKNOWN;
	if ((mode == 0) || (mode == 1) || (mode == 2))
	{
		sprintf(self->command, "%s=%u", REPORT_MOBILE_EQUIPMENT_ERROR, mode);
		self->command_response = sendCommand(self, self->command,
		RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
		output_status = self->command_response.status;
		sprintf(self->log_content, "<n> = %u", mode);
		writeLog(self, LOG_INFO, self->log_content, false);
	}
	else
	{
		sprintf(self->log_content, "<n> = %u is invalid.", mode);
		writeLog(self, LOG_ERROR, self->log_content, false);
		output_status = STATUS_BAD_PARAMETER;
	}

	sprintf(self->log_content, "Report Mobile Equipment Error is %s.",
			getStatusTypeString(output_status));
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

StatusType displayProductID(struct ViettelSDK *self)
{
	/* ATI */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", DISPLAY_PRODUCT_ID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 4000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Display Product ID is %s.",
			getStatusTypeString(output_status));
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

StatusType requestManufacturerID(struct ViettelSDK *self)
{
	/* AT+CGMI */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", REQUEST_MANUFACTURER_ID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Request Manufacturer ID is %s.",
			getStatusTypeString(output_status));
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

StatusType getAndSetMobileOperationBand(struct ViettelSDK *self)
{
	/* AT+CBAND= */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=1,3", GET_AND_SET_MOBILE_OPERATION_BAND);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Get And Set Mobile Operation Band is %s.",
			getStatusTypeString(output_status));
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

StatusType readReportNetworkState(struct ViettelSDK *self)
{
	/* AT+QENG? */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Write command */
	sprintf(self->command, "%s=0", REPORT_NETWORK_STATE);

	/* Send command */
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 1,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 3000);

	/* Get output status */
	output_status = self->command_response.status;

	/* Log status of command */
	sprintf(self->log_content, "Reading Report Network State Status is %s.",
			getStatusTypeString(output_status));

	/* Logical processing */
	if (output_status != STATUS_SUCCESS)
	{
		/* What to do if status is NOT SUCCESS */
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		/* What to do if status is SUCCESS */
		uint8_t num_tokens;
		char **token_array;

		token_array = tokenizeString(self, self->command_response.response, ":",
				&num_tokens);

		self->cell_data.cell_count = num_tokens;

		for (int i = 0; i < num_tokens; i++)
		{
			removeSubstring(token_array[i], "+QENG");
			removeSubstring(token_array[i], "OK");
			removeChars(token_array[i], "\"\r\n ");

			/* index = 1 is serving cell */
			if (i == 1)
			{
				char *ptr;
				char *data_ptr;
				uint8_t sign_count = 0;
				uint8_t number_of_elements = 0;

				/* Count number of comma since data may not be report */
				/* Example: 3,"A794",0,,-56 */
				ptr = strchr(token_array[i], ',');
				while (ptr)
				{
					ptr += 1;
					sign_count += 1;
					ptr = strchr(ptr, ',');
				}

				/* Initialize elemtent array */
				number_of_elements = sign_count + 1;
				char *string_array[number_of_elements];
				char clone_data[RESPONSE_MAIN_BUFFER_SIZE];
				uint8_t element_count = 0;

				data_ptr = token_array[i];
				strcpy(clone_data, data_ptr);
				ptr = strchr(clone_data, ',');
				sign_count = 0;
				self->token = strtok(data_ptr, ",");
				while (self->token && element_count < number_of_elements)
				{
					if (*(ptr + 1) == ',')
					{
						string_array[element_count] = static_null_string;
						element_count++;
					}
					else
					{
						/* Element footer */
						string_array[element_count] = self->token;
						self->token = strtok(NULL, ",");
						element_count++;
					}

					/* Delimiter footer */
					if (sign_count != 0)
					{
						ptr += 1;
						ptr = strchr(ptr, ',');
					}
					sign_count += 1;
				}

				/* Assign serving cell values */
				self->cell_data.serving_cell.sc_earfcn = atoi(					//Check at this line
						string_array[1]);
				self->cell_data.serving_cell.sc_earfcn_offset = strtoul(
						string_array[2], '\0', 10);
				self->cell_data.serving_cell.sc_pci = strtoul(string_array[3],
						'\0', 10);
				strcpy(self->cell_data.serving_cell.sc_cellid, string_array[4]);
				self->cell_data.serving_cell.sc_rsrp = atoi(string_array[5]);
				self->cell_data.serving_cell.sc_rsrq = atoi(string_array[6]);
				self->cell_data.serving_cell.sc_rssi = atoi(string_array[7]);
				self->cell_data.serving_cell.sc_snr = atoi(string_array[8]);
				self->cell_data.serving_cell.sc_band = strtoul(string_array[9],
						'\0', 10);
				strcpy(self->cell_data.serving_cell.sc_tac, string_array[10]);
				self->cell_data.serving_cell.sc_ecl = strtoul(string_array[11],
						'\0', 10);
				self->cell_data.serving_cell.sc_tx_pwr = atoi(string_array[12]);
				self->cell_data.serving_cell.sc_re_rsrp = atoi(
						string_array[12]);

			}
		}

		free(token_array);

		//addCENGData(self);
	}
	return output_status;
}

StatusType requestModelID(struct ViettelSDK *self)
{
	/* AT+CGMM */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s", REQUEST_MODEL_ID);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Request Model ID is %s.",
	getStatusTypeString(output_status));

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

StatusType requestInternationalMobileSubscriberIdentity(struct ViettelSDK *self)
{
	/* AT+CIMI */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s",
	REQUEST_INTERNATIONAL_MOBILE_SUBSCRIBER_IDENTITY);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content,
			"Request International Mobile Subscriber Identity is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		char res[30];
		strcpy(res, self->command_response.response);
		removeSubstring(res, "OK");
		removeChars(res, "\"\r\n ");
		strcpy(self->imsi,res);
	}
	return output_status;
}

StatusType readOperatorSelection(struct ViettelSDK *self)
{
	/* AT+COPS? */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", OPERATOR_SELECTION);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Read Operator Selection is %s.",
			getStatusTypeString(output_status));
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

StatusType readpreferredOperatorList(struct ViettelSDK *self)
{
	/* AT+CPOL? */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", PREFERRED_OPERATOR_LIST);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Preferred Operator List is %s.",
			getStatusTypeString(output_status));
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

StatusType setDefaultPSDConnection(struct ViettelSDK *self)
{
	/* AT+CGDCONT */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=\"ip\",\"nbiot\"", SET_DEFAULT_PSD_CONNECTION);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Set Default PSD Connection is %s.",
			getStatusTypeString(output_status));
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

StatusType readDynamicParamPDPContext(struct ViettelSDK *self, bool save_data)
{
	/* AT+CGCONTRDP= */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", READ_DYNAMIC_PARAM_PDP_CONTEXT);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 2,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Read Dynamic Param PDP Context is %s.",
			getStatusTypeString(output_status));

	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		char res[50];
		strcpy(res,self->command_response.response);
		removeSubstring(res, "+CGDCONT: ");
		removeSubstring(res, "OK");
		removeChars(res, "\"\r\n ");

		char *ptr;
		char *data_ptr;
		uint8_t sign_count = 0;
		uint8_t number_of_elements = 0;

		/* Count number of comma since data may not be report */
		/* Example: 3,"A794",0,,-56 */
		ptr = strchr(res, ',');
		while (ptr)
		{
			ptr += 1;
			sign_count += 1;
			ptr = strchr(ptr, ',');
		}

		/* Initialize elemtent array */
		number_of_elements = sign_count + 1;
		char *string_array[number_of_elements];
		char clone_data[50];
		uint8_t element_count = 0;

		data_ptr = res;
		strcpy(clone_data, data_ptr);
		ptr = strchr(clone_data, ',');
		sign_count = 0;
		self->token = strtok(data_ptr, ",");
		while (self->token && element_count < number_of_elements)
		{
			if (*(ptr + 1) == ',')
			{
				string_array[element_count] = static_null_string;
				element_count++;
			}
			else
			{
				/* Element footer */
				string_array[element_count] = self->token;
				self->token = strtok(NULL, ",");
				element_count++;
			}

			/* Delimiter footer */
			if (sign_count != 0)
			{
				ptr += 1;
				ptr = strchr(ptr, ',');
			}
			sign_count += 1;
		}

		strcpy(self->ipAddress,string_array[3]);
	}
	return output_status;
}

StatusType setAutoTimeUpdate(struct ViettelSDK *self)
{
	/* AT+CTZU= */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=1", AUTO_TIME_UPDATE);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Auto Time Update is %s.",
			getStatusTypeString(output_status));

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

StatusType enableLocalTimestamp(struct ViettelSDK *self)
{
	/*  AT+CLTS */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=1", ENABLE_LOCAL_TIMESTAMP);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Enable Local Timestamp is %s.",
			getStatusTypeString(output_status));

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

StatusType readClock(struct ViettelSDK *self, bool save_data)
{
	/* AT+CLCK=? */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s?", GET_CLOCK);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	output_status = self->command_response.status;
	sprintf(self->log_content, "Get Clock is %s.",
			getStatusTypeString(output_status));

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

StatusType checkSIM(struct ViettelSDK *self)
{
	/* AT+CPIN? */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
	sprintf(self->command, "%s?", ENTER_PIN);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 1,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);

	/* Get output status */
	output_status = self->command_response.status;

	/* Log status of command */
	sprintf(self->log_content, "Checking SIM Status is %s.",
			getStatusTypeString(output_status));

	/* Logical processing */
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
		/* What to do if status is NOT SUCCESS */
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		/* What to do if status is SUCCESS */
	}

	return output_status;
}

StatusType setPhoneFunctionality(struct ViettelSDK *self, uint8_t fun)
{
	/* AT+CFUN= */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	if ((fun != 0) && (fun != 1) && (fun != 4) && (fun != 7))
	{
	}

	/* Send command */
	sprintf(self->command, "%s=%u", PHONE_FUCNTIONALITY, fun);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);

	/* Get output status */
	output_status = self->command_response.status;

	/* Log status of command */
	sprintf(self->log_content, "Set Set Phone Functionality Status is %s.",
			getStatusTypeString(output_status));

	/* Logical processing */
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
		/* What to do if status is NOT SUCCESS */
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		/* What to do if status is SUCCESS */
	}

	return output_status;
}

StatusType readVoltage(struct ViettelSDK *self)
{
	/* AT+CBC */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s",READVOLTAGE);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content,
			"Query Power Supply Voltage is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		char res[15];
		strcpy(res, self->command_response.response);
		removeSubstring(res, "+CBC: ");
		removeSubstring(res, "OK");
		removeChars(res, "\"\r\n ");
		self->voltage = atoi(res);
		output_status = STATUS_SUCCESS;
	}
	return output_status;
}
