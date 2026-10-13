/*
 * stm32f030xx_spi_drivers.c
 *
 *  Created on: Mar 3, 2026
 *      Author: ssmia
 */

#include "hal_driver_utils.h"
#include "stm32f030xx_spi_drivers.h"


/**
 * @fn      -SPI_PeriClockControl
 * @brief   Enables or disables clock for SPI Peripheral
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
void SPI_PeriClockControl(SPI_TypeDef *pSPIx, uint8_t EnorDi){
	if (EnorDi == ENABLE){
		if (pSPIx == SPI1){
			RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
		}
		if (pSPIx == SPI2){
			RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
		}
	}
	if (EnorDi == DISABLE){
		if (pSPIx == SPI1){
			RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
		}
		if (pSPIx == SPI2){
			RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
		}
	}
}

/**
 * @fn      -SPI_Init
 * @brief   Initialises SPI as a new variable
 *
 * @param[1]   SPIHandle      structure storing configuration for SPI peripheral
 *
 *
 * @return  None
 *
 * @note
 *
 *
 */

void SPI_Init(SPI_Handle_t *SPIHandle) {
	SPI_PeriClockControl(SPIHandle->pSPIx, ENABLE);

	/*
	 * TO DO: create macros for each bitfield in the CR1 register so your code doesn't
	 * have magic numbers.
	 */

	//1. configure mode
	SPIHandle->pSPIx->CR1 |= (SPIHandle->SPIPinConfig.SPI_DeviceMode) << 2;

	//2. configure bus config
	if (SPIHandle->SPIPinConfig.SPI_BusConfig == SPI_BUS_CFG_FD){
		SPIHandle->pSPIx->CR1 &= ~(1 << 15); //BIDIMODE cleared
		SPIHandle->pSPIx->CR1 &= ~(1 << 10); //RX Mode cleared
	}
	if (SPIHandle->SPIPinConfig.SPI_BusConfig == SPI_BUS_CFG_HD){
		SPIHandle->pSPIx->CR1 |= (1 << 15); //BIDIMODE set
	}
	if (SPIHandle->SPIPinConfig.SPI_BusConfig == SPI_BUS_CFG_TX_ONLY){
		SPIHandle->pSPIx->CR1 |= (1 << 15); //BIDIMODE set (enabled half duplex)
		SPIHandle->pSPIx->CR1 |= (1 << 14); //BIDIOE set (transmit only)
	}

	if (SPIHandle->SPIPinConfig.SPI_BusConfig == SPI_BUS_CFG_S_RXONLY){
		SPIHandle->pSPIx->CR1 &= ~(1 << 15); //BIDIMODE cleared
		SPIHandle->pSPIx->CR1 |= (1 << 10);
	}

	// 3. configure clock speed
    SPIHandle->pSPIx->CR1 &= ~(7 << 3); // clear bits 5-3
    SPIHandle->pSPIx->CR1 |= ((SPIHandle->SPIPinConfig.SPI_SCLKSpeed) << 3);

	// 4. configure DFF
    SPIHandle->pSPIx->CR1 &= ~(1 << 11); //default
    SPIHandle->pSPIx->CR1 |= (SPIHandle->SPIPinConfig.SPI_DFF) << 11;

    // 5. configure LSB or MSB first
    SPIHandle->pSPIx->CR1 &= ~(1 << 7); //default
    SPIHandle->pSPIx->CR1 |= SPIHandle->SPIPinConfig.SPI_FirstBit;

	// 6. configure CPOL
    if(SPIHandle->SPIPinConfig.SPI_CPOL == SPI_CPOL_0){
    	SPIHandle->pSPIx->CR1 &= ~(1 << 1);
    }
    if(SPIHandle->SPIPinConfig.SPI_CPOL == SPI_CPOL_1){
    	SPIHandle->pSPIx->CR1 |= (1 << 1);
    }

    // 7. configure CPHA
    SPIHandle->pSPIx->CR1 &= ~(1);
    SPIHandle->pSPIx->CR1 |= SPIHandle->SPIPinConfig.SPI_CPHA;
}

/**
 * @fn      -SPI_Send
 * @brief   Writes to Tx FIFO via CR1 data register
 *
 * @param[1]   pSPIx      structure that maps to hardware of SPI peripheral
 * @param[2]   *pTxBuffer pointer to array storing data to be written
 * @param[3]   Len        length of array in bytes
 *
 * @return  None
 *
 * @note    -This is a blocking call
 *
 *
 */
void SPI_Send(SPI_TypeDef *pSPIx, uint8_t *TxBufferP, uint32_t Len){
	while(Len > 0)/*ensure entire buffer is written and no more*/{
		/*
		 * is the Tx buffer empty?
		 * think of it as a tank with a pipe transferring the water. you only
		 * fill the tank up again when its empty
		 */
		while (!( ((pSPIx->SR) >> 1) & 1 )); //nothing executed till flag is 1

		// if flag is set, frame is 16 bits
		if( (pSPIx->CR1) & (1<<11) ){
			pSPIx->DR = *((uint16_t*)TxBufferP);
			TxBufferP+=2; // increment by 2 bytes
			Len-=2; // decrement by 2 bytes
		}else{
			/*
			 * DR is sensitive to the number of bits written to it so we must ensure data
			 * written to it is 8 bits only
			 * This is done via casting DR as 8 bits (since by default DR is 16 bits)
			 * to prevent compiler from issuing a 16 bit register operation
			 */
			*((volatile uint8_t *)&pSPIx->DR) = *(TxBufferP);
			TxBufferP++; //go to next byte of data
			Len--;
		}
	while (pSPIx->SR & (1 << 7));//nothing executed till BSY is cleared
	/*
	 * Write function only terminates once everything written has been clocked
	 * out
	 */
 }
}

/**
 * @fn      -SPI_Receive
 * @brief   Reads from Rx FIFO via CR1 data register
 *
 * @param[1]   pSPIx      structure that maps to hardware of SPI peripheral
 * @param[2]   *pRxBuffer pointer to array storing data to be read
 * @param[3]   Len        length of array in bytes
 *
 * @return  None
 *
 * @note    -This is a blocking call
 *
 *
 */

void SPI_Receive(SPI_TypeDef *pSPIx, uint8_t *RxBufferP, uint32_t Len){
	while (Len > 0){
	if ( !(pSPIx->SR & 1) ); // is Rx buffer full yet? (0 = empty, 1 = not empty)
	uint8_t DataLength = ( ((pSPIx->CR1) >> 11) & 1); // testing CRCL bit
	if (DataLength == SPI_DFF_8BITS){
		//8 bytes
		*(RxBufferP) = pSPIx->DR; // reading from data register
		RxBufferP++;// increment buffer address
		Len--;
	}else{
		// 16 bytes
		*((uint16_t*)RxBufferP) = pSPIx->DR; // reading from data register
		RxBufferP+=2; // increment by 2 bytes
		Len-=2; // decrement by 2 bytes
	}
	while (pSPIx->SR & (1 << 7)); // nothing executed till BSY is cleared
  }
}

/**
 * @fn      -SPI_SendIT
 * @brief   SPI send driver with interrupt functionality
 *
 * @param[1]   SPIHandle     structure that maps to hardware of SPI peripheral
 * @param[2]   TxBufferP     pointer to buffer written to data register
 * @param[2]   Len           length of buffer in bytes
 *
 * @return  None
 *
 * @note
 *
 *
 */

uint8_t SPI_SendIT(SPI_Handle_t *SPIHandle, uint8_t *TxBufferP, uint32_t Len){
	uint8_t State = SPIHandle->TxState;
	if (State != SPI_BUSY_TX)/*set up skipped if SPI is busy (relates to step2)*/{

	// 1. save Tx buffer address and len info in global variables.
	//variables are global so they do not reset. they should be dynamically updated
	SPIHandle->TxBufferP = TxBufferP; // TxBuffer now equal to whatever has been passed in
	SPIHandle->Txlen = Len;

	// 2. Mark SPI state as busy in transmission so that no other code can take over the peripheral until transmission is over
	SPIHandle->TxState = SPI_BUSY_TX;

	//3. enable TXEIE control bit to get an interrupt whenever TXE flag is set
	SPIHandle->pSPIx->CR2 |= (1 << 7);

	//4. data transmission will be handled by ISR code

  }
	return State;
}

/**
 * @fn      -SPI_ReceiveIT
 * @brief   tbw
 *
 * @param[1]   pSPIx      structure that maps to hardware of SPI peripheral
 * @param[2]   EnorDi     ENABLE or DISABLE
 *
 * @return  None
 *
 * @note
 *
 *
 */
uint8_t SPI_ReceiveIT(SPI_Handle_t *SPIHandle, uint8_t *RxBufferP, uint32_t Len){
	uint8_t State = SPIHandle->RxState;
	if (State != SPI_BUSY_RX){
	SPIHandle->RxBufferP = RxBufferP; // RxBuffer now equal to whatever has been passed in
	SPIHandle->Rxlen = Len;
	SPIHandle->RxState = SPI_BUSY_RX;
	SPIHandle->pSPIx->CR2 |= (1 << 6); // RXNEIE enabled
 }
	return State;
}

/**
 * @fn      -SPI_SSIConfig
 * @brief   Sets SSI when SSM is enabled
 *
 * @param[1]   pSPIx      structure that maps to hardware of SPI peripheral
 * @param[2]   EnorDI     Enable or disable macro
 *
 * @return  None
 *
 * @note    -This is a blocking call
 *
 *
 */

void SPI_SSIConfig(SPI_TypeDef *pSPIx, uint8_t EnorDi){
	if (EnorDi == ENABLE) {
		pSPIx->CR1 |= (3 << 8); // SSM and SSI enabled
		// Enable the peripheral (SPE bit)
		pSPIx->CR1 |= (1 << 6);
		/*
		 * Enabling the SPE bit must be done after SSM and SSI is configured
		 * otherwise the peripheral will think NSS is low, disabling master mode
		 */
	}else{
		pSPIx->CR1 &= ~(1 << 8);
	}
}
/**
 * @fn      -SPI_SSOEConfig
 * @brief   Sets SSOE when HSM is enabled
 *
 * @param[1]   pSPIx      structure that maps to hardware of SPI peripheral
 * @param[2]   EnorDI     Enable or disable macro
 *
 * @return  None
 *
 * @note
 *
 *
 */
void SPI_SSOEConfig(SPI_TypeDef *pSPIx, uint8_t EnorDi){
	if (EnorDi == ENABLE) {
		pSPIx->CR2 |= (1 << 2); // SSOE Enable
		pSPIx->CR1 |= (1 << 6);// SPE Bit enabled
	}else{
		pSPIx->CR1 &= ~(1 << 2); // SSOE Disable
	}
}

/**
 * @fn      -SPI_PeripheralControl
 * @brief   Enables/disables SPI peripheral
 *
 * @param[1]   pSPIx      structure that maps to hardware of SPI peripheral
 * @param[2]   EnorDi     ENABLE or DISABLE
 *
 * @return  None
 *
 * @note
 *
 *
 */

void SPI_PeripheralControl(SPI_TypeDef *SPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE){
		SPIx->CR1 |= (1 << 6);
	}else{
		SPIx->CR1 &= ~(1 << 6);
	}
}



/**
 * @fn      -GPIO_IRQConfig
 * @brief   Configures how interrupt of SPI peripheral is managed
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


void SPI_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi){
	/*NVIC (PROCESSOR) SIDE OF INTERRUPT HANDLING*/

	if (EnorDi == ENABLE) {
		/*NVIC enabled to process interrupt corresponding to this IRQ number*/
		*(NVIC_ISER) |= (1 << IRQNumber);

		/*configuring priority*/

		uint8_t RegIndex = IRQNumber / 4; //which of the 8 registers
	    uint8_t Section_Offset = IRQNumber % 4; //which byte in the register
	    uint8_t Shift = (Section_Offset * 8) + LSB_IMPLEMENTED; //only bits 7:6 are used
	    IRQPriority &= 3; //prevents unimplemented priority numbers slipping through
	    NVIC_IPR[RegIndex] |= (IRQPriority << Shift); //implements priority for that IRQ Number

	}
	if (EnorDi == DISABLE) {
		*(NVIC_ICER) |= (1 << IRQNumber);
	}
}

