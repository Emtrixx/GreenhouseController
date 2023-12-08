/*
 * Menu.cpp
 *
 *  Created on: Dec 5, 2023
 *      Author: micro
 */

#include "Menu.h"
#include "input/rotaryinput.h"

Menu::Menu() {
	currentState = ViewCo2Level;
	idleCounter = 0;
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

void Menu::handle_input(InputEvent inputEvent) {
	switch (get_state()) {
		case ViewCo2Level: {
			switch (inputEvent) {
				case CW_ROTATION: {
					set_state(ViewHumidity);
					break;
				}
				case CCW_ROTATION: {
					set_state(ViewTemperature);
					break;
				}
				case PUSH: {
					set_state(SelectCo2Level);
					break;
				}
			}
			break;
		}
		case SelectCo2Level: {
			switch (inputEvent) {
				case CW_ROTATION: {
//					TODO: set co2 level
					break;
				}
				case CCW_ROTATION: {
//					TODO: set co2 level
					break;
				}
				case PUSH: {
					set_state(ViewCo2Level);
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
					break;
				}
			}
			break;
		}
		case ViewTemperature: {
			switch (inputEvent) {
				case CW_ROTATION: {
					set_state(ViewCo2Level);
					break;
				}
				case CCW_ROTATION: {
					set_state(ViewHumidity);
					break;
				}
				case PUSH: {
					break;
				}

			}
			break;
		}
	}
}

void Menu::idle() {
	if(idleCounter == 0) {
		set_state(ViewCo2Level);
	} else if (idleCounter == 1) {
		set_state(ViewHumidity);
	} else if (idleCounter == 2) {
		set_state(ViewTemperature);
	}

	idleCounter = (idleCounter + 1) % 3;
}
