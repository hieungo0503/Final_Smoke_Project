#include "ViettelSDK_Network_Management.h"

StatusType checkCoapConnection(struct ViettelSDK *self)
{
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
	sprintf(self->command, "%s?", CREATE_COAP_CONTEXT);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT); /* Configured for PSM mode, DO NOT CHANGE */

	/* Get output status */
	output_status = self->command_response.status;
	/* Log status of command */
	sprintf(self->log_content, "Checking COAP Connection Status is %s.",
	getStatusTypeString(output_status));
	/* Logical processing */
		if (output_status != STATUS_SUCCESS)
		{
			/* What to do if status is NOT SUCCESS */
			writeLog(self, LOG_WARNING, self->log_content, false);
		}
		else
		{
			if(!strstr(self->command_response.response,":"))
			{
				self->coap_params.connected = false;
				writeLog(self, LOG_INFO, self->log_content, false);
			}
			else
			{
				/* What to do if status is SUCCESS */
				writeLog(self, LOG_INFO, self->log_content, false);
				char res[50];
				strcpy(res,self->command_response.response);
				removeSubstring(res, "+QCOAPOPEN: ");
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

				if(strstr(string_array[3],"1") || strstr(string_array[3],"2"))
					self->coap_params.connected = 1;
				else
					self->coap_params.connected = 0;

				sprintf(self->log_content,
						"Read COAP parameters:\ClientID_id: %s\nCoAP_Server: %s\nPort: %s\nStatus: %d",
						string_array[0], string_array[1],string_array[2],
						self->coap_params.connected);
				writeLog(self, LOG_INFO, self->log_content, false);
			}
		}

		return output_status;

}
StatusType COAPDisconnect(struct ViettelSDK *self)
{
	/* AT+QCOAPCLOSE */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%u", COAP_DISCONNECT,
			self->coap_params.client_id);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "COAP disconnect is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		self->mqtt_params.connected = false;
	}
	return output_status;
}
StatusType newCOAP(struct ViettelSDK *self)
{
	/* AT+QCOAPOPEN */
	StatusType output_status = STATUS_UNKNOWN;

	sprintf(self->command, "%s=%u,\"%s\",%u", CREATE_COAP_CONTEXT,
			self->coap_params.client_id,self->coap_params.server, self->coap_params.port);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 1,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 3000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Initialize COAP is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
		self->mqtt_params.connected = false;
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		self->coap_params.connected= true;
	}
	return output_status;
}

StatusType CreateCoAPContex(struct ViettelSDK *self)
{
	/* AT+QCOAPOPEN */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
	sprintf(self->command, "%s=%u,\"%s\",%u",CREATE_COAP_CONTEXT,
			self->coap_params.client_id,
			self->coap_params.server,
			self->coap_params.port);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 4, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 4000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Send CoAP connection is %s.",
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

StatusType ConfigCoApHeader(struct ViettelSDK *self)
{
	/* AT+QCOAPOPTION */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
		sprintf(self->command, "%s=%u,2",CONFIG_COAP_HEADER,
			self->coap_params.client_id);

	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 4, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 4000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Generate message ID randomly, token value is omitted is %s.",
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

StatusType ConfigCoApMessageOpt(struct ViettelSDK *self, char *url)
{
	/* AT+QCOAPOPTION */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
		sprintf(self->command, "%s=%u,0,11,%u,\"%s\"",CONFIG_COAP_MESSAGE_OPTIONS,
			self->coap_params.client_id,strlen(url),url);

	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 4, RUN_COMMAND_TIMEOUT_MS_DEFAULT + 4000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Send CoAP connection is %s.",
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

StatusType sendCoAPMessage(struct ViettelSDK *self, char message[],uint8_t type, uint8_t method)
{
	/* AT*CMQPUB */
	StatusType output_status = STATUS_UNKNOWN;

	if(message == NULL)
	sprintf(self->command, "%s=%u,%u,%u,0,0",			//send with Empty Message
		SEND_COAP_MESSAGE,self->coap_params.client_id, type, method);
	else
	sprintf(self->command, "%s=%u,%u,%u,205,%u,\"%s\"",				//send with Message
	SEND_COAP_MESSAGE,self->coap_params.client_id, type, method,
			strlen(message)/2, message);

	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 1,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);
	output_status = self->command_response.status;

	sprintf(self->log_content, "Send COAP message is %s.",
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


/* MQTT */
StatusType checkMQTTConnection(struct ViettelSDK *self)
{
	/* AT+CMQNEW? */
	/* Initialize STATUS_UNKNOWN */
	StatusType output_status = STATUS_UNKNOWN;

	/* Send command */
	sprintf(self->command, "%s?", NEW_MQTT);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT); /* Configured for PSM mode, DO NOT CHANGE */

	/* Get output status */
	output_status = self->command_response.status;

	/* Log status of command */
	sprintf(self->log_content, "Checking MQTT Connection Status is %s.",
			getStatusTypeString(output_status));

	/* Logical processing */
	if (output_status != STATUS_SUCCESS)
	{
		/* What to do if status is NOT SUCCESS */
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		/* What to do if status is SUCCESS */
		writeLog(self, LOG_INFO, self->log_content, false);
		uint8_t num_tokens;
		char **token_array = tokenizeString(self,
				self->command_response.response, ":", &num_tokens);
		removeSubstring(token_array[1], "OK");
		removeChars(token_array[1], "\r\n ");

		char **sub_token_array = tokenizeString(self, token_array[1], ",",
				&num_tokens);
		if (num_tokens != 3)
		{
			output_status = STATUS_IMPROPER_DATA;
			sprintf(self->log_content,
					"Checking MQTT Connection Response is %s",
					getStatusTypeString(output_status));
			writeLog(self, LOG_ERROR, self->log_content, false);
			return output_status;
		}

		if (!strcmp(sub_token_array[2], "null"))
		{
			self->mqtt_params.connected = false;
		}
		else
		{
			self->mqtt_params.connected = true;
		}

		sprintf(self->log_content,
				"Read MQTT parameters:\nmqtt_id: %s\nused_state: %s\nserver: %s\nconnected: %d",
				sub_token_array[0], sub_token_array[1], sub_token_array[2],
				self->mqtt_params.connected);
		writeLog(self, LOG_INFO, self->log_content, false);
	}

	return output_status;
}

StatusType newMQTT(struct ViettelSDK *self)
{
	/* AT*CMQNEW */
	StatusType output_status = STATUS_UNKNOWN;

	sprintf(self->command, "%s=\"%s\",\"%s\",%u,%u", NEW_MQTT,
			self->mqtt_params.server, self->mqtt_params.port,
			self->mqtt_params.command_timeout_ms, self->mqtt_params.bufsize);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 1,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 3000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Initialize MQTT is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
		self->mqtt_params.resendNewMQTT = false;
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		self->mqtt_params.connected = true;
		if (strstr(self->command_response.response, "+CEREG"))
		{
			self->mqtt_params.resendNewMQTT = true;
		}
		else
		{
			self->mqtt_params.resendNewMQTT = false;
		}
	}
	return output_status;
}

StatusType sendMQTTConnect(struct ViettelSDK *self)
{
	/* AT*CMQCON */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%lu,%u,\"%s\",%u,%d,%d,\"%s\",\"%s\"",
	SEND_MQTT_CONNECT, self->mqtt_params.mqtt_id, self->mqtt_params.version,
			self->mqtt_params.client_id,
			MQTT_MAX_KEEP_ALIVE_INTERVAL, MQTT_CONNECT_CLEAN_SESSION,
			MQTT_CONNECT_WILL_FLAG, self->mqtt_params.mqtt_username,
			self->mqtt_params.mqtt_password);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT + 4,
	RUN_COMMAND_TIMEOUT_MS_DEFAULT + 4000);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Send MQTT connection is %s.",
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

StatusType sendMQTTPub(struct ViettelSDK *self, char topic[], uint8_t qos,
bool retained, bool dup, char message[])
{
	/* AT*CMQPUB */
	StatusType output_status = STATUS_UNKNOWN;

	if (strlen(message) < MQTT_MESSAGE_MIN_LENGTH
			|| strlen(message) > MQTT_MESSAGE_MAX_LENGTH)
	{
		output_status = STATUS_BAD_PARAMETER;
	}
	else
	{
		clearCommand(self);
		sprintf(self->command, "%s=%lu,\"%s\",%u,%d,%d,%u,\"%s\"",
		SEND_MQTT_PUB, self->mqtt_params.mqtt_id, topic, qos, retained, dup,
				strlen(message), message);
		self->command_response = sendCommand(self, self->command,
		RUN_COMMAND_COUNTER_DEFAULT + 1,
		RUN_COMMAND_TIMEOUT_MS_DEFAULT + 2000);
		output_status = self->command_response.status;
	}

	sprintf(self->log_content, "Send MQTT publish is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		self->mqtt_params.receiveSubcribe = false;
	}
	return output_status;
}

StatusType sendMQTTSub(struct ViettelSDK *self, char topic[], uint8_t qos)
{
	/* AT*CMQPUB */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%lu,\"%s\",%u", SEND_MQTT_SUB,
			self->mqtt_params.mqtt_id, topic, qos);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Send MQTT subcribe is %s.",
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

StatusType sendMQTTUnsub(struct ViettelSDK *self, char topic[])
{
	/* AT*CMQPUNSUB */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%lu,\"%s\"", SEND_MQTT_UNSUB,
			self->mqtt_params.mqtt_id, topic);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "Send MQTT unsubcribe is %s.",
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

StatusType MQTTDisconnect(struct ViettelSDK *self)
{
	/* AT+CMQDISCON */
	StatusType output_status = STATUS_UNKNOWN;
	sprintf(self->command, "%s=%lu", MQTT_DISCONNECT,
			self->mqtt_params.mqtt_id);
	self->command_response = sendCommand(self, self->command,
	RUN_COMMAND_COUNTER_DEFAULT, RUN_COMMAND_TIMEOUT_MS_DEFAULT);
	output_status = self->command_response.status;
	sprintf(self->log_content, "MQTT disconnect is %s.",
			getStatusTypeString(output_status));
	if (output_status != STATUS_SUCCESS)
	{
		writeLog(self, LOG_WARNING, self->log_content, false);
	}
	else
	{
		writeLog(self, LOG_INFO, self->log_content, false);
		self->mqtt_params.connected = false;
	}
	return output_status;
}
