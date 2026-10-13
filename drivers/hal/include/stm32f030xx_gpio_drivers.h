/*
 * stm32f030xx_gpio_drivers.h
 *
 *  Created on: Mar 3, 2026
 *      Author: ssmia
 */

#ifndef HAL_INCLUDE_STM32F030XX_GPIO_DRIVERS_H_
#define HAL_INCLUDE_STM32F030XX_GPIO_DRIVERS_H_

#include "stm32f030x8.h"


// GPIO Pin possible modes
#define GPIO_MODE_IN     0
#define GPIO_MODE_OUT    1
#define GPIO_MODE_ALTFUN 2
#define GPIO_MODE_ANALOG 3
#define GPIO_MODE_IT_FT  4
#define GPIO_MODE_IT_RT  5
#define GPIO_MODE_IT_RFT 6

// GPIO Pin possible Output configurations
#define GPIO_OTYPE_PP 0
#define GPIO_OTYPE_OD 1

// GPIO Pin Output speeds
#define GPIO_SPEED_LOW  0
#define GPIO_SPEED_MED  1
#define GPIO_SPEED_HIGH 2

// GPIO Pin resistor configurations
#define GPIO_NO_PUPD     0
#define GPIO_PIN_PU      1
#define GPIO_PIN_PD      2

typedef struct
{
	// TO DO: COMPLETE DOCUMENTATION FOR POSSIBLE VALUES LATER

	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;
	uint8_t GPIO_PinSpeed;
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;
}GPIO_PinConfig_t;

typedef struct
{
	GPIO_TypeDef *pGPIOx;
	GPIO_PinConfig_t GPIOPinConfig;
}GPIO_Handle_t;





                     /*API Prototypes*/

void GPIO_PeriClockControl(GPIO_TypeDef *pGPIOx, uint8_t EnorDi);
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_TypeDef *pGPIOx);
uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber, uint8_t Bool);
void GPIO_WriteToOutputPort(GPIO_TypeDef *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_ToggleOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber);


// Interrupt handling
void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi);
void GPIO_IRQHandling(uint8_t PinNumber); //from which pin has the IRQ been triggered


#endif /* HAL_INCLUDE_STM32F030XX_GPIO_DRIVERS_H_ */
