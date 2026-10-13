/*
 * hal_driver_utils.h
 *
 *  Created on: Mar 3, 2026
 *      Author: ssmia
 */

#ifndef HAL_INCLUDE_HAL_DRIVER_UTILS_H_
#define HAL_INCLUDE_HAL_DRIVER_UTILS_H_

#include "stm32f030x8.h"

/*Basic*/
#define ENABLE  1
#define DISABLE 0
#define SET     ENABLE
#define RESET   DISABLE
#define GPIO_PIN_SET SET
#define GPIO_PIN_RESET RESET

/*GPIO Pin numbers*/
#define GPIO_PIN_NO_0  0
#define GPIO_PIN_NO_1  1
#define GPIO_PIN_NO_2  2
#define GPIO_PIN_NO_3  3
#define GPIO_PIN_NO_4  4
#define GPIO_PIN_NO_5  5
#define GPIO_PIN_NO_6  6
#define GPIO_PIN_NO_7  7
#define GPIO_PIN_NO_8  8
#define GPIO_PIN_NO_9  9
#define GPIO_PIN_NO_10 10
#define GPIO_PIN_NO_11 11
#define GPIO_PIN_NO_12 12
#define GPIO_PIN_NO_13 13
#define GPIO_PIN_NO_14 14
#define GPIO_PIN_NO_15 15



/***********************************Processor Specific details****************************/
/*
 * ARM Cortex M0 specific NVIC register addresses
 */
#define NVIC_ISER   ((__IO uint32_t*) 0xE000E100) //Interrupt Set Enable register
#define NVIC_ICER   ((__IO uint32_t*) 0xE000E180) //Interrupt Clear Enable register
#define NVIC_IPR    ((__IO uint32_t*) 0xE000E400) //Interrupt Priority Registers

/*
 * Bits implemented in NVIC_IPR
 */
#define LSB_IMPLEMENTED 6

/*
 * Interrupt IRQ Numbers
 */
#define IRQ_NO_EXTI0_1  5
#define IRQ_NO_EXTI2_3  6
#define IRQ_NO_EXTI4_15 7
#define IRQ_NO_SPI1     25
#define IRQ_NO_SPI2     26


/*
 * Priorities implemented for NVIC
 */
#define NVIC_PRI_0 0
#define NVIC_PRI_1 1
#define NVIC_PRI_2 2
#define NVIC_PRI_3 3

/*
 * Macros that return a portcode for a given GPIO base address
 */

#define GPIO_BASEADDR_TO_CODE(x) ( (x == GPIOA) ? 0 :\
		                           (x == GPIOB) ? 1 :\
		                           (x == GPIOC) ? 2 :\
		                           (x == GPIOD) ? 3 : 0)



#endif /* HAL_INCLUDE_HAL_DRIVER_UTILS_H_ */
