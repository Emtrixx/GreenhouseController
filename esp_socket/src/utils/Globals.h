/*
 * Globals.h
 *
 *  Created on: Dec 5, 2023
 *      Author: micro
 */

#ifndef UTILS_GLOBALS_H_
#define UTILS_GLOBALS_H_

//#include <atomic>

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"


typedef struct {
    QueueHandle_t rotaryEncoderQueue;
//    	std::atomic<int> co2level;
    	int co2level;
    	int humidity;
    	int temperature;
    	float valveOpeningPercentage;
    	int co2Target;
//    std::atomic<int> humidity;
//    std::atomic<int> temperature;
//    std::atomic<float> valveOpeningPercentage;
//    std::atomic<int> co2Target;
} GlobalStruct_t;

extern GlobalStruct_t globalStruct;

void initializeGlobalStruct();

#endif /* UTILS_GLOBALS_H_ */
