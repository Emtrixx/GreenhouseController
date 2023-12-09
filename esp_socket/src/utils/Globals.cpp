/*
 * Globals.cpp
 *
 *  Created on: Dec 5, 2023
 *      Author: micro
 */


#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "Globals.h"

#include "input/rotaryinput.h"


GlobalStruct_t globalStruct;

void initializeGlobalStruct() {
	globalStruct.rotaryEncoderQueue = xQueueCreate(32, sizeof(InputEvent));
	globalStruct.co2SetPoint = 421;
}
