#ifndef VIETTELSDK_UTILITIES_H_
#define VIETTELSDK_UTILITIES_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "ViettelSDK_Device_Management.h"

struct ViettelSDK;
enum ValueType;
union Value;

char** tokenizeString(struct ViettelSDK *self, char *str, const char *delim,
		uint8_t *num_tokens);

void removeSubstring(char *str, const char *sub);

void removeChars(char *str, const char *sub);

void floatToString(float num, char *str, uint8_t decimal_digit);

char* castToString(union Value data, enum ValueType value_type);

uint8_t stringToBinary(char *binaryValue);

#endif /* VIETTELSDK_UTILITIES_H_ */
