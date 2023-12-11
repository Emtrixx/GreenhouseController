/*
 * EepromUtil.cpp
 *
 *  Created on: 11 Dec 2023
 *      Author: Eelis
 */

#include "EepromUtil.h"
#include "FreeRTOS.h"
#include "task.h"



int readTargetValueEeprom()
{
	int targetValue;
	unsigned char* p = (unsigned char*) &targetValue;
	uint8_t ret_code;

	vTaskSuspendAll();
	ret_code = Chip_EEPROM_Read(0x00000100, p, 4);
    xTaskResumeAll();

	if (ret_code == IAP_CMD_SUCCESS) {
		printf("Successful EEPROM read\n");
		return targetValue;
	}
	else {
		printf("EEPROM read failed with code: %d\n", ret_code);
		return ret_code*-1;
	}
}

int saveTargetValueEeprom(int target)
{
	int targetValue = target;
	unsigned char* p = (unsigned char*) &targetValue;
	uint8_t ret_code;

	vTaskSuspendAll();
	ret_code = Chip_EEPROM_Write(0x00000100, p, 4);
	xTaskResumeAll();

	if (ret_code == IAP_CMD_SUCCESS) {
		printf("Successful EEPROM write\n");
	}
	else {
		printf("EEPROM write failed with code: %d\n", ret_code);
	}
	return ret_code;
}
