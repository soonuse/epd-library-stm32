# GDEM0213E26LT 2.13" e-paper library for STM32
GDEM0213E26LT 2.13" e-paper library for STM32
## Development environment
  * Keil v5
  * STM32CubeMX
  * Library: HAL (hardware abstraction layers)
## Hardware connection (EPD => STM32F103R8)
  * VCC    ->    3.3
  * GND    ->    GND
  * DIN    ->    PA7
  * CLK    ->    PA5
  * CS     ->    PA4
  * D/C    ->    PC7
  * RST    ->    PA9
  * BUSY   ->    PA8
## How to use
1.  open the .ioc file with STM32CubeMX.
2.  set the toolchain/IDE (MDK-ARM V5 is recommended).
3.  generate source code based on user settings.
4.  open the project in the IDE.
5.  add the directories BSP/Fonts to the project.
6.  build the project and download it to your STM32 chip.
    * this project is created on STM32F103ZE but you can migrate it to your own chip, see the .ioc file.
    * for other chips, you may have to edit the epdif.h and change this line 
      #include "stm32f1xx_hal.h" according to the target chips.
  

