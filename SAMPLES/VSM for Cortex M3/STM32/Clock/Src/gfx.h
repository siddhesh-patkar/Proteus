#ifndef GFX_H
#define GFX_H

#include <stdint.h>
#include "tft.h"

struct DISPLAY_CONTEXT;

typedef void (*FONTDRAWFN)(const struct DISPLAY_CONTEXT* ctx, char ch);

struct FONT
{
  uint8_t cy;
  uint8_t cx;
  uint8_t spacex;
  uint8_t spacey;
  FONTDRAWFN fndraw;
};

struct DISPLAY_CONTEXT
{
  int16_t maxx;
  int16_t maxy;

  int16_t x;
  int16_t y;
  const struct FONT* font;

  uint32_t color_bg;
  uint32_t color_fg;

  uint8_t auto_line_feed;
};

struct POINT
{
  float x;
  float y;
  float z;
};

struct SIZE
{
  uint16_t cx;
  uint16_t cy;
};

void init_display_context(struct DISPLAY_CONTEXT* ctx);

void line(int x0, int y0, int x1, int y1, unsigned long color);
void circle(int x0, int y0, int r, unsigned long color);
void symbol(int x0, int y0, int w, int h, const uint32_t* scanlines, uint32_t color);

void rotateX(struct POINT* pts, int numpts, int angle_deg);
void rotateY(struct POINT* pts, int numpts, int angle_deg);
void rotateZ(struct POINT* pts, int numpts, int angle_deg);

void text(struct DISPLAY_CONTEXT* ctx, const char* str, int x0, int y0, unsigned long color);
struct SIZE get_text_size(struct DISPLAY_CONTEXT* ctx, const char* str);

uint8_t is_valid(struct DISPLAY_CONTEXT* ctx, int16_t x, int16_t y);
void validate(struct DISPLAY_CONTEXT* ctx);

const struct FONT* select_font(struct DISPLAY_CONTEXT* ctx, const struct FONT* font);

#endif