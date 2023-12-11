/*
 * EepromUtil.h
 *
 *  Created on: 11 Dec 2023
 *      Author: Eelis
 */

#ifndef UTILS_EEPROMUTIL_H_
#define UTILS_EEPROMUTIL_H_

int readTargetValueEeprom(); //return negative if failed
int saveTargetValueEeprom(int target); //return nonzero if failed



#endif /* UTILS_EEPROMUTIL_H_ */
