#ifndef TFT_H
#define TFT_H

#include <stdint.h>

// center of world coordinates (on he screen)
/*
#define  LCD_WIDTH    240
#define  LCD_HEIGHT   320
*/
#define  LCD_WIDTH    132
#define  LCD_HEIGHT   162

#define  X0       LCD_WIDTH/2
#define  Y0       LCD_HEIGHT/2
#define  WHALF    LCD_WIDTH/2
#define  HHALF    LCD_HEIGHT/2

void tft_init(void);

void clear_screen(unsigned long color);
void LCD_square(int x, int y, int w, int h, unsigned long color);
void LCD_setpixel(int x, int y, unsigned long color);
void LCD_setpixels(int x, int y, int num_pixels_x, int num_pixels_y, unsigned long* pixels);
unsigned char read_register(unsigned char adr, unsigned char xParameter);

#endif
