/**
 *  @filename   :   epd2in13b.c
 *  @brief      :   Implements for Dual-color e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 22 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "epd2in13b.h"
#include "epdif.h"

int EPD_Init(EPD* epd) {
  epd->reset_pin = RST_PIN;
  epd->dc_pin = DC_PIN;
  epd->cs_pin = CS_PIN;
  epd->busy_pin = BUSY_PIN;
  epd->width = EPD_WIDTH;
  epd->height = EPD_HEIGHT;
  
  /* this calls the peripheral hardware interface, see epdif */
  if (EpdInitCallback() != 0) {
    return -1;
  }
  
  /* EPD hardware init start */
  EPD_Reset(epd);
  EPD_SendCommand(epd, BOOSTER_SOFT_START);
  EPD_SendData(epd, 0x17);
  EPD_SendData(epd, 0x17);
  EPD_SendData(epd, 0x17);
  EPD_SendCommand(epd, POWER_ON);
  EPD_WaitUntilIdle(epd);
  EPD_SendCommand(epd, PANEL_SETTING);
  EPD_SendData(epd, 0x8F);
  EPD_SendCommand(epd, VCOM_AND_DATA_INTERVAL_SETTING);
  EPD_SendData(epd, 0x37);
  EPD_SendCommand(epd, RESOLUTION_SETTING);
  EPD_SendData(epd, 0x68);     // width: 104
  EPD_SendData(epd, 0x00);
  EPD_SendData(epd, 0xD4);     // height: 212
  /* EPD hardware init end */
  return 0;
}

void EPD_DigitalWrite(EPD* epd, int pin, int value) {
  EpdDigitalWriteCallback(pin, value);
}

int EPD_DigitalRead(EPD* epd, int pin) {
  return EpdDigitalReadCallback(pin);
}

void EPD_DelayMs(EPD* epd, unsigned int delaytime) {  // 1ms
  EpdDelayMsCallback(delaytime);
}

void EPD_SendCommand(EPD* epd, unsigned char command) {
  EPD_DigitalWrite(epd, epd->dc_pin, LOW);
  EpdSpiTransferCallback(command);
}

void EPD_SendData(EPD* epd, unsigned char data) {
  EPD_DigitalWrite(epd, epd->dc_pin, HIGH);
  EpdSpiTransferCallback(data);
}

void EPD_WaitUntilIdle(EPD* epd) {
  while(EPD_DigitalRead(epd, epd->busy_pin) == 0) {      //0: busy, 1: idle
    EPD_DelayMs(epd, 100);
  }      
}

void EPD_Reset(EPD* epd) {
  EPD_DigitalWrite(epd, epd->reset_pin, LOW);            //module reset    
  EPD_DelayMs(epd, 200);
  EPD_DigitalWrite(epd, epd->reset_pin, HIGH);
  EPD_DelayMs(epd, 200);    
}

void EPD_DisplayFrame(EPD* epd, const unsigned char* frame_black, const unsigned char* frame_red) {
  if (frame_black != NULL) {
    EPD_SendCommand(epd, DATA_START_TRANSMISSION_1);           
    EPD_DelayMs(epd, 2);
    for(int i = 0; i < epd->width / 8 * epd->height; i++) {
      EPD_SendData(epd, frame_black[i]);
    }  
    EPD_DelayMs(epd, 2);
  }
  if (frame_red!= NULL) {
    EPD_SendCommand(epd, DATA_START_TRANSMISSION_2);
    EPD_DelayMs(epd, 2);
    for(int i = 0; i < epd->width / 8 * epd->height; i++) {
      EPD_SendData(epd, frame_red[i]);  
    }  
    EPD_DelayMs(epd, 2);  
  }
  EPD_SendCommand(epd, DISPLAY_REFRESH); 
  EPD_WaitUntilIdle(epd);
}

/* After this command is transmitted, the chip would enter the deep-sleep mode to save power. 
   The deep sleep mode would return to standby by hardware reset. The only one parameter is a 
   check code, the command would be executed if check code = 0xA5. 
   You can use EPD_Reset() to awaken and EPD_Init() to initialize */
void EPD_Sleep(EPD* epd) {
  EPD_SendCommand(epd, DEEP_SLEEP);
  EPD_SendData(epd, 0xa5);
}


/* END OF FILE */


