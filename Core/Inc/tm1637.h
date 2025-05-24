/*
 * tm1637.h
 *
 *  Created on: May 23, 2025
 *      Author: s
 */

#ifndef TM1637_H
#define TM1637_H

#include "stm32f4xx_hal.h"

// ==== Pin Definitions ====
// see main.h for cubeMx generated preprocessor defines

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


#endif
