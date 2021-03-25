/*********************************************************************
This is a library for Monochrome OLEDs based on SSD1308 controller,
derived from adafruit SSD1306 driver.
  See at ------> http://www.adafruit.com/category/63_98
*********************************************************************/
#ifndef _GROVE_SSD1308H_
#define _GROVE_SSD1308H_

#include "Arduino.h"
#include "Print.h"

#define WIRE_WRITE Wire.write

#include "Adafruit_GFX.h"

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define SSD1308_I2C_ADDRESS   0x3D
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)

/*=========================================================================
    SSD1308 Controller 128x64 pixel display
  =========================================================================*/

#define SSD1308_LCDWIDTH                              128
#define SSD1308_LCDHEIGHT                             64

#define SSD1308_SETLOWCOLUMN                          0x00
#define SSD1308_SETHIGHCOLUMN                         0x10
#define SSD1308_MEMORYMODE                            0x20
#define SSD1308_COLUMNADDR                            0x21
#define SSD1308_PAGEADDR                              0x22
#define SSD1308_SETSTARTLINE                          0x40
#define SSD1308_SETCONTRAST                           0x81
#define SSD1308_SEGREMAP                              0xA0
#define SSD1308_DISPLAYALLON_RESUME                   0xA4
#define SSD1308_DISPLAYALLON                          0xA5
#define SSD1308_NORMALDISPLAY                         0xA6
#define SSD1308_INVERTDISPLAY                         0xA7
#define SSD1308_SETMULTIPLEX                          0xA8
#define SSD1308_SETEXTINT                             0xAD 
#define SSD1308_DISPLAYOFF                            0xAE
#define SSD1308_DISPLAYON                             0xAF
#define SSD1308_COMSCANINC                            0xC0
#define SSD1308_COMSCANDEC                            0xC8
#define SSD1308_SETDISPLAYOFFSET                      0xD3
#define SSD1308_SETDISPLAYCLOCKDIV                    0xD5
#define SSD1308_SETPRECHARGE                          0xD9
#define SSD1308_SETCOMPINS                            0xDA
#define SSD1308_SETVCOMDETECT                         0xDB
#define SSD1308_NOP                                   0xE3
// Scrolling definition
#define SSD1308_RIGHT_HORIZONTAL_SCROLL               0x26
#define SSD1308_LEFT_HORIZONTAL_SCROLL                0x27
#define SSD1308_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL  0x29
#define SSD1308_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL   0x2A
#define SSD1308_DEACTIVATE_SCROLL                     0x2E
#define SSD1308_ACTIVATE_SCROLL                       0x2F
#define SSD1308_SET_VERTICAL_SCROLL_AREA              0xA3

class Grove_SSD1308 : public Adafruit_GFX 
 { public:
      Grove_SSD1308();

      void begin(uint8_t i2caddr); // = SSD1308_I2C_ADDRESS);
      void writeCommand(uint8_t c);
      void writeData(uint8_t c);

      void clearDisplay(void);
      void invertDisplay(uint8_t i);
      void display(void);

      void startScrollRight(uint8_t start, uint8_t stop);
      void startScrollLeft(uint8_t start, uint8_t stop);

      void startScrollDiagRight(uint8_t start, uint8_t stop);
      void startScrollDiagLeft(uint8_t start, uint8_t stop);
      void stopScroll(void);

      void dim(boolean dim);

      void drawPixel(int16_t x, int16_t y, uint16_t color);

      virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
      virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

   private:
      int8_t _i2caddr;
      inline void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
      inline void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));
 };

#endif
