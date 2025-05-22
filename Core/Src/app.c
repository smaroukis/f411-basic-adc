/*
 * app.c
 *
 *  Created on: May 22, 2025
 *      Author: s
 */
#include "app.h"    // Public interface
#include "main.h"  // or alternatively stm32f4xx_hal.h

#define NUM_LEDS 10

// Private variables (file scope)

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} LedPin;

LedPin led_pins[NUM_LEDS] = {
		{BAR0_GPIO_Port, BAR0_Pin},
		{BAR1_GPIO_Port, BAR1_Pin},
		{BAR2_GPIO_Port, BAR2_Pin},
		{BAR3_GPIO_Port, BAR3_Pin},
		{BAR4_GPIO_Port, BAR4_Pin},
		{BAR5_GPIO_Port, BAR5_Pin},
		{BAR6_GPIO_Port, BAR6_Pin},
		{BAR7_GPIO_Port, BAR7_Pin},
		{BAR8_GPIO_Port, BAR8_Pin},
		{BAR9_GPIO_Port, BAR9_Pin}
};

static uint16_t led_mask = 0x0001;

// Private function prototypes
static void updateLeds(void);

// Public Function Implementations
void app_init(void)
{
    // Initialization code, e.g., set up GPIOs or variables
}

void app_loop(void)
{
    // Repeatedly called from main loop
	static uint8_t i = 0;

	led_mask <<= 1;
	updateLeds();

	i++;
	if (i == 10 || led_mask == 0) {
		i = 0;
		led_mask = 0x0001;
	}

	HAL_Delay(200);

}

// Private/Internal Function Definitions
static void updateLeds(void)
{
	// Check bit mask and set corresponding ledPin port and pin
	for (int i=0; i < NUM_LEDS; i++) {
		HAL_GPIO_WritePin(
				led_pins[i].port,
				led_pins[i].pin,
				((led_mask >> i) & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}


}
