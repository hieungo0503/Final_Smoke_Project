#ifndef VIETTELSDK_NETWORK_MANAGEMENT_H_
#define VIETTELSDK_NETWORK_MANAGEMENT_H_

#include "ViettelSDK_Device_Management.h"

/* MQTT */
StatusType checkMQTTConnection(struct ViettelSDK *self);

StatusType newMQTT(struct ViettelSDK *self);

StatusType sendMQTTConnect(struct ViettelSDK *self);

StatusType sendMQTTPub(struct ViettelSDK *self, char topic[], uint8_t qos,
bool retained, bool dup, char message[]);

StatusType sendMQTTSub(struct ViettelSDK *self, char topic[], uint8_t qos);

StatusType sendMQTTUnsub(struct ViettelSDK *self, char topic[]);

StatusType MQTTDisconnect(struct ViettelSDK *self);

/* COAP */
StatusType checkCoapConnecttion(struct ViettelSDK *self);

StatusType COAPDisconnect(struct ViettelSDK *self);

StatusType newCOAP(struct ViettelSDK *self);

StatusType CreateCoAPContex(struct ViettelSDK *self);

StatusType sendCoAPMessage(struct ViettelSDK *self, char message[],uint8_t type, uint8_t method);

StatusType ConfigCoApMessageOpt(struct ViettelSDK *self, char *url);

#endif /* VIETTELSDK_NETWORK_MANAGEMENT_H_ */
