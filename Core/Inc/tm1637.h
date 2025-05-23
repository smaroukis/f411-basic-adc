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
#define TM_CLK_GPIO_Port GPIOC
#define TM_CLK_Pin       GPIO_PIN_1

#define TM_DIO_GPIO_Port GPIOC
#define TM_DIO_Pin       GPIO_PIN_0

// ==== Direction Control Macros ====
// Hack for no external pullups, set internal pullup during input mode (for ACKs)
#define TM_DIO_INPUT()  do { GPIO_InitTypeDef GPIO_InitStruct = {0}; \
                             GPIO_InitStruct.Pin = TM_DIO_Pin; \
                             GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
                             GPIO_InitStruct.Pull = GPIO_PULLUP; \
                             HAL_GPIO_Init(TM_DIO_GPIO_Port, &GPIO_InitStruct); } while(0)


#define TM_DIO_OUTPUT() do { GPIO_InitTypeDef GPIO_InitStruct = {0}; \
                             GPIO_InitStruct.Pin = TM_DIO_Pin; \
                             GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
                             GPIO_InitStruct.Pull = GPIO_NOPULL; \
                             HAL_GPIO_Init(TM_DIO_GPIO_Port, &GPIO_InitStruct); } while(0)

void tm1637_init(void);
void tm1637_display_decimal(int num, uint8_t show_colon);

#endif
