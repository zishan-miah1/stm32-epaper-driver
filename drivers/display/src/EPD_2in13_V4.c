/*****************************************************************************
* | File      	:  	EPD_2in13_V4.c
* | Author      :   Waveshare team, Zishan Miah
* | Function    :   2.13inch e-paper V4
* | Info        :
*----------------
* |	This version:   V1.1
* | Date        :   03-03-2026
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_2in13_V4.h"
#include "stm32f030xx_gpio_drivers.h"
#include "stm32f030xx_spi_drivers.h"

static void EPD_GPIO_init(void){
	/*
	 * configure GPIO pins for SPI use
	 */
	GPIO_Handle_t SPIPins;
	SPIPins.pGPIOx = GPIOB; //all pins used are port B
	SPIPins.GPIOPinConfig.GPIO_PinMode = GPIO_MODE_ALTFUN;
	SPIPins.GPIOPinConfig.GPIO_PinAltFunMode = 0;
	SPIPins.GPIOPinConfig.GPIO_PinOPType = GPIO_OTYPE_PP;
	SPIPins.GPIOPinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	// MOSI pin
	SPIPins.GPIOPinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&SPIPins);

	// SCLK pin
	SPIPins.GPIOPinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&SPIPins);

	// Chip select via standard GPIO
	SPIPins.GPIOPinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	SPIPins.GPIOPinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	SPIPins.GPIOPinConfig.GPIO_PinOPType = GPIO_OTYPE_PP;
	SPIPins.GPIOPinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	/*
	 * the corresponding bit in the ODR register must be written high before the
	 * pin is configured to be in output mode, else it will snap to low, enabling
	 * a write to the screen when not required.
	 */
	GPIO_WriteToOutputPin(GPIOB, 12, SET);
	GPIO_Init(&SPIPins);

	//PB2 in input mode to read busy pin
	GPIO_Handle_t ReadBusyPin;
	ReadBusyPin.pGPIOx = GPIOB;
	ReadBusyPin.GPIOPinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
	ReadBusyPin.GPIOPinConfig.GPIO_PinMode = GPIO_MODE_IN;
	ReadBusyPin.GPIOPinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	ReadBusyPin.GPIOPinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	GPIO_Init(&ReadBusyPin);

	// PB14 in output mode to drive epaper D/C# pin
	GPIO_Handle_t DataCommandPin;
	DataCommandPin.pGPIOx = GPIOB;
	DataCommandPin.GPIOPinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	DataCommandPin.GPIOPinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	DataCommandPin.GPIOPinConfig.GPIO_PinOPType = GPIO_OTYPE_PP;
	DataCommandPin.GPIOPinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	DataCommandPin.GPIOPinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	GPIO_Init(&DataCommandPin);

	// PB1 in output mode to drive epaper reset pin
	GPIO_Handle_t ResetPin;
	ResetPin.pGPIOx = GPIOB;
	ResetPin.GPIOPinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	ResetPin.GPIOPinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ResetPin.GPIOPinConfig.GPIO_PinOPType = GPIO_OTYPE_PP;
	ResetPin.GPIOPinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;

	// before the system is ready, the default state is for reset to be held low
	GPIO_WriteToOutputPin(GPIOB, 1, RESET);
	GPIO_Init(&ResetPin);
}

static void EPD_SPI_init(void){
	SPI_Handle_t epaperSPI;
		epaperSPI.pSPIx = SPI2;
		epaperSPI.SPIPinConfig.SPI_BusConfig = SPI_BUS_CFG_TX_ONLY; // half duplex, write only
		epaperSPI.SPIPinConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
		epaperSPI.SPIPinConfig.SPI_SCLKSpeed = SPI_CLK_SPEED_DIV8; //2MHz
		epaperSPI.SPIPinConfig.SPI_DFF = SPI_DFF_8BITS;
		epaperSPI.SPIPinConfig.SPI_CPHA = SPI_CPHA_1ST;
		epaperSPI.SPIPinConfig.SPI_CPOL = SPI_CPOL_0;
		epaperSPI.SPIPinConfig.SPI_FirstBit = SPI_FB_MSB;
		epaperSPI.SPIPinConfig.SPI_SSM = SPI_SSM_EN; // SPI peripheral in SSM mode
		SPI_Init(&epaperSPI);

		/*
		 * enabling the SPI peripheral and setting SSI bit should come LAST
		 * to prevent mode fault
		 */
		SPI_SSIConfig(SPI2, ENABLE);
}

static void DEV_SPI_WriteByte(UBYTE reg){
	SPI_Send(SPI2, &reg, 1);
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_2in13_V4_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_2in13_V4_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_2in13_V4_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_2in13_V4_ReadBusy(void)
{
//    Debug("e-Paper busy\r\n");
	while(1)
	{	 //=1 BUSY
		if(DEV_Digital_Read(EPD_BUSY_PIN)==0)
			break;
		DEV_Delay_ms(10);
	}
	DEV_Delay_ms(10);
//    Debug("e-Paper busy release\r\n");
}

/******************************************************************************
function :	Setting the display window
parameter:
	Xstart : X-axis starting position
	Ystart : Y-axis starting position
	Xend : End position of X-axis
	Yend : End position of Y-axis
******************************************************************************/
static void EPD_2in13_V4_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EPD_2in13_V4_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EPD_2in13_V4_SendData((Xstart>>3) & 0xFF);
    EPD_2in13_V4_SendData((Xend>>3) & 0xFF);

    EPD_2in13_V4_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EPD_2in13_V4_SendData(Ystart & 0xFF);
    EPD_2in13_V4_SendData((Ystart >> 8) & 0xFF);
    EPD_2in13_V4_SendData(Yend & 0xFF);
    EPD_2in13_V4_SendData((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
	Xstart : X-axis starting position
	Ystart : Y-axis starting position
******************************************************************************/
static void EPD_2in13_V4_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EPD_2in13_V4_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EPD_2in13_V4_SendData(Xstart & 0xFF);

    EPD_2in13_V4_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EPD_2in13_V4_SendData(Ystart & 0xFF);
    EPD_2in13_V4_SendData((Ystart >> 8) & 0xFF);
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_2in13_V4_TurnOnDisplay(void)
{
	EPD_2in13_V4_SendCommand(0x22); // Display Update Control
	EPD_2in13_V4_SendData(0xf7);
	EPD_2in13_V4_SendCommand(0x20); // Activate Display Update Sequence
	EPD_2in13_V4_ReadBusy();
}

static void EPD_2in13_V4_TurnOnDisplay_Fast(void)
{
	EPD_2in13_V4_SendCommand(0x22); // Display Update Control
	EPD_2in13_V4_SendData(0xc7);	// fast:0x0c, quality:0x0f, 0xcf
	EPD_2in13_V4_SendCommand(0x20); // Activate Display Update Sequence
	EPD_2in13_V4_ReadBusy();
}

static void EPD_2in13_V4_TurnOnDisplay_Partial(void)
{
	EPD_2in13_V4_SendCommand(0x22); // Display Update Control
	EPD_2in13_V4_SendData(0xff);	// fast:0x0c, quality:0x0f, 0xcf
	EPD_2in13_V4_SendCommand(0x20); // Activate Display Update Sequence
	EPD_2in13_V4_ReadBusy();
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_2in13_V4_Init(void)
{
	//should i make initialisation for these pins happen before or after reset?
	EPD_GPIO_init();
	EPD_SPI_init();

	EPD_2in13_V4_Reset();

	EPD_2in13_V4_ReadBusy();
	EPD_2in13_V4_SendCommand(0x12);  //SWRESET
	EPD_2in13_V4_ReadBusy();

	EPD_2in13_V4_SendCommand(0x01); //Driver output control
	EPD_2in13_V4_SendData(0xF9);
	EPD_2in13_V4_SendData(0x00);
	EPD_2in13_V4_SendData(0x00);

	EPD_2in13_V4_SendCommand(0x11); //data entry mode
	EPD_2in13_V4_SendData(0x03);

	EPD_2in13_V4_SetWindows(0, 0, EPD_2in13_V4_WIDTH-1, EPD_2in13_V4_HEIGHT-1);
	EPD_2in13_V4_SetCursor(0, 0);

	EPD_2in13_V4_SendCommand(0x3C); //BorderWavefrom
	EPD_2in13_V4_SendData(0x05);

	EPD_2in13_V4_SendCommand(0x21); //  Display update control
	EPD_2in13_V4_SendData(0x00);
	EPD_2in13_V4_SendData(0x80);

	EPD_2in13_V4_SendCommand(0x18); //Read built-in temperature sensor
	EPD_2in13_V4_SendData(0x80);
	EPD_2in13_V4_ReadBusy();
}

void EPD_2in13_V4_Init_Fast(void)
{
	EPD_2in13_V4_Reset();

	EPD_2in13_V4_SendCommand(0x12);  //SWRESET
	EPD_2in13_V4_ReadBusy();

	EPD_2in13_V4_SendCommand(0x18); //Read built-in temperature sensor
	EPD_2in13_V4_SendData(0x80);

	EPD_2in13_V4_SendCommand(0x11); //data entry mode
	EPD_2in13_V4_SendData(0x03);

	EPD_2in13_V4_SetWindows(0, 0, EPD_2in13_V4_WIDTH-1, EPD_2in13_V4_HEIGHT-1);
	EPD_2in13_V4_SetCursor(0, 0);

	EPD_2in13_V4_SendCommand(0x22); // Load temperature value
	EPD_2in13_V4_SendData(0xB1);
	EPD_2in13_V4_SendCommand(0x20);
	EPD_2in13_V4_ReadBusy();

	EPD_2in13_V4_SendCommand(0x1A); // Write to temperature register
	EPD_2in13_V4_SendData(0x64);
	EPD_2in13_V4_SendData(0x00);

	EPD_2in13_V4_SendCommand(0x22); // Load temperature value
	EPD_2in13_V4_SendData(0x91);
	EPD_2in13_V4_SendCommand(0x20);
	EPD_2in13_V4_ReadBusy();
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_2in13_V4_Clear(void)
{
	UWORD Width, Height;
    Width = (EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1);
    Height = EPD_2in13_V4_HEIGHT;

    EPD_2in13_V4_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2in13_V4_SendData(0XFF);
        }
    }

	EPD_2in13_V4_TurnOnDisplay();
}

void EPD_2in13_V4_Clear_Black(void)
{
	UWORD Width, Height;
    Width = (EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1);
    Height = EPD_2in13_V4_HEIGHT;

    EPD_2in13_V4_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2in13_V4_SendData(0X00);
        }
    }

	EPD_2in13_V4_TurnOnDisplay();
}

/**************************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
Image : Image data
notes: loop operates on a byte-by-byte basis horizontally and pixel by pixel vertically 
***************************************************************************************/
void EPD_2in13_V4_Display(UBYTE *Image)
{
	UWORD Width, Height;
    Width = (EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1);
    Height = EPD_2in13_V4_HEIGHT;

    EPD_2in13_V4_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2in13_V4_SendData(Image[i + j * Width]);
        }
    }

	EPD_2in13_V4_TurnOnDisplay();
}

void EPD_2in13_V4_Display_Fast(UBYTE *Image)
{
	UWORD Width, Height;
    Width = (EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1);
    Height = EPD_2in13_V4_HEIGHT;

    EPD_2in13_V4_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2in13_V4_SendData(Image[i + j * Width]);
        }
    }

	EPD_2in13_V4_TurnOnDisplay_Fast();
}


/******************************************************************************
function :	Refresh a base image
parameter:
	Image : Image data
******************************************************************************/
void EPD_2in13_V4_Display_Base(UBYTE *Image)
{
	UWORD Width, Height;
    Width = (EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1);
    Height = EPD_2in13_V4_HEIGHT;

	EPD_2in13_V4_SendCommand(0x24);   //Write Black and White image to RAM
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
			EPD_2in13_V4_SendData(Image[i + j * Width]);
		}
	}
	EPD_2in13_V4_SendCommand(0x26);   //Write Black and White image to RAM
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
			EPD_2in13_V4_SendData(Image[i + j * Width]);
		}
	}
	EPD_2in13_V4_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and partial refresh
parameter:
	Image : Image data
******************************************************************************/
void EPD_2in13_V4_Display_Partial(UBYTE *Image)
{
	UWORD Width, Height;
    Width = (EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1);
    Height = EPD_2in13_V4_HEIGHT;

	//Reset
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(1);
    DEV_Digital_Write(EPD_RST_PIN, 1);

	EPD_2in13_V4_SendCommand(0x3C); // BorderWavefrom
	EPD_2in13_V4_SendData(0x80);

	EPD_2in13_V4_SendCommand(0x01); // Driver output control
	EPD_2in13_V4_SendData(0xF9);
	EPD_2in13_V4_SendData(0x00);
	EPD_2in13_V4_SendData(0x00);

	EPD_2in13_V4_SendCommand(0x11); // data entry mode
	EPD_2in13_V4_SendData(0x03);

	EPD_2in13_V4_SetWindows(0, 0, EPD_2in13_V4_WIDTH-1, EPD_2in13_V4_HEIGHT-1);
	EPD_2in13_V4_SetCursor(0, 0);

	EPD_2in13_V4_SendCommand(0x24);   // Write Black and White image to RAM
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
			EPD_2in13_V4_SendData(Image[i + j * Width]);
		}
	}
	EPD_2in13_V4_TurnOnDisplay_Partial();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_2in13_V4_Sleep(void)
{
	EPD_2in13_V4_SendCommand(0x10); //enter deep sleep
	EPD_2in13_V4_SendData(0x01);
	DEV_Delay_ms(100);
}
