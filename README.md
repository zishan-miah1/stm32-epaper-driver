# 2.13" E-Paper Display Driver & Graphics Library
In this repo you will find:
- Bare-metal driver enabling an STM32 Nucleo-64 development board to communicate with a Waveshare 2.13" E-Paper display to render Graphics
- Hardware agnostic graphics libary 
- Display specific adapter 

## Demo 

## Prerequisites to get the project running 
### Hardware 
- This [STM Nucleo development board](https://www.st.com/en/evaluation-tools/nucleo-f030r8.html)
- This [Waveshare E-Paper display](https://www.amazon.co.uk/Waveshare-2-13inch-Resolution-Compatible-Raspberry/dp/B0BFQGN6X4?psc=1) 
- Jumper wires
- Breadboard 

### Software tools 
- STM32CubeIDE

### Setting Up Hardware
| E-Paper Pin | Function       | STM32 Connection |
| ----------- | -------------- | ---------------- |
| BUSY        | Busy Status    | PB2              |
| RST         | Reset          | PB1              |
| DC          | Data / Command | PB14             |
| CS          | Chip Select    | PB12             |
| CLK         | SPI Clock      | PB13             |
| DIN         | SPI MOSI       | PB15             |
| GND         | Ground         | GND              |
| VCC         | Power Supply   | 3.3V             |

The STM32 connections are not chosen out of necessity. Selection is arbitrary and the pins used can be changed so long as the software is changed accordingly they are able to fulfil the purpose you expect of them (check the reference manual for this information).

### Setting Up Environment 
1. Create a new STM32 project in STM32CubeIDE for the NUCLEO-F030R8 board.
2. Under the project root, copy the directory structure found in this repo (only 3 directories must be added by the user: `drivers` as a source folder, `fbgfx` as a source folder and `fonts` as a standard folder).
3. Copy the corresponding source files from this repository into their respective directories, preserving the mentioned directory structure.
4. Add the include paths listed below to the GCC compiler.

#### Workspace Include Paths 
Add the following directories under **Project Properties → C/C++ Build → Settings → MCU GCC Compiler → Include paths**:

- `${workspace_loc:/${ProjName}/fbgfx/include}`
- `${workspace_loc:/${ProjName}/drivers/display/include}`
- `${workspace_loc:/${ProjName}/drivers/hal/include}`
- `${workspace_loc:/${ProjName}/Fonts}`

#### CMSIS Header Include Paths 
Add the following directories to the compiler's include paths:

`<STM32Cube repository>/STM32Cube_FW_F0_V1.11.5/Drivers/CMSIS/Include`

`<STM32Cube repository>/Repository/STM32Cube_FW_F0_V1.11.5/Drivers/CMSIS/Device/ST/STM32F0xx/Include`

Replace `<STM32Cube repository>` with the path to your local STM32Cube repository. The default location on Windows is typically:
`C:/Users/<username>/STM32Cube/Repository`

 
## Examples
You're one step away from building the project! 

Create a C source file in the `Src/` directory. For example: 
```C
#include <stdint.h>
#include "EPD_2in13_V4_gfx.h"

int main(void)
{
	init();
	fillRectangle(30, 50, 50, 50); 
	refresh();
	
  /* Loop forever */
	for(;;);
}
```

Rendering text on the display:
```C
#include <stdint.h>
#include "EPD_2in13_V4_gfx.h"
#include "fonts.h"

int main(void){
	init();

	// a default font is used when size is set to 16 
	drawText(20, 100, 16, "Hello");
	drawText(20, 70, 16, "Hi there");
	refresh();

	for(;;);
}
```

Build the project then run it to see cool shapes appear! 