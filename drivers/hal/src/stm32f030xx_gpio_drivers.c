/*
 * stm32f030xx_gpio_drivers.c
 *
 *  Created on: Mar 3, 2026
 *      Author: ssmia
 */

#include "hal_driver_utils.h"
#include "stm32f030xx_gpio_drivers.h"


/*Takes in pointer to base address of a peripheral and a
 "enabled or disabled variable"*/
/**
 * @fn      -GPIO_PeriClockControl
 * @brief   Enables or disables clock for GPIO Peripheral
 *
 * @param[1]   pGPIOx      Pointer to base address of the GPIO port.
 * @param[2]   EnorDi      Enable or disable macros
 *
 *
 * @return  None
 *
 * @note
 *
 *
 */

void GPIO_PeriClockControl(GPIO_TypeDef *pGPIOx, uint8_t EnorDi){
	if (EnorDi == ENABLE) {
		if (pGPIOx == GPIOA){
			RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
		}else if (pGPIOx == GPIOB) {
			RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
		}else if (pGPIOx == GPIOC) {
			RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
		}else {
			RCC->AHBENR |= RCC_AHBENR_GPIODEN;
		}
	}
	if (EnorDi == DISABLE) {
		if (pGPIOx == GPIOA){
			RCC->AHBRSTR |= RCC_AHBRSTR_GPIOARST;
		}else if (pGPIOx == GPIOB) {
			RCC->AHBRSTR |= RCC_AHBRSTR_GPIOBRST;
		}else if (pGPIOx == GPIOC) {
			RCC->AHBRSTR |= RCC_AHBRSTR_GPIOCRST;
		}else {
			RCC->AHBRSTR |= RCC_AHBRSTR_GPIODRST;
		}

	}



};

/**
 * @fn      -GPIO_Init
 * @brief   Initialises pin as a new variable
 *
 * @param[1]   pGPIOHandle      structure handling GPIO pin initialisation
 *
 *
 * @return  None
 *
 * @note
 *
 *
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle) {
	GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);
	uint32_t temp = 0;
	// 1. configure mode
	if((pGPIOHandle->GPIOPinConfig.GPIO_PinMode) < GPIO_MODE_IT_FT) {
		temp = (pGPIOHandle->GPIOPinConfig.GPIO_PinMode) << (2 * pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
		pGPIOHandle->pGPIOx->MODER &= ~(3 << (2 * pGPIOHandle->GPIOPinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER |= temp;
		temp = 0;
    }else if ((pGPIOHandle->GPIOPinConfig.GPIO_PinMode) > GPIO_MODE_ANALOG) {
    	/*PERIPHERAL SIDE OF INTERRUPT HANDLING*/

    	if ((pGPIOHandle->GPIOPinConfig.GPIO_PinMode) == GPIO_MODE_IT_FT){
		// clear corresponding bit in RTSR
		EXTI->RTSR &= ~(1 << pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
		// set corresponding bit in FTSR
		EXTI->FTSR |= (1 << pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);

    	}else if ((pGPIOHandle->GPIOPinConfig.GPIO_PinMode) == GPIO_MODE_IT_RT){
		// set corresponding bit in FTSR
		EXTI->FTSR &= ~(1 << pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
		// set corresponding bit in RTSR
		EXTI->RTSR |= (1 << pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);

    	}else if ((pGPIOHandle->GPIOPinConfig.GPIO_PinMode) == GPIO_MODE_IT_RFT) {
		EXTI->FTSR |= (1 << pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
		EXTI->RTSR |= (1 << pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
	}
	/*configure GPIO port selection for EXTI line in SYSCFG_EXTICR*/
	RCC->APB2ENR |= 1; // enable SYSCFG Peripheral
	uint8_t Portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
	uint8_t RegIndex = (pGPIOHandle->GPIOPinConfig.GPIO_PinNumber) / 4;
	uint8_t Shift = ((pGPIOHandle->GPIOPinConfig.GPIO_PinNumber) % 4) * 4;
	SYSCFG->EXTICR[RegIndex] &= ~(0xF /*1111*/ << Shift); // clear
	SYSCFG->EXTICR[RegIndex] |= Portcode << Shift;

	// enable EXTI interrupt delivery via IMR
	EXTI->IMR |= (1 << pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
    }

	// 2. configure speed
	temp = pGPIOHandle->GPIOPinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(3 << (2 * pGPIOHandle->GPIOPinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;
    temp = 0;

	// 3. configure pupd settings
    temp = pGPIOHandle->GPIOPinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->PUPDR &= ~(3 << (2 * pGPIOHandle->GPIOPinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->PUPDR |= temp;
    temp = 0;

	// 4. configure output type
    temp = pGPIOHandle->GPIOPinConfig.GPIO_PinOPType << (pGPIOHandle->GPIOPinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->OTYPER &= ~(3 << (pGPIOHandle->GPIOPinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->OTYPER |= temp;
    temp = 0;

	// 5. configure alt functionality
    if((pGPIOHandle->GPIOPinConfig.GPIO_PinMode) == GPIO_MODE_ALTFUN) {
    	/*configure alternate function registers*/

    	// this decides whether shift and manipulation occurs in AFR[0] or AFR[1]
    	// e.g. 9/8 = 1 -> AFR[1] is modified, 4/8 = 0 -> AFR[0] is modified
    	uint8_t RegIndex = (pGPIOHandle->GPIOPinConfig.GPIO_PinNumber) / 8;

    	/*
    	 * if PinNumber < 8, remainder = PinNumber. multiply by 4 for correct bitfields
    	 * e.g. 5%8 = 5, 5*4=20
    	 * when PinNumber>8 remainder = correct Pin of top half
    	 * e.g. 12%8 = 4. 4 greater than 8 is 12.
    	 */
    	uint8_t Shift = (pGPIOHandle->GPIOPinConfig.GPIO_PinNumber % 8) * 4;

    	if (RegIndex < 1) {
    		pGPIOHandle->pGPIOx->AFR[0] &= ~(3 << Shift);
    		pGPIOHandle->pGPIOx->AFR[0] |= (pGPIOHandle->GPIOPinConfig.GPIO_PinAltFunMode << Shift);
    	}else{
    		pGPIOHandle->pGPIOx->AFR[1] &= ~(3 << Shift);
    		pGPIOHandle->pGPIOx->AFR[1] |= (pGPIOHandle->GPIOPinConfig.GPIO_PinAltFunMode << Shift);
    	}


    }



};

/**
 * @fn      -GPIO_DeInit
 * @brief   Enables or disables clock for GPIO Peripheral
 *
 * @param[1]   pGPIOx      Pointer to base address of the GPIO port.
 *
 *
 * @return  None
 *
 * @note
 *
 *
 */
void GPIO_DeInit(GPIO_TypeDef *pGPIOx){
	if (pGPIOx == GPIOA){
		     RCC->AHBRSTR |=  RCC_AHBRSTR_GPIOARST; //set GPIOA disable bit to 1
		     RCC->AHBRSTR &= ~RCC_AHBRSTR_GPIOARST; //set to 0 again to exit reset state
			}else if (pGPIOx == GPIOB) {
				RCC->AHBRSTR |=  RCC_AHBRSTR_GPIOBRST;
			    RCC->AHBRSTR &= ~RCC_AHBRSTR_GPIOBRST;
			}else if (pGPIOx == GPIOC) {
				RCC->AHBRSTR |=  RCC_AHBRSTR_GPIOCRST;
			    RCC->AHBRSTR &= ~RCC_AHBRSTR_GPIOCRST;
			}else {
				RCC->AHBRSTR |=  RCC_AHBRSTR_GPIODRST;
				RCC->AHBRSTR &= ~RCC_AHBRSTR_GPIODRST;
			}



};



/**
 * @fn      -GPIO_ReadFromInputPin
 * @brief   Reads value from pin
 *
 * @param[1]   pGPIOx      Pointer to base address of the GPIO port.
 * @param[2]   PinNumber   Pin number to read from
 *
 *
 * @return  0 or 1
 *
 * @note
 *
 *
 */

uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber){
	uint8_t bit = (uint8_t) ((pGPIOx->IDR) >> PinNumber) & 1;
	return bit;
};

/**
 * @fn      -GPIO_ReadFromInputPort
 * @brief   Reads value from entire GPIO peripheral
 *
 * @param[1]   pGPIOx      Pointer to base address of the GPIO port.
 *
 *
 * @return  value from port
 *
 * @note
 *
 *
 */
uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef *pGPIOx){
	uint16_t value = (uint16_t) (pGPIOx->IDR);
	return value;
};

/**
 * @fn      -GPIO_WriteToOutputPin
 * @brief   Writes to pin
 *
 * @param[1]   pGPIOx      Pointer to base address of the GPIO port.
 * @param[2]   PinNumber   Pin number to write to
 * @param[3]   Bool        Value should either be 1 or 0
 *
 * @return  None
 *
 * @note
 *
 *
 */
void GPIO_WriteToOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber, uint8_t Bool){
	if (Bool == GPIO_PIN_SET){
		pGPIOx->ODR |= (1 << PinNumber);
	}else {
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
};

/**
 * @fn      -GPIO_WriteToOutputPort
 * @brief   Writes value to ODR of entire GPIO port
 *
 * @param[1]   pGPIOx      Pointer to base address of the GPIO port.
 * @param[2]   PinNumber   Pin number to write to
 * @param[3]   Value       Value being written to ODR
 *
 * @return  None
 *
 * @note
 *
 *
 */
void GPIO_WriteToOutputPort(GPIO_TypeDef *pGPIOx, uint8_t PinNumber, uint8_t Value){
	pGPIOx->ODR = Value;
};

/**
 * @fn      -GPIO_ToggleOutputPin
 * @brief   Toggles a pin
 *
 * @param[1]   pGPIOx      Pointer to base address of the GPIO port.
 * @param[2]   PinNumber   Pin to toggle
 *
 * @return  None
 *
 * @note
 *
 *
 */
void GPIO_ToggleOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber){
	pGPIOx->ODR ^= (1 << PinNumber);
};


/**
 * @fn      -GPIO_IRQConfig
 * @brief   Configures how interrupt of a peripheral is managed
 *
 * @param[1]   IRQNumber   Interrupt service request number
 * @param[2]   IRQPriority
 * @param[3]   EnorDi      Enable or disable macros
 *
 * @return  None
 *
 * @note
 *
 *
 */


void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi){
	// NVIC (PROCESSOR) SIDE OF INTERRUPT HANDLING

	if (EnorDi == ENABLE) {
		// NVIC enabled to process interrupt corresponding to this IRQ number
		*(NVIC_ISER) |= (1 << IRQNumber);

		// configuring priority
		uint8_t RegIndex = IRQNumber / 4; //which of the 8 registers
	    uint8_t Section_Offset = IRQNumber % 4; //which byte in the register
	    uint8_t Shift = (Section_Offset * 8) + LSB_IMPLEMENTED; //only bits 7:6 are used
	    IRQPriority &= 3; // prevents unimplemented priority numbers slipping through
	    NVIC_IPR[RegIndex] |= (IRQPriority << Shift); // implements priority for that IRQ Number

	}
	if (EnorDi == DISABLE) {
		*(NVIC_ICER) |= (1 << IRQNumber);
	}
};

/**
 * @fn      -GPIO_IRQHandling
 * @brief   Handles the IRQ
 *
 * @param[1]   PinNumber   Indicates from which pin the IRQ has been issued
 *
 * @return  None
 *
 * @note
 *
 *
 */
void GPIO_IRQHandling(uint8_t PinNumber){
	// clear EXTI PR register corresponding to pin number
	if (EXTI->PR & (1 << PinNumber)) /*testing bit corresponding to pin*/ {
		// clear that bit (external event has been handled)
		EXTI->PR |= (1 << PinNumber);
	}


};

void crude_delay_ms(uint32_t ms)
{
    for (uint32_t m = 0; m < ms; m++) {
        for (volatile uint32_t i = 0; i < 2000; i++) {
            __asm__("nop");
        }
    }
}
