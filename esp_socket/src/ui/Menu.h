/*
 * Menu.h
 *
 *  Created on: Dec 5, 2023
 *      Author: micro
 */

#ifndef UI_MENU_CPP_
#define UI_MENU_CPP_

#include "input/rotaryinput.h"
#include "LiquidCrystal.h"

enum MenuState {
	ViewCo2Level = 0,
	SelectCo2Target,
	ViewHumidity,
	ViewTemperature,
	ViewValvePercentage,
};


class Menu {
public:
	Menu(LiquidCrystal* lcd);
	virtual ~Menu();
	void set_state(MenuState newState);
	MenuState get_state();
	void run_menu();
	void handle_input(InputEvent inputEvent);
	void idle();
private:
	MenuState currentState;
	int co2TargetSelection;
	LiquidCrystal* lcd;
};

#endif /* UI_MENU_CPP_ */


