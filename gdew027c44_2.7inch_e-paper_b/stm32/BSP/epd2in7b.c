/**
 *  @filename   :   epd2in7b.c
 *  @brief      :   Implements for Dual-color e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     July 13 2017
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
#include "epd2in7b.h"
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

  EPD_SendCommand(epd, POWER_SETTING);
  EPD_SendData(epd, 0x03);                  // VDS_EN, VDG_EN
  EPD_SendData(epd, 0x00);                  // VCOM_HV, VGHL_LV[1], VGHL_LV[0]
  EPD_SendData(epd, 0x2b);                  // VDH
  EPD_SendData(epd, 0x2b);                  // VDL
  EPD_SendData(epd, 0x09);                  // VDHR

  EPD_SendCommand(epd, BOOSTER_SOFT_START);
  EPD_SendData(epd, 0x07);
  EPD_SendData(epd, 0x07);
  EPD_SendData(epd, 0x17);

  // Power optimization
  EPD_SendCommand(epd, 0xF8);
  EPD_SendData(epd, 0x60);
  EPD_SendData(epd, 0xA5);

  // Power optimization
  EPD_SendCommand(epd, 0xF8);
  EPD_SendData(epd, 0x89);
  EPD_SendData(epd, 0xA5);

  // Power optimization
  EPD_SendCommand(epd, 0xF8);
  EPD_SendData(epd, 0x90);
  EPD_SendData(epd, 0x00);
  
  // Power optimization
  EPD_SendCommand(epd, 0xF8);
  EPD_SendData(epd, 0x93);
  EPD_SendData(epd, 0x2A);

  // Power optimization
  EPD_SendCommand(epd, 0xF8);
  EPD_SendData(epd, 0x73);
  EPD_SendData(epd, 0x41);
  
  EPD_SendCommand(epd, VCM_DC_SETTING_REGISTER);
  EPD_SendData(epd, 0x12);                   
  EPD_SendCommand(epd, VCOM_AND_DATA_INTERVAL_SETTING);
  EPD_SendData(epd, 0x87);        // define by OTP
  
  EPD_SetLut(epd);
  
  EPD_SendCommand(epd, PARTIAL_DISPLAY_REFRESH);
  EPD_SendData(epd, 0x00);  
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
  EPD_DigitalWrite(epd, epd->reset_pin, LOW);                //module reset    
  EPD_DelayMs(epd, 200);
  EPD_DigitalWrite(epd, epd->reset_pin, HIGH);
  EPD_DelayMs(epd, 200);    
}

void EPD_SetLut(EPD* epd) {
  unsigned int count;     
  EPD_SendCommand(epd, LUT_FOR_VCOM);                            //vcom
  for(count = 0; count < 44; count++) {
    EPD_SendData(epd, lut_vcom_dc[count]);
  }
  
  EPD_SendCommand(epd, LUT_WHITE_TO_WHITE);                      //ww --
  for(count = 0; count < 42; count++) {
    EPD_SendData(epd, lut_ww[count]);
  }   
  
  EPD_SendCommand(epd, LUT_BLACK_TO_WHITE);                      //bw r
  for(count = 0; count < 42; count++) {
    EPD_SendData(epd, lut_bw[count]);
  } 

  EPD_SendCommand(epd, LUT_WHITE_TO_BLACK);                      //wb w
  for(count = 0; count < 42; count++) {
    EPD_SendData(epd, lut_bb[count]);
  } 

  EPD_SendCommand(epd, LUT_BLACK_TO_BLACK);                      //bb b
  for(count = 0; count < 42; count++) {
    EPD_SendData(epd, lut_wb[count]);
  } 
}

/* this function won't be affected by the rotate parameter */
void EPD_DisplayPartial(EPD* epd, int x, int y, int w, int l, 
  const unsigned char* buffer_black, const unsigned char* buffer_red) {

  EPD_SendCommand(epd, POWER_ON);

  EPD_WaitUntilIdle(epd);

  EPD_SendCommand(epd, PANEL_SETTING);
  EPD_SendData(epd, 0xaf);        //KW-BF   KWR-AF    BWROTP 0f
  
  EPD_SendCommand(epd, PLL_CONTROL);
  EPD_SendData(epd, 0x3a);       //3A 100HZ   29 150Hz 39 200HZ    31 171HZ

  if (buffer_black != NULL) {
    EPD_SendCommand(epd, PARTIAL_DATA_START_TRANSMISSION_1);
    EPD_SendData(epd, x >> 8);
    EPD_SendData(epd, x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    EPD_SendData(epd, y >> 8);        
    EPD_SendData(epd, y & 0xff);
    EPD_SendData(epd, w >> 8);
    EPD_SendData(epd, w & 0xf8);     // w (width) should be the multiple of 8, the last 3 bit will always be ignored
    EPD_SendData(epd, l >> 8);        
    EPD_SendData(epd, l & 0xff);
    EPD_DelayMs(epd, 2);
    for(int i = 0; i < w  / 8 * l; i++) {
      EPD_SendData(epd, buffer_black[i]);  
    }  
    EPD_DelayMs(epd, 2);                  
  }
  if (buffer_red != NULL) {
    EPD_SendCommand(epd, PARTIAL_DATA_START_TRANSMISSION_2);
    EPD_SendData(epd, x >> 8);
    EPD_SendData(epd, x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    EPD_SendData(epd, y >> 8);        
    EPD_SendData(epd, y & 0xff);
    EPD_SendData(epd, w >> 8);
    EPD_SendData(epd, w & 0xf8);     // w (width) should be the multiple of 8, the last 3 bit will always be ignored
    EPD_SendData(epd, l >> 8);        
    EPD_SendData(epd, l & 0xff);
    EPD_DelayMs(epd, 2);
    for(int i = 0; i < w  / 8 * l; i++) {
      EPD_SendData(epd, buffer_red[i]);  
    }  
    EPD_DelayMs(epd, 2);                  
  }

  EPD_SendCommand(epd, PARTIAL_DISPLAY_REFRESH); 
  EPD_SendData(epd, x >> 8);
  EPD_SendData(epd, x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
  EPD_SendData(epd, y >> 8);        
  EPD_SendData(epd, y & 0xff);
  EPD_SendData(epd, w >> 8);
  EPD_SendData(epd, w & 0xf8);     // w (width) should be the multiple of 8, the last 3 bit will always be ignored
  EPD_SendData(epd, l >> 8);        
  EPD_SendData(epd, l & 0xff);

  EPD_WaitUntilIdle(epd);
}

void EPD_DisplayFrame(EPD* epd, const unsigned char* frame_buffer_black, const unsigned char* frame_buffer_red) {
  EPD_SendCommand(epd, POWER_ON);

  EPD_WaitUntilIdle(epd);

  EPD_SendCommand(epd, PANEL_SETTING);
  EPD_SendData(epd, 0xaf);        //KW-BF   KWR-AF    BWROTP 0f
  
  EPD_SendCommand(epd, PLL_CONTROL);
  EPD_SendData(epd, 0x3a);       //3A 100HZ   29 150Hz 39 200HZ    31 171HZ

  // To avoid display orientation effects
  // use EPD_WIDTH instead of epd->width 
  // use EPD_HEIGHT instead of epd->height
  EPD_SendCommand(epd, TCON_RESOLUTION);
  EPD_SendData(epd, EPD_WIDTH >> 8);
  EPD_SendData(epd, EPD_WIDTH & 0xff);        //176      
  EPD_SendData(epd, EPD_HEIGHT >> 8);        
  EPD_SendData(epd, EPD_HEIGHT & 0xff);       //264

  if (frame_buffer_black != NULL) {
    EPD_SendCommand(epd, DATA_START_TRANSMISSION_1);           
    EPD_DelayMs(epd, 2);
    for(int i = 0; i < epd->width * epd->height / 8; i++) {
      EPD_SendData(epd, frame_buffer_black[i]);  
    }  
    EPD_DelayMs(epd, 2);                  
  }
  if (frame_buffer_red != NULL) {
    EPD_SendCommand(epd, DATA_START_TRANSMISSION_2);
    EPD_DelayMs(epd, 2);
    for(int i = 0; i < epd->width * epd->height / 8; i++) {
      EPD_SendData(epd, frame_buffer_red[i]);  
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

const unsigned char lut_vcom_dc[] =
{
0x00, 0x00,
0x00, 0x1A, 0x1A, 0x00, 0x00, 0x01,     
0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,     
0x00, 0x0E, 0x01, 0x0E, 0x01, 0x10,     
0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,     
0x00, 0x04, 0x10, 0x00, 0x00, 0x05,     
0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,     
0x00, 0x23, 0x00, 0x00, 0x00, 0x01    
};

//R21H
const unsigned char lut_ww[] =
{
0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
0x40, 0x0A, 0x0A, 0x00, 0x00, 0x08,
0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
0x80, 0x0A, 0x0A, 0x00, 0x00, 0x08,
0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

//R22H    r
const unsigned char lut_bw[] =
{
0xA0, 0x1A, 0x1A, 0x00, 0x00, 0x01,
0x00, 0x0A, 0x0A, 0x00, 0x00, 0x08,
0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
0x90, 0x0A, 0x0A, 0x00, 0x00, 0x08,
0xB0, 0x04, 0x10, 0x00, 0x00, 0x05,
0xB0, 0x03, 0x0E, 0x00, 0x00, 0x0A,
0xC0, 0x23, 0x00, 0x00, 0x00, 0x01
};

//R23H    w
const unsigned char lut_bb[] =
{
0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
0x40, 0x0A, 0x0A, 0x00, 0x00, 0x08,
0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
0x80, 0x0A, 0x0A, 0x00, 0x00, 0x08,
0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

//R24H    b
const unsigned char lut_wb[] =
{
0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
0x20, 0x0A, 0x0A, 0x00, 0x00, 0x08,
0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10,
0x10, 0x0A, 0x0A, 0x00, 0x00, 0x08,
0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

/* END OF FILE */


