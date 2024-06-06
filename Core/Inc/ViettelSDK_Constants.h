#ifndef VIETTELSDK_CONSTANTS_H_
#define VIETTELSDK_CONSTANTS_H_

#include "stdbool.h"

/* Initialize parameter constants */
static const char *SUCCESS_COMMAND_SIGN[] =
{ "OK\r\n", "\r\n\r\n" }; // intentionally left unused

static const char *ERROR_COMMAND_SIGN[] =
{ "ERROR\r\n", "ERROR" }; // intentionally left unused

static const char *PASSIVE_RESPONSE_SIGN[] =
{ "NORMAL POWER DOWN", "+QNBIOTEVENT: \"ENTER PSM", "+QNBIOTEVENT: \"EX", "+QCOAPURC: "}; // intentionally left unused

#ifndef COMMAND_DELAY_MS
#define COMMAND_DELAY_MS (1000U)
#endif

#ifndef COMMAND_SIZE
#define COMMAND_SIZE (1200U)
#endif

#ifndef CONNECTING_PERIOD_MS
#define CONNECTING_PERIOD_MS (3000U)
#endif

#ifndef DELIMITER_SIZE
#define DELIMITER_SIZE (4U)
#endif

#ifndef DETAIL_LOG_SETUP
#define DETAIL_LOG_SETUP true
#endif

#ifndef ERROR_RESPONSE_DELAY_MS
#define ERROR_RESPONSE_DELAY_MS (2000U)
#endif

#ifndef ERROR_RESPONSE_LENGTH
#define ERROR_RESPONSE_LENGTH (sizeof(ERROR_COMMAND_SIGN) / sizeof(ERROR_COMMAND_SIGN[0]))
#endif

#ifndef MAX_FAIL_INCREMENT
#define MAX_FAIL_INCREMENT (5U)
#endif

#ifndef MODULE_PWR_PIN_PULL_UP_DELAY_MS
#define MODULE_PWR_PIN_PULL_UP_DELAY_MS (300U) /* Configured for PSM mode, DO NOT CHANGE */
#endif

#ifndef MODULE_PWR_PIN_PULL_DOWN_DELAY_MS
#define MODULE_PWR_PIN_PULL_DOWN_DELAY_MS (800U) /* Configured for PSM mode, DO NOT CHANGE */
#endif

#ifndef PASSIVE_RESPONSE_LENGTH
#define PASSIVE_RESPONSE_LENGTH (sizeof(PASSIVE_RESPONSE_SIGN) / sizeof(PASSIVE_RESPONSE_SIGN[0]))
#endif

#ifndef RECEIVE_SUBSCRIBE_TIMEOUT
#define RECEIVE_SUBSCRIBE_TIMEOUT (10000U)
#endif

#ifndef RESPONSE_MAIN_BUFFER_SIZE
#define RESPONSE_MAIN_BUFFER_SIZE (1200U)
#endif

#ifndef RESPONSE_RECEIVE_BUFFER_SIZE
#define RESPONSE_RECEIVE_BUFFER_SIZE (1200U)
#endif

#ifndef PSM_RECEIVE_TAU_BUFFER_SIZE
#define PSM_RECEIVE_TAU_BUFFER_SIZE (26U)
#endif

#ifndef SLEEP_INTERVAL
#define SLEEP_INTERVAL (86400)
#endif

#ifndef STAGE_DELAY_MS
#define STAGE_DELAY_MS (2000U)
#endif

#ifndef SUCCESS_COMMAND_LENGTH
#define SUCCESS_COMMAND_LENGTH (200U)
#endif

#ifndef SUCCESS_RESPONSE_LENGTH
#define SUCCESS_RESPONSE_LENGTH (sizeof(SUCCESS_COMMAND_SIGN) / sizeof(SUCCESS_COMMAND_SIGN[0]))
#endif

#ifndef WAIT_FOR_PSM_MODE
#define WAIT_FOR_PSM_MODE (90000LU)
#endif

#ifndef WAKE_UP_MODULE_PUSH_COUNT
#define WAKE_UP_MODULE_PUSH_COUNT (2U) /* Configured for PSM mode, DO NOT CHANGE */
#endif

#ifndef WARMING_UP_COUNT
#define WARMING_UP_COUNT (4U) /* Configured for PSM mode, DO NOT CHANGE */
#endif

#ifndef WATCHDOG_TIMER
#define WATCHDOG_TIMER (false)
#endif

/* Debugger */
#ifndef LOG_CONTENT_SIZE
#define LOG_CONTENT_SIZE (1200U)
#endif

#ifndef LOG_TIMEOUT_MS
#define LOG_TIMEOUT_MS (600U)
#endif

/* Command attribute constants */
#ifndef CCID_MAX_LENGTH
#define CCID_MAX_LENGTH (20U)
#endif

#ifndef CLIENT_MAX_LENGTH
#define CLIENT_MAX_LENGTH (120U)
#endif

#ifndef IMEI_MAX_LENGTH
#define IMEI_MAX_LENGTH (16U)
#endif

#ifndef MAXIMUM_NEIGHBOR_CELL
#define MAXIMUM_NEIGHBOR_CELL (4U) /* Do not change this constant for SIMCom7020 */
#endif

#ifndef MESSAGE_SIZE
#define MESSAGE_SIZE (750U) /* DO NOT CHANGE. Must not exceeds 450 bytes since translate to HEX will exceeds 900 bytes (SAFE LIMITATION) */
#endif

#ifndef MQTT_BUFFER_SIZE
#define MQTT_BUFFER_SIZE (512U) /* Range: 20 - 1132 */
#endif

#ifndef COAP_BUFFER_SIZE
#define COAP_BUFFER_SIZE (1024U) /* Range: 20 - 1132 */
#endif

#ifndef MQTT_MAX_COMMAND_TIMEOUT_MS
#define MQTT_MAX_COMMAND_TIMEOUT_MS (60000U)
#endif

#ifndef MQTT_CONNECT_CLEAN_SESSION
#define MQTT_CONNECT_CLEAN_SESSION (1)
#endif

#ifndef MQTT_CONNECT_WILL_FLAG
#define MQTT_CONNECT_WILL_FLAG (0U)
#endif

#ifndef MQTT_MAX_KEEP_ALIVE_INTERVAL
#define MQTT_MAX_KEEP_ALIVE_INTERVAL (64800U)
#endif

#ifndef MQTT_MESSAGE_MIN_LENGTH
#define MQTT_MESSAGE_MIN_LENGTH (2U)
#endif

#ifndef MQTT_MESSAGE_MAX_LENGTH
#define MQTT_MESSAGE_MAX_LENGTH (900U) /* DO NOT CHANGE */
#endif

#ifndef MQTT_PASSWORD_MAX_LENGTH
#define MQTT_PASSWORD_MAX_LENGTH (100U)
#endif

#ifndef MQTT_SERVER_MAX_LENGTH
#define MQTT_SERVER_MAX_LENGTH (50U)
#endif

#ifndef MQTT_SERVER_PORT_MAX_LENGTH
#define MQTT_SERVER_PORT_MAX_LENGTH (6U)
#endif

#ifndef MQTT_USERNAME_MAX_LENGTH
#define MQTT_USERNAME_MAX_LENGTH (100U)
#endif

#ifndef NEIGHBOR_CELL_ELEMENT_COUNT
#define NEIGHBOR_CELL_ELEMENT_COUNT (4U)
#endif

#ifndef RUN_COMMAND_COUNTER_DEFAULT
#define RUN_COMMAND_COUNTER_DEFAULT (4U)
#endif

#ifndef RUN_COMMAND_TIMEOUT_MS_DEFAULT
#define RUN_COMMAND_TIMEOUT_MS_DEFAULT (2000U)
#endif

#ifndef SERVING_CELL_ELEMENT_COUNT
#define SERVING_CELL_ELEMENT_COUNT (13U)
#endif

#ifndef TA_REVISION_ID_MAX_LENGTH
#define TA_REVISION_ID_MAX_LENGTH (18U)
#endif

/* Data Constants */
#ifndef FLASH_ADDR_PAGE_126
#define FLASH_ADDR_PAGE_126 ((uint32_t)0x0801F800)
#endif

#ifndef FLASH_ADDR_PAGE_127
#define FLASH_ADDR_PAGE_127 ((uint32_t)0x0801FC00)
#endif

#ifndef FLASH_USER_START_ADDR
#define FLASH_USER_START_ADDR (FLASH_ADDR_PAGE_126)
#endif

#ifndef FLASH_USER_END_ADDR
#define FLASH_USER_END_ADDR (FLASH_ADDR_PAGE_127 + FLASH_PAGE_SIZE)
#endif

#ifndef KEY_MAX_LENGTH
#define KEY_MAX_LENGTH (30U)
#endif

/* Utilities Constants */
#ifndef MAXIMUM_TOKEN_COUNT
#define MAXIMUM_TOKEN_COUNT (15U)
#endif

#endif /* VIETTELSDK_CONSTANTS_H_ */
