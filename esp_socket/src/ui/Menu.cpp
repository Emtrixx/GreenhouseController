/*
 * Menu.cpp
 *
 *  Created on: Dec 5, 2023
 *      Author: micro
 */

#include "Menu.h"
#include "input/rotaryinput.h"
#include "utils/Globals.h"
#include "utils/EepromUtil.h"

Menu::Menu(LiquidCrystal* lcd)
	: lcd{lcd}
{
	currentState = ViewCo2Level;
}

Menu::~Menu() {
	// TODO
}

MenuState Menu::get_state() {
	return currentState;
}

void Menu::set_state(MenuState newState) {
	currentState = newState;
}

void Menu::run_menu()
{
	InputEvent inputEvent;

	char buffer[32];
	int relativeHumidity;
	int co2Level;
	int temperature;
	float valvePercentage;

	while (true) {

		switch (currentState) {
		case ViewCo2Level: {
			co2Level = globalStruct.co2level;
			snprintf(buffer, 32, "Co2=%dppm", co2Level);
			printf("res: %s\n", buffer);
			break;
		}
		case SelectCo2Target: {
			snprintf(buffer, 32, "Co2=%dppm", co2TargetSelection);
			printf("res: %s\n", buffer);
			break;
		}
		case ViewHumidity: {
			relativeHumidity = globalStruct.humidity;
			snprintf(buffer, 32, "RH=%d%%", relativeHumidity);
			printf("res: %s\n", buffer);
			break;
		}
		case ViewTemperature: {
			temperature = globalStruct.temperature;
			snprintf(buffer, 32, "Temp=%dC", temperature);
			printf("res: %s\n", buffer);
			break;
		}
		case ViewValvePercentage: {
			valvePercentage = globalStruct.valveOpeningPercentage;
			snprintf(buffer, 32, "Valve%%=%.1f", valvePercentage);
			printf("res: %s\n", buffer);
		}
		}

		lcd->clear();
		lcd->setCursor(0, 0);
		if (currentState != SelectCo2Target)
		{
			lcd->print("Readings:");
		} else {
			lcd->print("Set new target:");
		}
		lcd->setCursor(0, 1);
		// Print a message to the LCD.
		lcd->print(buffer);

		if (xQueueReceive(globalStruct.rotaryEncoderQueue, &inputEvent,
				5000) == pdTRUE) {
			handle_input(inputEvent);
		} else {
			idle();
		}
	}
}

void Menu::handle_input(InputEvent inputEvent) {
	switch (currentState) {
		case ViewCo2Level: {
			switch (inputEvent) {
				case CW_ROTATION: {
					set_state(ViewHumidity);
					break;
				}
				case CCW_ROTATION: {
					set_state(ViewValvePercentage);
					break;
				}
				case PUSH: {
					co2TargetSelection = globalStruct.co2Target;
					set_state(SelectCo2Target);
					break;
				}
			}
			break;
		}
		case ViewHumidity: {
			switch (inputEvent) {
				case CW_ROTATION: {
					set_state(ViewTemperature);
					break;
				}
				case CCW_ROTATION: {
					set_state(ViewCo2Level);
					break;
				}
				case PUSH: {
					set_state(SelectCo2Target);
					co2TargetSelection = globalStruct.co2Target;
					break;
				}
			}
			break;
		}
		case ViewTemperature: {
			switch (inputEvent) {
				case CW_ROTATION: {
					set_state(ViewValvePercentage);
					break;
				}
				case CCW_ROTATION: {
					set_state(ViewHumidity);
					break;
				}
				case PUSH: {
					set_state(SelectCo2Target);
					co2TargetSelection = globalStruct.co2Target;
					break;
				}

			}
			break;
		}
		case ViewValvePercentage: {
			switch (inputEvent) {
				case CW_ROTATION: {
					set_state(ViewCo2Level);
					break;
				}
				case CCW_ROTATION: {
					set_state(ViewTemperature);
					break;
				}
				case PUSH: {
					set_state(SelectCo2Target);
					co2TargetSelection = globalStruct.co2Target;
					break;
				}

			}
			break;
		}
		case SelectCo2Target: {
			switch (inputEvent) {
				case CW_ROTATION: {
					if (co2TargetSelection < 2000) {
						co2TargetSelection++;
					}
					break;
				}
				case CCW_ROTATION: {
					if (co2TargetSelection > 200) {
						co2TargetSelection--;
					}
					break;
				}
				case PUSH: {
					globalStruct.co2Target = co2TargetSelection;
					if (saveTargetValueEeprom(co2TargetSelection) != 0) {
						printf("Failed to save target to EEPROM\n");
					}
					set_state(ViewTemperature);
					break;
				}
			}
			break;
		}
	}
}

void Menu::idle() {
	if(currentState == SelectCo2Target)
	{
		return;
	}

	if(currentState == ViewCo2Level) {
		set_state(ViewHumidity);
	} else if (currentState == ViewHumidity) {
		set_state(ViewTemperature);
	} else if (currentState == ViewTemperature) {
		set_state(ViewValvePercentage);
	} else if (currentState == ViewValvePercentage) {
		set_state(ViewCo2Level);
	}
}
