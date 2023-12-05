/*
 * Globals.h
 *
 *  Created on: Dec 5, 2023
 *      Author: micro
 */

#ifndef UTILS_GLOBALS_H_
#define UTILS_GLOBALS_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"


typedef struct {
    QueueHandle_t rotaryEncoderQueue;
    uint32_t co2level;
    uint32_t humidity;
    uint32_t temperature;
    uint32_t valveOpeningPercentage;
    uint32_t co2SetPoint;
} GlobalStruct_t;

extern GlobalStruct_t globalStruct;

void initializeGlobalStruct();

#endif /* UTILS_GLOBALS_H_ */
