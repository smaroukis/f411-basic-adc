/*
 * app.c
 *
 *  Created on: May 22, 2025
 *      Author: s
 */
#include "app.h"    // Public interface
#include "main.h"  // or alternatively stm32f4xx_hal.h

// Private variables (file scope)
static uint32_t internal_counter = 0;

// Private function prototypes
static void internal_do_something(void);

// Public Function Implementations
void app_init(void)
{
    // Initialization code, e.g., set up GPIOs or variables
    internal_counter = 0;
}

void app_loop(void)
{
    // Repeatedly called from main loop
	internal_do_something();

	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  HAL_Delay(200);
}

// Private/Internal Function Definitions
static void internal_do_something(void)
{
    internal_counter++;
    // Add your periodic logic here
}
