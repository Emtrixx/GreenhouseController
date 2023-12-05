/*
 * Menu.h
 *
 *  Created on: Dec 5, 2023
 *      Author: micro
 */

#ifndef UI_MENU_CPP_
#define UI_MENU_CPP_

#include "input/rotaryinput.h"

enum MenuState {
	ViewCo2Level = 0,
	SelectCo2Level,
	ViewHumidity,
	ViewTemperature,
	ViewValvePercentage,
};


class Menu {
public:
	Menu();
	virtual ~Menu();
	void set_state(MenuState newState);
	MenuState get_state();
	void handle_input(InputEvent inputEvent);
	void idle();
private:
	MenuState currentState;
	int idleCounter;
};


#endif /* UI_MENU_CPP_ */


