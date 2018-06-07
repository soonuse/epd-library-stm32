/**
 *  @filename   :   epd1in54c.c
 *  @brief      :   Implements for Dual-color e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     June 1 2018
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
#include "epd1in54c.h"
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
  EPD_SendData(epd, 0x07);
  EPD_SendData(epd, 0x00);
  EPD_SendData(epd, 0x08);
  EPD_SendData(epd, 0x00);
  EPD_SendCommand(epd, BOOSTER_SOFT_START);
  EPD_SendData(epd, 0x17);
  EPD_SendData(epd, 0x17);
  EPD_SendData(epd, 0x17);
  EPD_SendCommand(epd, POWER_ON);
  EPD_WaitUntilIdle(epd);

  EPD_SendCommand(epd, PANEL_SETTING);
  EPD_SendData(epd, 0x0f);
	EPD_SendData(epd, 0x0d);
  EPD_SendCommand(epd, VCOM_AND_DATA_INTERVAL_SETTING);
  EPD_SendData(epd, 0xf7);
  EPD_SendCommand(epd, PLL_CONTROL);
  EPD_SendData(epd, 0x39);
  EPD_SendCommand(epd, TCON_RESOLUTION);
  EPD_SendData(epd, 0x98);
  EPD_SendData(epd, 0x00);
  EPD_SendData(epd, 0x98);
  EPD_SendCommand(epd, VCM_DC_SETTING_REGISTER);
  EPD_SendData(epd, 0x0E);

  EPD_SetLutBw(epd);
  EPD_SetLutRed(epd);
  /* EPD hardware init end */

  return 0;
}

/**
 *  @brief: this calls the corresponding function from epdif.h
 *          usually there is no need to change this function
 */
void EPD_DigitalWrite(EPD* epd, int pin, int value) {
  EpdDigitalWriteCallback(pin, value);
}

/**
 *  @brief: this calls the corresponding function from epdif.h
 *          usually there is no need to change this function
 */
int EPD_DigitalRead(EPD* epd, int pin) {
  return EpdDigitalReadCallback(pin);
}

/**
 *  @brief: this calls the corresponding function from epdif.h
 *          usually there is no need to change this function
 */
void EPD_DelayMs(EPD* epd, unsigned int delaytime) {  // 1ms
  EpdDelayMsCallback(delaytime);
}

/**
 *  @brief: basic function for sending commands
 */
void EPD_SendCommand(EPD* epd, unsigned char command) {
  EPD_DigitalWrite(epd, epd->dc_pin, LOW);
  EpdSpiTransferCallback(command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_SendData(EPD* epd, unsigned char data) {
  EPD_DigitalWrite(epd, epd->dc_pin, HIGH);
  EpdSpiTransferCallback(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void EPD_WaitUntilIdle(EPD* epd) {
  while(EPD_DigitalRead(epd, epd->busy_pin) == 0) {      //0: busy, 1: idle
    EPD_DelayMs(epd, 100);
  }      
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see EPD::Sleep();
 */
void EPD_Reset(EPD* epd) {
  EPD_DigitalWrite(epd, epd->reset_pin, LOW);                //module reset    
  EPD_DelayMs(epd, 200);
  EPD_DigitalWrite(epd, epd->reset_pin, HIGH);
  EPD_DelayMs(epd, 200);    
}

/**
 *  @brief: set the look-up tables
 */
void EPD_SetLutBw(EPD* epd) {
  unsigned int count;     
  EPD_SendCommand(epd, 0x20);         //g vcom
  for(count = 0; count < 15; count++) {
    EPD_SendData(epd, lut_vcom0[count]);
  } 
  EPD_SendCommand(epd, 0x21);        //g ww --
  for(count = 0; count < 15; count++) {
    EPD_SendData(epd, lut_w[count]);
  } 
  EPD_SendCommand(epd, 0x22);         //g bw r
  for(count = 0; count < 15; count++) {
    EPD_SendData(epd, lut_b[count]);
  } 
  EPD_SendCommand(epd, 0x23);         //g wb w
  for(count = 0; count < 15; count++) {
    EPD_SendData(epd, lut_g1[count]);
  } 
  EPD_SendCommand(epd, 0x24);         //g bb b
  for(count = 0; count < 15; count++) {
    EPD_SendData(epd, lut_g2[count]);
  } 
}

void EPD_SetLutRed(EPD* epd) {
  unsigned int count;     
  EPD_SendCommand(epd, 0x25);
  for(count = 0; count < 15; count++) {
    EPD_SendData(epd, lut_vcom1[count]);
  } 
  EPD_SendCommand(epd, 0x26);
  for(count = 0; count < 15; count++) {
    EPD_SendData(epd, lut_red0[count]);
  } 
  EPD_SendCommand(epd, 0x27);
  for(count = 0; count < 15; count++) {
    EPD_SendData(epd, lut_red1[count]);
  } 
}

void EPD_DisplayFrame(EPD* epd, const unsigned char* frame_buffer_black, const unsigned char* frame_buffer_red) {
  if (frame_buffer_black != NULL) {
    EPD_SendCommand(epd, DATA_START_TRANSMISSION_1);
    EPD_DelayMs(epd, 2);
    for (int i = 0; i < epd->width * epd->height / 8; i++) {
      EPD_SendData(epd, frame_buffer_black[i]);
    }
    EPD_DelayMs(epd, 2);
  }
  if (frame_buffer_red != NULL) {
    EPD_SendCommand(epd, DATA_START_TRANSMISSION_2);
    EPD_DelayMs(epd, 2);
    for (int i = 0; i < epd->width * epd->height / 8; i++) {
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
  EPD_SendCommand(epd, VCOM_AND_DATA_INTERVAL_SETTING);
  EPD_SendData(epd, 0x17);
  EPD_SendCommand(epd, VCM_DC_SETTING_REGISTER);         //to solve Vcom drop
  EPD_SendData(epd, 0x00);
  EPD_SendCommand(epd, POWER_SETTING);         //power setting
  EPD_SendData(epd, 0x02);        //gate switch to external
  EPD_SendData(epd, 0x00);
  EPD_SendData(epd, 0x00);
  EPD_SendData(epd, 0x00);
  EPD_WaitUntilIdle(epd);
  EPD_SendCommand(epd, POWER_OFF);         //power off
}

const unsigned char lut_vcom0[] =
{
  0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A,
  0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00
};

const unsigned char lut_w[] =
{
  0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A,
  0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04
};

const unsigned char lut_b[] = 
{
  0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A,
  0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04
};

const unsigned char lut_g1[] = 
{
  0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A,
  0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04
};

const unsigned char lut_g2[] = 
{
  0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A,
  0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04
};

const unsigned char lut_vcom1[] = 
{
  0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char lut_red0[] = 
{
  0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char lut_red1[] = 
{
  0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/* END OF FILE */


