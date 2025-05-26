/*
 * app.c
 *
 *  Created on: May 22, 2025
 *      Author: s
 */
#include "app.h"    // Public interface
#include "util.h"
#include "main.h"  // or alternatively stm32f4xx_hal.h
#include "tm1637.h"
#include <string.h> // for memset

#define ADC_MAX_VALUE_12B 4095

#define NUM_LEDS 10
#define RANGE_IN_MIN 0
#define RANGE_IN_MAX ADC_MAX_VALUE_12B
#define RANGE_OUT_MIN 0
#define RANGE_OUT_MAX 10


// ==================  External Variables Only Used Here =========================================
extern ADC_HandleTypeDef hadc1;


// ==================  Private Variables (File Scope)  =================================

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

static uint16_t led_mask = 0x0000;

// =======================  Private function prototypes =======================================
static void updateLeds(void);
static void updateMaskFromInt(uint16_t value);

// =======================  Public Function Implementations =======================================
void app_init(void)
{
    // Initialization code, e.g., set up GPIOs or variables
	tm1637_init();
	tm1637_unset_all();
}

// Read ADC and update Bargraph to represent value from 1-10
void app_loop(void)
{

	uint8_t buff[] = {0xFF, 0xFF, 0xFF, 0xFF};

	memset(buff, 0xFF, sizeof(buff));
	tm1637_write_packet(buff, 4);
	HAL_Delay(1000);

	memset(buff, 0, sizeof(buff));
	tm1637_write_packet(buff, 4);
	HAL_Delay(1000);

//	HAL_ADC_Start(&hadc1);
//	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
//	uint16_t adc_value = HAL_ADC_GetValue(&hadc1);

//	int val = mapInt(adc_value, RANGE_IN_MIN, RANGE_IN_MAX, RANGE_OUT_MIN, RANGE_OUT_MAX); // 2730/4096 -> 6/10

//	updateMaskFromInt(val);

//	updateLeds();

//	HAL_Delay(200);

}

// =======================  Private Function Definitions =======================================
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

// Updates the global led pin mask so that bottom N LEDs are lit for value N
// value must be between 0  NUM_LEDS
static void updateMaskFromInt(uint16_t value) {
	// Convert value into a mask
	if (value > NUM_LEDS) value = NUM_LEDS;
	led_mask = (1 << value) - 1; // subtracting 1 sets all the lower bits
}



