/*
 * stm32_nop_delay.h
 *
 *  Created on: May 24, 2025
 *      Author: spencer
 */

#ifndef INC_STM32_NOP_DELAY_H_
#define INC_STM32_NOP_DELAY_H_

#ifndef __NOP
  #define __NOP() __asm volatile ("nop")
#endif

static inline void delay_cycles(uint32_t cycles)
{
    while (cycles--) {
        __NOP();
    }
}

#endif /* INC_STM32_NOP_DELAY_H_ */
