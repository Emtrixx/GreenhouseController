/*
 ===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "heap_lock_monitor.h"
#include "retarget_uart.h"

#include "ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"

#include "utils/Globals.h"
#include "input/rotaryinput.h"
#include "ui/Menu.h"

// TODO: insert other definitions and declarations here

/* The following is required if runtime statistics are to be collected
 * Copy the code to the source file where other you initialize hardware */
extern "C" {

void vConfigureTimerForRunTimeStats(void) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}
/* end runtime statictics collection */

static void idle_delay() {
	vTaskDelay(1);
}

void task1(void *params) {
	(void) params;

	retarget_init();

//	DigitalIoPin sw_a2(1, 8, DigitalIoPin::pullup, true);
//	DigitalIoPin sw_a3(0, 5, DigitalIoPin::pullup, true);
//	DigitalIoPin sw_a4(0, 6, DigitalIoPin::pullup, true);
	DigitalIoPin sw_a5(0, 7, DigitalIoPin::pullup, true);

	DigitalIoPin *rs = new DigitalIoPin(0, 29, DigitalIoPin::output);
	DigitalIoPin *en = new DigitalIoPin(0, 9, DigitalIoPin::output);
	DigitalIoPin *d4 = new DigitalIoPin(0, 10, DigitalIoPin::output);
	DigitalIoPin *d5 = new DigitalIoPin(0, 16, DigitalIoPin::output);
	DigitalIoPin *d6 = new DigitalIoPin(1, 3, DigitalIoPin::output);
	DigitalIoPin *d7 = new DigitalIoPin(0, 0, DigitalIoPin::output);
	LiquidCrystal *lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);
	// configure display geometry
	lcd->begin(16, 2);
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd->setCursor(0, 0);
	// Print a message to the LCD.
	lcd->print("MQTT_FreeRTOS");

	Menu menu = Menu(lcd);
	menu.run_menu();
}

void modbusTask(void *params) {
	ModbusMaster gmp252(240);
	gmp252.begin(9600); // all nodes must operate at the same speed!
	gmp252.idle(idle_delay); // idle function is called while waiting for reply from slave
	ModbusMaster hmp60(241); // Create modbus object that connects to slave id 241 (HMP60)
	hmp60.begin(9600); // all nodes must operate at the same speed!
	hmp60.idle(idle_delay); // idle function is called while waiting for reply from slave

	ModbusRegister co2(&gmp252, 256, true);
	ModbusRegister rh(&hmp60, 256, true);
	ModbusRegister tempc(&hmp60, 257, true);

	const TickType_t xCharDelay = 5 / portTICK_PERIOD_MS;

	while(true) {
		vTaskDelay(xCharDelay);
		globalStruct.co2level = co2.read();
		vTaskDelay(xCharDelay);
		globalStruct.humidity = rh.read() / 10.0;
		vTaskDelay(xCharDelay);
		globalStruct.temperature = tempc.read() / 10.0;
		vTaskDelay(50);
	}

}

void keepCo2levelTask(void *params) {
	DigitalIoPin relay(0, 27, DigitalIoPin::output); // CO2 relay
	relay.write(0);
	int durationOpen = 0;
	int durationClosed = 0;
	int co2level = 0;
	int co2Target = 0;
	int relayPosition = 0;
	int lastTick = xTaskGetTickCount();

	while(true) {
		int temp = xTaskGetTickCount();
		if (relayPosition == 0) {
			durationClosed = durationClosed + (temp - lastTick);
		} else {
			durationOpen = durationOpen + (temp - lastTick);
		}
		lastTick = temp;
		float opening = (1.0*durationOpen / (1.0*durationOpen + durationClosed)) * 100.0;
		globalStruct.valveOpeningPercentage = opening;

		co2level = globalStruct.co2level;
		co2Target = globalStruct.co2Target;
		if (co2level < co2Target)
		{
			relayPosition = 1;
		} else {
			relayPosition = 0;
		}
		relay.write(relayPosition);
		vTaskDelay(50);
	}
}

extern "C" {
void vStartSimpleMQTTDemo(void); // ugly - should be in a header
}

int main(void) {

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
	// Set the LED to the state of "On"
	Board_LED_Set(0, true);
#endif
#endif

	heap_monitor_setup();

	// Setup global state
	initializeGlobalStruct();

	// Setup input (rotary encoder)
	setup_input_gpios();

	// initialize RIT (= enable clocking etc.)
	//Chip_RIT_Init(LPC_RITIMER);
	// set the priority level of the interrupt
	// The level must be equal or lower than the maximum priority specified in FreeRTOS config
	// Note that in a Cortex-M3 a higher number indicates lower interrupt priority
	//NVIC_SetPriority( RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

	xTaskCreate(task1, "test",
	configMINIMAL_STACK_SIZE * 3, NULL, (tskIDLE_PRIORITY + 1UL), //If need to reduce, between * 2 and * 3
			(TaskHandle_t*) NULL);

	xTaskCreate(modbusTask, "modbusTask",
	configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 1UL), //If need to reduce, between * 3 and * 4
			(TaskHandle_t*) NULL);

	xTaskCreate(keepCo2levelTask, "keepCo2levelTask",
	configMINIMAL_STACK_SIZE * 3, NULL, (tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t*) NULL);



	vStartSimpleMQTTDemo();
	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}
