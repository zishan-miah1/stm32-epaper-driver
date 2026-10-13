/*
 * stm32f030xx_spi_drivers.h
 *
 *  Created on: Mar 3, 2026
 *      Author: ssmia
 */

#ifndef HAL_INCLUDE_STM32F030XX_SPI_DRIVERS_H_
#define HAL_INCLUDE_STM32F030XX_SPI_DRIVERS_H_

#include "stm32f030x8.h"

/****************************************CR1 Bitfields**********************************/


/*
 * SPI Device Modes
 */
#define SPI_DEVICE_MODE_MASTER 1
#define SPI_DEVICE_MODE_SLAVE  0

/*
 * SPI Communciation modes
 */
#define SPI_BUS_CFG_FD 0
#define SPI_BUS_CFG_HD 1
#define SPI_BUS_CFG_S_RXONLY 2
#define SPI_BUS_CFG_TX_ONLY 3

/*
 * SPI Clock Speeds
 */
#define SPI_CLK_SPEED_DIV2   0
#define SPI_CLK_SPEED_DIV4   1
#define SPI_CLK_SPEED_DIV8   2
#define SPI_CLK_SPEED_DIV16  3
#define SPI_CLK_SPEED_DIV32  4
#define SPI_CLK_SPEED_DIV64  5
#define SPI_CLK_SPEED_DIV128 6
#define SPI_CLK_SPEED_DIV256 7

/*
 * SPI DFF Options
 */
#define SPI_DFF_8BITS  0
#define SPI_DFF_16BITS 1

/*
 * SPI First Bit Options
 */
#define SPI_FB_MSB 0
#define SPI_FB_LSB 1


/*
 * SPI CPOL Options
 */
#define SPI_CPOL_0 0
#define SPI_CPOL_1 1

/*
 * SPI CPHA Options
 */
#define SPI_CPHA_1ST 0
#define SPI_CPHA_2ND 1

/*
 * SPI SSM ENABLE
 */
#define SPI_SSM_DI 0
#define SPI_SSM_EN 1

/*
 * SPI Status Register
 */
#define NOT_EMPTY 0
#define EMPTY     1

/*
 * SPI State options (for when driver with interrupt functionality is used)
 */
#define SPI_READY   0
#define SPI_BUSY_RX 1
#define SPI_BUSY_TX 2


/*
 * SSI Options
 */
#define SSI_EN ENABLE
#define SSI_DI DISABLE

/*
 * Configuration structure for SPI peripheral
 */
typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SCLKSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_FirstBit;
	uint8_t SPI_CPOL;
	uint8_t SPI_SSM;
	uint8_t SPI_CPHA;

}SPI_Config_t;

/*
 * Handle structure for SPI peripheral
 */
typedef struct
{
	SPI_TypeDef *pSPIx;
	SPI_Config_t SPIPinConfig;
	uint8_t *TxBufferP;
	uint8_t *RxBufferP;
	uint32_t Txlen;
	uint32_t Rxlen;
	uint8_t TxState;
	uint8_t RxState;


}SPI_Handle_t;





/*********************************** API Prototypes ****************************************/

void SPI_PeriClockControl(SPI_TypeDef *pSPIx, uint8_t EnorDi);
void SPI_Init(SPI_Handle_t *SPIHandle);
void SPI_DeInit(SPI_Handle_t *SPIHandle); //just pointer to the peripheral required
void SPI_Send(SPI_TypeDef *SPIx, uint8_t *TxBufferP, uint32_t Len /*in bytes*/);
void SPI_Receive(SPI_TypeDef *SPIx, uint8_t *RxBufferP, uint32_t Len);

/*
 * SPI Send and Receive with interrupt functionality
 */
uint8_t SPI_SendIT(SPI_Handle_t *SPIHandle, uint8_t *TxBufferP, uint32_t Len /*in bytes*/);
uint8_t SPI_ReceiveIT(SPI_Handle_t *SPIHandle, uint8_t *RxBufferP, uint32_t Len);

void SPI_SSIConfig(SPI_TypeDef *pSPIx, uint8_t EnorDi);
void SPI_SSOEConfig(SPI_TypeDef *pSPIx, uint8_t EnorDi);

void SPI_PeripheralControl(SPI_TypeDef *SPIx, uint8_t EnorDi);

// TO DO: Interrupt handling for SPI
void SPI_IRQConfig(uint8_t IRQNumber, uint8_t IRQPriority, uint8_t EnorDi);

#endif /* HAL_INCLUDE_STM32F030XX_SPI_DRIVERS_H_ */
