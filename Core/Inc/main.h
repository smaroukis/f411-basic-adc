/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define CN10P13_Pin GPIO_PIN_6
#define CN10P13_GPIO_Port GPIOA
#define CN10P15_Pin GPIO_PIN_7
#define CN10P15_GPIO_Port GPIOA
#define CN10P25_Pin GPIO_PIN_10
#define CN10P25_GPIO_Port GPIOB
#define CN10P19_Pin GPIO_PIN_7
#define CN10P19_GPIO_Port GPIOC
#define CN10P1_Pin GPIO_PIN_9
#define CN10P1_GPIO_Port GPIOC
#define CN10P23_Pin GPIO_PIN_8
#define CN10P23_GPIO_Port GPIOA
#define CN10P21_Pin GPIO_PIN_9
#define CN10P21_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define CN10P17_Pin GPIO_PIN_6
#define CN10P17_GPIO_Port GPIOB
#define CN10P3_Pin GPIO_PIN_8
#define CN10P3_GPIO_Port GPIOB
#define CN10P5_Pin GPIO_PIN_9
#define CN10P5_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
// BAR0 is at the bottom of the bar graph

// TODO map bar positions to pins
#define BAR9_Pin CN10P1_Pin
#define BAR8_Pin CN10P3_Pin
#define BAR7_Pin CN10P5_Pin
#define BAR6_Pin CN10P13_Pin
#define BAR5_Pin CN10P15_Pin
#define BAR4_Pin CN10P17_Pin
#define BAR3_Pin CN10P19_Pin
#define BAR2_Pin CN10P21_Pin
#define BAR1_Pin CN10P23_Pin
#define BAR0_Pin CN10P25_Pin

#define BAR9_GPIO_Port CN10P1_GPIO_Port
#define BAR8_GPIO_Port CN10P3_GPIO_Port
#define BAR7_GPIO_Port CN10P5_GPIO_Port
#define BAR6_GPIO_Port CN10P13_GPIO_Port
#define BAR5_GPIO_Port CN10P15_GPIO_Port
#define BAR4_GPIO_Port CN10P17_GPIO_Port
#define BAR3_GPIO_Port CN10P19_GPIO_Port
#define BAR2_GPIO_Port CN10P21_GPIO_Port
#define BAR1_GPIO_Port CN10P23_GPIO_Port
#define BAR0_GPIO_Port CN10P25_GPIO_Port

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
