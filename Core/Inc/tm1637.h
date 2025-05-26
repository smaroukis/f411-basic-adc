/*
 * tm1637.h
 *
 *  Created on: May 23, 2025
 *      Author: s
 */

// Digit 0 is right most
// Colon is part of Digit 1 ("g")

#ifndef TM1637_H
#define TM1637_H

#include "stm32f4xx_hal.h"

// ==== Pin Definitions ====
// see main.h for cubeMx generated preprocessor defines

// ==== Hardware Setup ====
#define TM1637_DIGIT_COUNT  4   // or 6 if you really have six digits


// ==== Direction Control Macros ====
// Set open drain - requires external pullups
#define TM_DIO_INPUT()  do { GPIO_InitTypeDef GPIO_InitStruct = {0}; \
                             GPIO_InitStruct.Pin = TM_SDA_Pin; \
                             GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
                             GPIO_InitStruct.Pull = GPIO_NOPULL; \
                             HAL_GPIO_Init(TM_SDA_GPIO_Port, &GPIO_InitStruct); } while(0)


#define TM_DIO_OUTPUT() do { GPIO_InitTypeDef GPIO_InitStruct = {0}; \
                             GPIO_InitStruct.Pin = TM_SDA_Pin; \
                             GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; \
                             GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; \
                             GPIO_InitStruct.Pull = GPIO_NOPULL; \
                             HAL_GPIO_Init(TM_SDA_GPIO_Port, &GPIO_InitStruct); } while(0)


void tm1637_delay(void);
void tm1637_init(void);
void tm1637_start(void);
void tm1637_stop(void);
void tm1637_write_byte(uint8_t b);
void tm1637_set_all(void);
void tm1637_unset_all(void);
void tm1637_loopDigNSegs(uint8_t n);
void tm1637_loopAllSegs(void);
void encoded_buf_from_int_buf(const uint8_t *digits, const uint8_t *use_dp, uint8_t *buf, uint8_t len);
void tm1637_write_packet(uint8_t *buff, size_t len, uint8_t start_addr);
void tm1637_fill_with_blanks(const uint8_t *data, uint8_t len, uint8_t start_pos);
void tm1637_displayNumber(uint16_t number, uint8_t pos);
void tm1637_displayError(void);

#endif
