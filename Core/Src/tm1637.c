/*
 * tm1637.c
 *
 *  Created on: May 23, 2025
 *      Author: s
 */


#include "tm1637.h"
#include "main.h" // for pin definitions

#define CMD_DATA_AUTO_INC 	0x40u 			// 01xx 00xx
#define CMD_DATA_NO_AUTO 	0x44u 			// 01xx 01xx
#define CMD_ADDR         	0xC0u     		/* 11xx aaaa */
#define TM1637_ADDR(a)      (CMD_ADDR | ((a) & 0x3Fu))

#define   CMD_DISPLAY      	0x80u     		/* 10xx pbbb */
#define   DISP_ON         	(1u << 3) 		/* p = 1 → ON */
#define   DISP_OFF        	(0u << 3)
#define   BRIGHT(n)       	((n) & 0x07u)


// Forward Declarations
void CLK_HIGH(void);
void CLK_LOW(void);
void DATA_SET(uint8_t b);
void DATA_HIGH(void);
void DATA_LOW(void);

// TODO/later setup a hardware timer to pass to delay_us instead of using HAL_Delay
//static void delay_us(uint32_t us)
//{
//    uint32_t start = __HAL_TIM_GET_COUNTER(&htimX);  // Use a configured 1MHz timer
//    while ((__HAL_TIM_GET_COUNTER(&htimX) - start) < us);
//}

// Delay will actually be half a bit width
void tm1637_delay(void)
{
    HAL_Delay(1);  // You can refine this with `delay_us(5);` later
    // 5us is 1/2 clock period, 10us is 100kHz
}

void tm1637_init(void)
{
    TM_DIO_OUTPUT();
    // Idle HIGH, no pullups needed, handled by switching GPIO state
    CLK_HIGH();
    DATA_HIGH();
}


// 2 - Start/Stop Conditions

// Start condition: with CLK = HIGH, DIO HIGH->LOW signals START
void tm1637_start(void)
{
    TM_DIO_OUTPUT();
    CLK_HIGH();
    DATA_HIGH();
    tm1637_delay();
    DATA_LOW();
    tm1637_delay();
}

// Stop condition: With CLK = HIGH, DIO LOW->HIGH (After ACK/NACK_)
void tm1637_stop(void)
{
    TM_DIO_OUTPUT();
    CLK_LOW();
    DATA_LOW();
    tm1637_delay();
    CLK_HIGH();
    tm1637_delay();
    DATA_HIGH();
    tm1637_delay();
}


// 3 - Send Byte with ACK Handling
// TODO change to static once done debugging
// Note we write LSb first so 0b1111 0000 looks like 0000_1111 in time
void tm1637_write_byte(uint8_t b)
{
    TM_DIO_OUTPUT();

    for (int i = 0; i < 8; i++) {
        CLK_LOW();
        DATA_SET(b & 0x01);
        tm1637_delay();
        CLK_HIGH();
        tm1637_delay();
        b >>= 1;
    }

    // Wait for ACK
    // Assume: Master handles clock while releasing DIO line for Secondary to respond
    CLK_LOW();
    //DATA_HIGH(); // release bus, see if we get an ACK
    TM_DIO_INPUT();
    // Datasheet says delay 5us after falling edge of clock (vs 2us typ.)
    tm1637_delay();
    // heres where we would read the ACK
    CLK_HIGH(); //  data valid during HIGH, so we can read
    tm1637_delay();

    tm1637_delay();
}

// Wrapper for CLK = 1
void CLK_HIGH(void) {
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);
}

void CLK_LOW(void) {
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_RESET);
}

void DATA_SET(uint8_t b) {
	HAL_GPIO_WritePin(TM_SDA_GPIO_Port, TM_SDA_Pin, b ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void DATA_HIGH(void) {
	HAL_GPIO_WritePin(TM_SDA_GPIO_Port, TM_SDA_Pin, GPIO_PIN_SET);
}

void DATA_LOW(void) {
	HAL_GPIO_WritePin(TM_SDA_GPIO_Port, TM_SDA_Pin, GPIO_PIN_RESET);
}

// ================== Public Functions ===============================

// 4 digit set all function
// LSB first
// Address is 0x00 to 0x05, 8-bits wide
// Segments are Columns (Left=SEG1->Right=SEG8), Grids/Digits are rows (top=GRID1->bottom=GRID6)
void tm1637_set_all(void){

	// Data Command
	tm1637_start();
	tm1637_write_byte(CMD_DATA_AUTO_INC);
	tm1637_stop();

	// Address Command
	tm1637_start();
	tm1637_write_byte(TM1637_ADDR(0)); // Write Address From 0x0; 0b1100 0000
	for (int i = 0; i < 4; i++) {
		tm1637_write_byte(0xff);
	}
	tm1637_stop();


	tm1637_delay();
	tm1637_delay();

	// Flush/Display Control
	tm1637_start();
	tm1637_write_byte(CMD_DISPLAY | DISP_ON | BRIGHT(7) ); //Display Control +  max brightness from manual 1000 1111
	tm1637_stop();

}

void tm1637_unset_all(void) {
	// Data Command
	tm1637_start();
	tm1637_write_byte(CMD_DATA_AUTO_INC);
	tm1637_stop();

	// Address Command
	tm1637_start();
	tm1637_write_byte(TM1637_ADDR(0x0)); // Write Address From 0x0; 0b1100 0000
	for (int i = 0; i < 4; i++) {
		tm1637_write_byte(0x00);
	}
	tm1637_stop();

	tm1637_delay();
	tm1637_delay();
}

