/*
 * app.c
 *
 *  Created on: May 22, 2025
 *      Author: s
 */
// Testing only

// testing end

#include "app.h"    // Public interface
#include "util.h"
#include "main.h"  // or alternatively stm32f4xx_hal.h
#include "tm1637.h"
#include <string.h> // for memset
#include <math.h> // for rounding instead of truncating

// for Retargeting printf
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <unistd.h>


#define ADC_MAX_VALUE_12B 4095

#define NUM_LEDS 10
#define RANGE_IN_MIN 0
#define RANGE_IN_MAX ADC_MAX_VALUE_12B
#define RANGE_OUT_MIN 0
#define RANGE_OUT_MAX 3.3f
#define AVDD_VREF 3.3f

#define HVBUS_RSERIES 93800.0f
#define HVBUS_RSHUNT 2000.0f
#define AMC_GAIN 2.0f
#define HVBUS_GAIN 1.65f

// Compile time constants for ADC conversion
static const float ADC_COUNT_TO_VOLT 		= AVDD_VREF / (float)ADC_MAX_VALUE_12B;
static const double HVBUS_PATH_GAIN_FULL =
			(double) AMC_GAIN
		*	(double) HVBUS_GAIN
		* 	(double) HVBUS_RSHUNT
		/	(double) (HVBUS_RSHUNT + HVBUS_RSERIES);

static const double INV_HVBUS_PATH_GAIN_FULL = 1.0f/(HVBUS_PATH_GAIN_FULL);
static const float ADC_COUNT_TO_HVBUS_VOLT_FULL = ADC_COUNT_TO_VOLT * INV_HVBUS_PATH_GAIN_FULL;



// ==================  External Variables Only Used Here =========================================
extern ADC_HandleTypeDef hadc1; // adc1 channel 8 is PB0
extern UART_HandleTypeDef huart2; // st-link vcp wired to uart2


// ==================  Private Variables (File Scope)  =================================
uint32_t lastHBTick;

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
float calc_HVBus_voltage(int adc_value);
void dwt_init(void);
void heartbeat_init(void);
void heartbeat(void);


// =======================  Public Function Implementations =======================================
void app_init(void)
{
    // Initialization code, e.g., set up GPIOs or variables
//	tm1637_init();
//	tm1637_unset_all();

	// Setup UART and Hearbeat
  printf("\r\n=== UART online ===\r\n");
  heartbeat_init();

// ==================  DEBUG BENCHMARKING ONLY =========================================
//	dwt_init();
// ==================  DEBUG BENCHMARKING ONLY =========================================
}



// =============== app_loop() Usage Examples ======================
// Test segments:  	tm1637_loopAllSegs() <-- to see which segment is digit 0
// Example 2 - Encode and Display with blanks
//	uint8_t buff[] = {0,0,0};
//	uint8_t nums[] = {1,2,3};
//	uint8_t dots[] = {0,0,0};
//
//	encoded_buf_from_int_buf(nums, dots, buff, sizeof(buff) / sizeof(buff[0]) ); // builds encoded buffer of bytes
//	tm1637_fill_with_blanks(buff, sizeof(buff) / sizeof(buff[0]), 0);

void app_loop(void)
{

	HAL_ADC_Start(&hadc1);                               // ← start *each* pass
	if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
		uint16_t adc_count = HAL_ADC_GetValue(&hadc1);

		//	float busvoltage = calc_HVBus_voltage(adc_count);
		//	tm1637_displayDecimal(busvoltage, 0);

		float busvoltage = (float)adc_count * ADC_COUNT_TO_VOLT;

		printf("ADC = %u -> %.3f V\r\n", adc_count, busvoltage); // should come through UART
	}

	heartbeat();
	HAL_Delay(1000);


//	updateMaskFromInt(val);
//	updateLeds();

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



// Convert an ADC integer value to the HVBUS input voltage
// Using compile time constants for the full inversion of the gain path
float calc_HVBus_voltage(int adc_count) {
	return adc_count * ADC_COUNT_TO_HVBUS_VOLT_FULL; // meas at 16 cycles
}


// Retarget Printf, requires <stdio.h>
int _write(int file, char *ptr, int len)
{
    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
        return len;                 /* tell newlib it worked */
    }
    return -1;                      /* any other fd: error   */
}


// Debug and SWV
void dwt_init(void) {
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	// Reset cycle counter
	DWT->CYCCNT = 0;
	// Enable cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

}

// Not working, see SWV_INIT
void itm_enable(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   // global trace enable
    ITM->TCR  = 0x1000 | ITM_TCR_ITMENA_Msk;          // SWOENA + ITMENA
    ITM->TER  = 1;                                    // enable stimulus port 0
}

// FORCE Override since itm_enable wasn't working
/* Call this very early in main() */
// Clock is 84MHz, Baud is 2MHz
// Call example
	//HAL_Init();
	//SystemClock_Config();          // sets SystemCoreClock
	//SWV_Init(SystemCoreClock, 2000000);   // 2 MHz SWO
void SWV_Init(uint32_t core_clk_hz, uint32_t swo_baud)
{
    /* 1. Turn the SWO pin driver on (F4-only register) */
    DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN;        // enable trace pins
    DBGMCU->CR &= ~DBGMCU_CR_TRACE_MODE;       // 00: asynchronous (SWV)

    /* 2. Enable global trace in the core */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* 3. Set the SWO speed prescaler */
    /*   TPIU baud = core_clk / (ACPR+1)  */
    TPI->ACPR = (core_clk_hz / swo_baud) - 1;  // e.g. 2 MHz

    /* 4. Choose NRZ/async protocol on SWO */
    TPI->SPPR = 2;                             // 2 = NRZ

    /* 5. Optional: disable formatter for clean ASCII stream */
    TPI->FFCR = 0x0100;

    /* 6. Unlock and switch the ITM on */
    ITM->LAR = 0xC5ACCE55;                     // unlock write-protected regs
    ITM->TCR = ITM_TCR_ITMENA_Msk              // ITM master enable
             | ITM_TCR_SYNCENA_Msk             // enable sync packets
             | (1 << 16);                      // TraceBusID = 1
    ITM->TER = 1;                              // stimulus port 0
}



void heartbeat_init(void) {
	printf("\n\n..[heartbeat] started..\n\n");
	lastHBTick = HAL_GetTick();
}

void heartbeat(void) {
	if ((HAL_GetTick() - lastHBTick) >= 5000 ) {
		printf("\n\n..[heartbeat] alive..\n\n");
		lastHBTick = HAL_GetTick();
	}
}


