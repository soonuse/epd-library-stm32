# STM32 libraries for Waveshare e-paper series
STM32 libraries for Waveshare e-paper series 1.54"/1.54" B/2.13"/2.13" B/2.7"/2.7" B/2.9"/2.9" B/4.2"/4.2 B/7.5"/ 7.5" B
## Development environment
  * Keil v5
  * STM32CubeMX
  * Library: HAL (hardware abstraction layers)
## Hardware connection (EPD => STM32F103ZE)
for 1.54" B/2.13" B/2.7"/2.7" B/2.9" B/4.2"/4.2 B/7.5"/ 7.5" B
  * VCC    ->    3.3
  * GND    ->    GND
  * DIN    ->    PA7
  * CLK    ->    PA5
  * CS     ->    PA4
  * D/C    ->    PA2
  * RST    ->    PA1
  * BUSY   ->    PA3
## Hardware connection (EPD => STM32F103R8)
for 1.54"/2.13"/2.9"
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
## Supported models
1.54"/1.54" B/2.13"/2.13" B/2.7"/2.7" B/2.9"/2.9" B/4.2"/4.2 B/7.5"/ 7.5" B

![gdep015oc1_1.54inch_e-paper](http://www.waveshare.com/img/devkit/general/e-Paper-Modules-CMP.jpg)
