#ifndef VIETTELSDK_COLLECTIONS_H_
#define VIETTELSDK_COLLECTIONS_H_

//#include "gpio.h"
#include "stdbool.h"
#include "stdint.h"

#include "ViettelSDK_Constants.h"

typedef enum StatusType
{
	STATUS_SUCCESS = 0,
	STATUS_ERROR,
	STATUS_INVALID_HANDLE,
	STATUS_MODEM_NOT_READY,
	STATUS_LIBRARY_NOT_OPEN,
	STATUS_LIBRARY_ALREADY_OPEN,
	STATUS_BAD_PARAMETER,
	STATUS_NO_MEMORY,
	STATUS_TIMEOUT,
	STATUS_SOCKET_CLOSED,
	STATUS_SOCKET_NOT_CONNECTED,
	STATUS_INTERNAL_FAILURE,
	STATUS_RESOURCE_CREATION_FAIL,
	STATUS_UNSUPPORTED,
	STATUS_NOT_ALLOWED,
	STATUS_IMPROPER_DATA,
	STATUS_UNKNOWN
} StatusType;

static char* getStatusTypeString(StatusType status)
{
	switch (status)
	{
	case STATUS_SUCCESS:
		return "SUCCESS";
	case STATUS_ERROR:
		return "ERROR";
	case STATUS_INVALID_HANDLE:
		return "INVALID HANDLE";
	case STATUS_MODEM_NOT_READY:
		return "MODEM NOT READY";
	case STATUS_LIBRARY_NOT_OPEN:
		return "LIBRARY NOT OPEN";
	case STATUS_LIBRARY_ALREADY_OPEN:
		return "LIBRARY ALREADY OPEN";
	case STATUS_BAD_PARAMETER:
		return "BAD PARAMETER";
	case STATUS_NO_MEMORY:
		return "NO MEMORY";
	case STATUS_TIMEOUT:
		return "TIMEOUT";
	case STATUS_SOCKET_CLOSED:
		return "SOCKET CLOSED";
	case STATUS_SOCKET_NOT_CONNECTED:
		return "SOCKET NOT CONNECTED";
	case STATUS_INTERNAL_FAILURE:
		return "INTERNAL FAILURE";
	case STATUS_RESOURCE_CREATION_FAIL:
		return "RESOURCE CREATION FAIL";
	case STATUS_UNSUPPORTED:
		return "UNSUPPORTED";
	case STATUS_NOT_ALLOWED:
		return "NOT ALLOWED";
	case STATUS_IMPROPER_DATA:
		return "IMPROPER DATA";
	case STATUS_UNKNOWN:
		return "UNKNOWN";
	default:
		return "INVALID STATUS";
	}
}

typedef enum LogType
{
	LOG_INFO = 0, LOG_WARNING, LOG_ERROR, LOG_DEBUG, LOG_EMPTY
} LogType;

typedef struct CommandResponse
{
	StatusType status;
	char response[RESPONSE_MAIN_BUFFER_SIZE];
} CommandResponse;

typedef struct ServingCell
{
	uint32_t sc_earfcn;
	uint8_t sc_earfcn_offset;
	uint16_t sc_pci;
	char sc_cellid[9];
	int16_t sc_rsrp;
	int16_t sc_rsrq;
	int16_t sc_rssi;
	int16_t sc_snr;
	uint8_t sc_band;
	char sc_tac[5];
	uint8_t sc_ecl;
	int16_t sc_tx_pwr;
	int16_t sc_re_rsrp;
	uint32_t T3324;
	uint32_t T3412;

} ServingCell;

typedef struct NeighborCell
{
	uint32_t nc_earfcn;
	uint8_t nc_earfcn_offset;
	uint16_t nc_pci;
	int16_t nc_rsrp;
} NeighborCell;

typedef struct Cell
{
	uint8_t cell_count;
	struct ServingCell serving_cell;
	struct NeighborCell neighbor_cell_array[MAXIMUM_NEIGHBOR_CELL];
} Cell;

typedef struct MQTTParams
{
	bool receiveSubcribe;
	uint32_t receiveSubcribeTimeout;
	bool resendNewMQTT;
	bool connected;
	uint32_t mqtt_id;
	bool used_state;
	char server[MQTT_SERVER_MAX_LENGTH];
	char port[MQTT_SERVER_PORT_MAX_LENGTH];
	uint16_t command_timeout_ms;
	uint16_t bufsize;
	uint8_t cid;
	char client_id[CLIENT_MAX_LENGTH];
	uint8_t version;
	char mqtt_username[MQTT_USERNAME_MAX_LENGTH];
	char mqtt_password[MQTT_PASSWORD_MAX_LENGTH];
	/* Some modules are only able to send HEX string, then we need a message_buffer string with size of MESSAGE_SIZE
	 * and a message string to send HEX string with size of (MESSAGE_SIZE * 2).
	 * Otherwise, if module supports original string, then we do not need message_buffer variable. */
	char *message_buffer;
	char *message;
} MQTTParams;

typedef struct COAPParams
{
	char server[50];
	uint16_t port;
	uint8_t client_id;
	bool connected;
	char *message_buffer;
	char *message;

}COAPParams;

typedef struct Pin
{
	GPIO_TypeDef *pin_GPIOx;
	uint16_t pin_GPIO_Pin;
} Pin;

#endif /* VIETTELSDK_COLLECTIONS_H_ */
