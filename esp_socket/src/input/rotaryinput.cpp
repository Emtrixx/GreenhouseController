
/*
 * rotaryinput.cpp
 *
 *  Created on: Nov 28, 2023
 *      Author: micro
 */

#include "rotaryinput.h"
#include "FreeRTOS.h"
#include "task.h"
#include "heap_lock_monitor.h"
#include "semphr.h"
#include "timers.h"

#include "utils/Globals.h"

#define LEN 60

// GPIO
#define PORT_0 0
#define PORT_1 1
#define ROT_A_PIN 5
#define ROT_B_PIN 6
#define ROT_PUSH_PIN 8
#define PININT_INDEX   0	/* PININT index used for GPIO mapping */
#define PININT_INDEX_2 1
#define PININT_IRQ_HANDLER  PIN_INT0_IRQHandler	/* GPIO interrupt IRQ function name */
#define PININT_IRQ_HANDLER_2  PIN_INT1_IRQHandler
#define PININT_NVIC_NAME    PIN_INT0_IRQn	/* GPIO interrupt NVIC interrupt name */
#define PININT_NVIC_NAME_2    PIN_INT1_IRQn


/*****************************************************************************
 * Types
 ****************************************************************************/

static TickType_t lastActivation;
static TickType_t currentActivation;

static TickType_t lastActivationPush;
static TickType_t currentActivationPush;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from GPIO pin or GPIO pin mapped to PININT
 * @return	Nothing
 */
extern "C" {
void PIN_INT0_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

	// Debounce
	currentActivation = xTaskGetTickCountFromISR();
	if (currentActivation - lastActivation >= 30) {

		InputEvent val = CW_ROTATION;
		bool pinStateB = Chip_GPIO_GetPinState(LPC_GPIO, PORT_0, ROT_B_PIN);

		if (pinStateB) {
			val = CCW_ROTATION;
		}

		xQueueSendToBackFromISR(globalStruct.rotaryEncoderQueue, &val,
				&pxHigherPriorityTaskWoken);
	}

	lastActivation = currentActivation;

	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}

void PIN_INT1_IRQHandler(void) {
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_2));
	BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

	// Debounce
	currentActivationPush = xTaskGetTickCountFromISR();
	if (currentActivationPush - lastActivationPush >= 30) {

		InputEvent val = PUSH;

		xQueueSendToBackFromISR(globalStruct.rotaryEncoderQueue, &val,
				&pxHigherPriorityTaskWoken);
	}

	lastActivationPush = currentActivationPush;

	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}
}



void setup_input_gpios(void) {
	// GPIOs
	/* Initialize PININT driver */
	Chip_PININT_Init(LPC_GPIO_PIN_INT);

	/* Set pins to PULLUP mode */
	Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, ROT_A_PIN,
			(IOCON_DIGMODE_EN | IOCON_MODE_PULLUP));
	Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_0, ROT_B_PIN,
			(IOCON_DIGMODE_EN | IOCON_MODE_PULLUP));
	Chip_IOCON_PinMuxSet(LPC_IOCON, PORT_1, ROT_PUSH_PIN,
				(IOCON_DIGMODE_EN | IOCON_MODE_PULLUP));

	/* Configure GPIO pins as input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, PORT_0, ROT_A_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, PORT_0, ROT_B_PIN);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, PORT_1, ROT_PUSH_PIN);

	/* Enable PININT clock */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);

	/* Reset the PININT block */
	Chip_SYSCTL_PeriphReset(RESET_PININT);

	/* Configure interrupt channel for the GPIO pin in INMUX block */
	Chip_INMUX_PinIntSel(PININT_INDEX, PORT_0, ROT_A_PIN);
	Chip_INMUX_PinIntSel(PININT_INDEX_2, PORT_1, ROT_PUSH_PIN);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_2));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_2));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX_2));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(PININT_NVIC_NAME);
	NVIC_EnableIRQ(PININT_NVIC_NAME);
	NVIC_SetPriority(PININT_NVIC_NAME,
	configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);

	NVIC_ClearPendingIRQ(PININT_NVIC_NAME_2);
	NVIC_EnableIRQ(PININT_NVIC_NAME_2);
	NVIC_SetPriority(PININT_NVIC_NAME_2,
	configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
}
