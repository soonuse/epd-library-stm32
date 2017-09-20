/**
 *  @filename   :   epd4in2.c
 *  @brief      :   Implements for e-paper library
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
#include "epd4in2.h"
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
  EPD_SendData(epd, 0xff);                  // VDHR
  EPD_SendCommand(epd, BOOSTER_SOFT_START);
  EPD_SendData(epd, 0x17);
  EPD_SendData(epd, 0x17);
  EPD_SendData(epd, 0x17);                  //07 0f 17 1f 27 2F 37 2f
  EPD_SendCommand(epd, POWER_ON);
  EPD_WaitUntilIdle(epd);
  EPD_SendCommand(epd, PANEL_SETTING);
  EPD_SendData(epd, 0xbf);    // KW-BF   KWR-AF  BWROTP 0f
  EPD_SendData(epd, 0x0b);
  EPD_SendCommand(epd, PLL_CONTROL);
  EPD_SendData(epd, 0x3c);        // 3A 100HZ   29 150Hz 39 200HZ  31 171HZ
  /* EPD hardware init end */
  return 0;}

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
 *  @brief: set the look-up table
 */
void EPD_SetLut(EPD* epd) {
  unsigned int count;     
  EPD_SendCommand(epd, LUT_FOR_VCOM);                            //vcom
  for(count = 0; count < 44; count++) {
    EPD_SendData(epd, lut_vcom0[count]);
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

void EPD_DisplayFrame(EPD* epd, const unsigned char* frame_buffer) {
  EPD_SendCommand(epd, RESOLUTION_SETTING);
  EPD_SendData(epd, EPD_WIDTH >> 8);        
  EPD_SendData(epd, EPD_WIDTH & 0xff);
  EPD_SendData(epd, EPD_HEIGHT >> 8);
  EPD_SendData(epd, EPD_HEIGHT & 0xff);

  EPD_SendCommand(epd, VCM_DC_SETTING);
  EPD_SendData(epd, 0x12);                   

  EPD_SendCommand(epd, VCOM_AND_DATA_INTERVAL_SETTING);
  EPD_SendCommand(epd, 0x97);    //VBDF 17|D7 VBDW 97  VBDB 57  VBDF F7  VBDW 77  VBDB 37  VBDR B7

  if (frame_buffer != NULL) {
    EPD_SendCommand(epd, DATA_START_TRANSMISSION_1);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
      EPD_SendData(epd, 0xFF);      // bit set: white, bit reset: black
    }
    EPD_DelayMs(epd, 2);
    EPD_SendCommand(epd, DATA_START_TRANSMISSION_2); 
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
      EPD_SendData(epd, frame_buffer[i]);
    }  
    EPD_DelayMs(epd, 2);                  
  }

  EPD_SetLut(epd);

  EPD_SendCommand(epd, DISPLAY_REFRESH); 
  EPD_DelayMs(epd, 100);
  EPD_WaitUntilIdle(epd);
}

/* After this command is transmitted, the chip would enter the deep-sleep mode to save power. 
   The deep sleep mode would return to standby by hardware reset. The only one parameter is a 
   check code, the command would be executed if check code = 0xA5. 
   You can use EPD_Reset() to awaken and EPD_Init() to initialize */
void EPD_Sleep(EPD* epd) {
  EPD_SendCommand(epd, VCOM_AND_DATA_INTERVAL_SETTING);
  EPD_SendData(epd, 0x17);                       //border floating    
  EPD_SendCommand(epd, VCM_DC_SETTING);          //VCOM to 0V
  EPD_SendCommand(epd, PANEL_SETTING);
  EPD_DelayMs(epd, 100);          

  EPD_SendCommand(epd, POWER_SETTING);           //VG&VS to 0V fast
  EPD_SendData(epd, 0x00);        
  EPD_SendData(epd, 0x00);        
  EPD_SendData(epd, 0x00);              
  EPD_SendData(epd, 0x00);        
  EPD_SendData(epd, 0x00);
  EPD_DelayMs(epd, 100);          
                
  EPD_SendCommand(epd, POWER_OFF);          //power off
  EPD_WaitUntilIdle(epd);
  EPD_SendCommand(epd, DEEP_SLEEP);         //deep sleep
  EPD_SendData(epd, 0xA5);
}

const unsigned char lut_vcom0[] =
{
0x00, 0x17, 0x00, 0x00, 0x00, 0x02,        
0x00, 0x17, 0x17, 0x00, 0x00, 0x02,        
0x00, 0x0A, 0x01, 0x00, 0x00, 0x01,        
0x00, 0x0E, 0x0E, 0x00, 0x00, 0x02,        
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};
const unsigned char lut_ww[] ={
0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};
const unsigned char lut_bw[] ={
0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      
};

const unsigned char lut_bb[] ={
0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             
};

const unsigned char lut_wb[] ={
0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            
};



/* END OF FILE */





