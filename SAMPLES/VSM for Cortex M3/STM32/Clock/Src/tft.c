//#include <msp430g2402.h>

#include "gfx.h"
#include"tft.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_spi.h"
#include "stm32f1xx_hal_gpio.h"
#include "cmsis_os.h"
//#include "stm32f101x6.h"

#define GPIO_CS         GPIOA
#define GPIO_DC         GPIOA  
#define PIN_CS          GPIO_PIN_0
#define PIN_DC          GPIO_PIN_15

#define SELECT_TFT      HAL_GPIO_WritePin(GPIO_CS,PIN_CS,GPIO_PIN_RESET);
#define DESELECT_TFT    HAL_GPIO_WritePin(GPIO_CS,PIN_CS,GPIO_PIN_SET);

#define SELECT_COMMAND  HAL_GPIO_WritePin(GPIO_DC,PIN_DC,GPIO_PIN_RESET);
#define SELECT_DATA     HAL_GPIO_WritePin(GPIO_DC,PIN_DC,GPIO_PIN_SET);
   
#define TFTCMD_NOOPERATION          0x00
#define TFTCMD_SW_RESET             0x01
#define TFTCMD_STATUS_READ          0x09
#define TFTCMD_WAKEUP               0x11
#define TFTCMD_MODE_PARTIAL         0x12
#define TFTCMD_MODE_NORMAL          0x13
#define TFTCMD_DISPAY_OFF           0x28
#define TFTCMD_DISPAY_ON            0x29
#define TFTCMD_COLUMN_SET           0x2A
#define TFTCMD_PAGE_SET             0x2B
#define TFTCMD_MEM_WRITE            0x2C
#define TFTCMD_PIXELFORMAT_SET      0x3A
#define TFTCMD_CTRL_WRITE           0x53
#define TFTCMD_CTRL_READ            0x54

#define MSB(w) ((w>>8)&0xFF)
#define LSB(w) (w&0xFF)

#define  X0       LCD_WIDTH/2
#define  Y0       LCD_HEIGHT/2

extern SPI_HandleTypeDef hspi1;

void wait_for_unlock_spi()
{
  while((hspi1.State&HAL_SPI_STATE_BUSY)==HAL_SPI_STATE_BUSY);
}

void transfer_locked(uint8_t dt)
{
  // must use HAL_SPI_TransmitReceive - wait for whole byte transmission
  uint8_t rx;
  while(HAL_SPI_TransmitReceive(&hspi1, &dt, &rx, 1, HAL_MAX_DELAY) == HAL_BUSY);
}

void transfer_dma(uint8_t* pdt, uint16_t dtsz)
{
  while(HAL_SPI_Transmit_DMA(&hspi1, pdt, dtsz)==HAL_BUSY);
}

#define send_command(dt)          SELECT_COMMAND; transfer_locked(dt);
#define send_data(dt)             SELECT_DATA; transfer_locked(dt);
#define receive_data(res, dt)     SELECT_DATA; res = transfer_locked(dt);

unsigned char packet[32];
unsigned char packet_sz;
#define clear_data_packet()     packet_sz=0;
#define data_push_back(ch)      packet[packet_sz] = ch; ++packet_sz;  
#define send_data_packet()      SELECT_DATA; transfer_dma(packet, packet_sz);

void LCD_setcol(unsigned short c_strt, unsigned short c_end)
{
  send_command(TFTCMD_COLUMN_SET);                                                      /* Column Command address  */
  send_data(MSB(c_strt));
  send_data(LSB(c_strt));
  send_data(MSB(c_end));
  send_data(LSB(c_end));
}

void LCD_setpage(unsigned short p_strt, unsigned short p_end)
{
  send_command(TFTCMD_PAGE_SET);                                                      /* Page Command address       */
  send_data(MSB(p_strt));
  send_data(LSB(p_strt));
  send_data(MSB(p_end));
  send_data(LSB(p_end));
}

void LCD_setpixel(int x, int y, unsigned long color)
{
//  LCD_setcol(x+X0, x+X0);
//  LCD_setpage(Y0-y, Y0-y);
  LCD_setcol(x, x);
  LCD_setpage(y, y);
  send_command(TFTCMD_MEM_WRITE);
  // convert from RGB888 to RGB565

  unsigned short c = 
      ((color>>8)&0xF800) |             // R
      ((color>>5)&0x07E0) |             // G
      ((color>>3)&0x001F);              // B

  send_data(MSB(c));
  send_data(LSB(c));
}

void LCD_square(int x, int y, int w, int h, unsigned long color)
{
  LCD_setcol(x, x+w);
  LCD_setpage(LCD_HEIGHT - y, LCD_HEIGHT - y + h);
  send_command(TFTCMD_MEM_WRITE);
  // convert from RGB888 to RGB565

  unsigned short c = 
      ((color>>8)&0xF800) |             // R
      ((color>>5)&0x07E0) |             // G
      ((color>>3)&0x001F);              // B

  unsigned char msb = MSB(c);
  unsigned char lsb = LSB(c);     
  SELECT_DATA;
  for(int i=0; i<w*h; i++)
  {
     transfer_locked(msb);
     transfer_locked(lsb);
  }
}

void LCD_setpixels(int x, int y, int num_pixels_x, int num_pixels_y, unsigned long* pixels)
{
  wait_for_unlock_spi();
  LCD_setcol(x, x+num_pixels_x-1);
  LCD_setpage(y, y+num_pixels_y-1);
  send_command(TFTCMD_MEM_WRITE);
  // convert from RGB888 to RGB565
  SELECT_DATA;
  uint16_t scanline[32]; 
  int total = num_pixels_x * num_pixels_y;
  int nblocks = total/32;
  int rest = total - (nblocks*32);
  int idx = 0;
  unsigned long color;
  do
  {
    int sz = nblocks>0 ? 32 : rest; 
    for (int i=0; i<sz; i++, idx++)
    {
      color = pixels[idx]; 
      scanline[i] = 
        ((color>>8)&0xF800) |             // R
        ((color>>5)&0x07E0) |             // G
        ((color>>3)&0x001F);              // B
    }
    transfer_dma((uint8_t*)scanline, num_pixels_x*2);  
    nblocks--;
  }
  while(nblocks>=0); 
}

void tft_init(void)
{
  SELECT_TFT;

  send_command(TFTCMD_SW_RESET);
  send_command(TFTCMD_WAKEUP); 
  send_command(TFTCMD_DISPAY_ON);
  send_command(TFTCMD_PIXELFORMAT_SET);
//  send_data(6);                // RGB666-bitpixel format
  send_data(5);                // RGB565-bitpixel format

  send_command(0x36);      /* Memory Access Control        */
  send_data(0x08);

  send_command(0x3A);
  send_data(0x55);

  send_command(0xB1);
  send_data(0x00);
  send_data(0x1B);

//  send_command(0xB6);      /* Display Function Control     */
//  send_data(0x0A);
//  send_data(0xA2);

}

