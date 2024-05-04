#ifndef VIETTELSDK_DATA_MANAGEMENT_H_
#define VIETTELSDK_DATA_MANAGEMENT_H_

#include "stdint.h"
#include "math.h"

#include "ViettelSDK_Device_Management.h"

// Forward declaration of ViettelSDK struct
struct ViettelSDK;

typedef enum ValueType
{
	/* bool */
	VALUE_BOOL,

	/* 1 byte */
	VALUE_CHAR, VALUE_UNSIGNED_CHAR, VALUE_UINT8_T,

	/* 2 bytes */
	VALUE_SHORT, VALUE_UNSIGNED_SHORT, VALUE_UINT16_T,

	/* 4 bytes */
	VALUE_INT, VALUE_UINT32_T,

	/* 4 or 8 bytes */
	VALUE_LONG,

	/* Decimal point */
	VALUE_FLOAT,

	/* String */
	VALUE_CHAR_ARRAY

} ValueType;

// Union to store different types of values
typedef union Value
{
	bool *bool_value;
	char *char_value;
	unsigned char *unsigned_char_value;
	uint8_t *uint8_value;
	short *short_value;
	unsigned short *unsigned_short_value;
	uint16_t *uint16_value;
	int *int_value;
	long *long_value;
	uint32_t *uint32_value;
	float *float_value;
	char *char_array_value;
} Value;

// Struct to represent a key-value pair with a variable value datatype
typedef struct Pair
{
	char key[KEY_MAX_LENGTH];
	Value value;
	enum ValueType value_type;
} Pair;

typedef struct DataNode
{
	struct Pair pair;
	struct DataNode *next;
} DataNode;

void addData(struct ViettelSDK *self, char key[], void *value,
		enum ValueType value_type);

void addCENGData(struct ViettelSDK *self);

void addNetworkData(struct ViettelSDK *self);

void addBatteryVoltage(struct ViettelSDK *self);

void addSimProfile(struct ViettelSDK *self);

uint32_t PSM_timerDecodeT3412(uint8_t  binaryValue);

uint32_t PSM_timerDecodeT3324(uint8_t  binaryValue);

void addDeviceID(struct ViettelSDK *self, char ID[]);

void packData(struct ViettelSDK *self);

void getSensorData(struct ViettelSDK *self);

void updateFLASHData(struct ViettelSDK *self);

void addSensorData(struct ViettelSDK *self, struct Smoke_Data *smoke_hler);

#endif /* VIETTELSDK_DATA_MANAGEMENT_H_ */
