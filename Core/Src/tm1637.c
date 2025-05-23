/*
 * tm1637.c
 *
 *  Created on: May 23, 2025
 *      Author: s
 */


#include "tm1637.h"

// TODO/later setup a hardware timer to pass to delay_us instead of using HAL_Delay
//static void delay_us(uint32_t us)
//{
//    uint32_t start = __HAL_TIM_GET_COUNTER(&htimX);  // Use a configured 1MHz timer
//    while ((__HAL_TIM_GET_COUNTER(&htimX) - start) < us);
//}

// Delay will actually be half a bit width
static void tm1637_delay(void)
{
    HAL_Delay(5);  // You can refine this with `delay_us(5);` later
}

void tm1637_init(void)
{
    TM_DIO_OUTPUT();
    // Idle HIGH, no pullups needed, handled by switching GPIO state
    HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TM_DIO_GPIO_Port, TM_DIO_Pin, GPIO_PIN_SET);
}


// 2 - Start/Stop Conditions

// Start condition: with CLK = HIGH, DIO HIGH->LOW signals START
static void tm1637_start(void)
{
    TM_DIO_OUTPUT();
    HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TM_DIO_GPIO_Port, TM_DIO_Pin, GPIO_PIN_SET);
    tm1637_delay();
    HAL_GPIO_WritePin(TM_DIO_GPIO_Port, TM_DIO_Pin, GPIO_PIN_RESET);
    tm1637_delay();
}

// Stop condition: With CLK = HIGH, DIO LOW->HIGH (After ACK/NACK_)
static void tm1637_stop(void)
{
    TM_DIO_OUTPUT();
    HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TM_DIO_GPIO_Port, TM_DIO_Pin, GPIO_PIN_RESET);
    tm1637_delay();
    HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);
    tm1637_delay();
    HAL_GPIO_WritePin(TM_DIO_GPIO_Port, TM_DIO_Pin, GPIO_PIN_SET);
    tm1637_delay();
}


// 3 - Send Byte with ACK Handling
static void tm1637_write_byte(uint8_t b)
{
    TM_DIO_OUTPUT();

    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TM_DIO_GPIO_Port, TM_DIO_Pin, (b & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        tm1637_delay();
        HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);
        tm1637_delay();
        b >>= 1;
    }

    // Wait for ACK
    // Assume: Master handles clock while releasing DIO line for Secondary to respond
    HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_RESET);
    TM_DIO_INPUT();
    // Datasheet says delay 5us after falling edge of clock (vs 2us typ.)
    tm1637_delay(); tm1637_delay();
    HAL_GPIO_WritePin(TM_CLK_GPIO_Port, TM_CLK_Pin, GPIO_PIN_SET);
    tm1637_delay();
    TM_DIO_OUTPUT();
}


// ================== Public Functions ===============================

// 4 digit set all function
// LSB first
// Address is 0x00 to 0x05, 8-bits wide
// Segments are Columns (Left=SEG1->Right=SEG8), Grids/Digits are rows (top=GRID1->bottom=GRID6)
void tm1637_set_all(void){

// First Frame is Data Command Set b[7:6]=01, b[5:0]=xxx000 (write) or b[5:0]=xxx100 (write w/ auto increment address)
tm1637_start();
tm1637_write_byte(0x40); // Data Command Set, Write, Auto Increment 0b0100 0000
tm1637_write_byte(0xc0); // Write Address From 0x0; 0b1100 0000
// Data - Depends on Common Cathode or Common Anode
for (int i = 0; i < 6; i++) {
	tm1637_write_byte(0xff);
}
tm1637_stop();


tm1637_delay();
tm1637_delay();

// Flush/Display Control
tm1637_start();
tm1637_write_byte(0x8f); //Display Control +  max brightness from manual 1000 1111
tm1637_stop();

}
