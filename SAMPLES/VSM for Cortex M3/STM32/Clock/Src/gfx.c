#include "gfx.h"
#include <math.h>

#define abs(v) (v>0?v:-v)

#define DEG2RAD   0.01745329251

//#include "sincos.inc"
#include "fonts.inc"

void fn_fontdraw_8x5(const struct DISPLAY_CONTEXT* ctx, char ch);

struct FONT font_8x5 = { 8, 5, 1, 2, (FONTDRAWFN)&fn_fontdraw_8x5 };

void line(int x0, int y0, int x1, int y1, unsigned long color)
{
    int x = x1-x0;
    int y = y1-y0;
    int dx = abs(x), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2;                                                /* error value e_xy             */
    for (;;){                                                           /* loop                         */
        LCD_setpixel(x0,y0,color);
        e2 = 2*err;
        if (e2 >= dy) {                                                 /* e_xy+e_x > 0                 */
            if (x0 == x1) break;
            err += dy; x0 += sx;
        }
        if (e2 <= dx) {                                                 /* e_xy+e_y < 0                 */
            if (y0 == y1) break;
            err += dx; y0 += sy;
        }
    }
}

/*
void circle(int x0, int y0, int r, unsigned long color)
{
  int xprev=x0;
  int yprev=y0;
  for (int a=0; a<360; a++)
  {
    float v = cos_table[a];
    float vv = r*v;
    int vvv = vv;
    int x = x0 + vvv;
    int y = y0 + r*sin_table[a];
    LCD_setpixel(x,y,color);
//    if (xprev!=x0 && yprev!=y0)
//       line(xprev, yprev, x, y, color); 
//    xprev = x;
//    yprev = y;
  }
}
*/
/*
void multiple(struct POINT* pts, int numpts, const float m[])
{
  for (int i=0; i<numpts; i++)
  {
    pts[i].x = m[0]*pts[i].x + m[1]*pts[i].y + m[2]*pts[i].z;
    pts[i].y = m[3]*pts[i].x + m[4]*pts[i].y + m[5]*pts[i].z;
    pts[i].z = m[6]*pts[i].x + m[7]*pts[i].y + m[8]*pts[i].z;
  } 
}

void rotateX(struct POINT* pts, int numpts, int angle_deg)
{
  const float m[9] = {
    1, 0, 0, 
    0, cos_table[angle_deg], -1*sin_table[angle_deg], 
    0, sin_table[angle_deg], cos_table[angle_deg]};

  multiple(pts, numpts, m);
}

void rotateY(struct POINT* pts, int numpts, int angle_deg)
{
  const float m[9] = {
    cos_table[angle_deg], 0, sin_table[angle_deg], 
    0, 1, 0,
   -1*sin_table[angle_deg],0, cos_table[angle_deg]};

  multiple(pts, numpts, m);
}

void rotateZ(struct POINT* pts, int numpts, int angle_deg)
{
  const float m[9] = {
    cos_table[angle_deg], -1*sin_table[angle_deg], 0,
    sin_table[angle_deg], cos_table[angle_deg], 0, 
    0, 0, 1};

  multiple(pts, numpts, m);
}
*/

uint8_t putchar_tft(struct DISPLAY_CONTEXT* ctx, char ch, unsigned long color)
{
  ctx->color_fg = color;

  int8_t xe = ctx->x + ctx->font->cx;   
  int8_t ye = ctx->y + ctx->font->cy;
    
  if (is_valid(ctx, xe, ye)==0)
  {
    if (ctx->auto_line_feed==0)
      return 0;
    // line feed        
    ctx->y = ye + ctx->font->spacey;
    ctx->x = 0;
    xe = ctx->font->cx;
  }

  // draw symbol inside valid bounds [ctx->x..xe, ctx->y..ye]
  (*ctx->font->fndraw)(ctx, ch);

  ctx->x = xe + ctx->font->spacex;
  return 1;  
}

void text(struct DISPLAY_CONTEXT* ctx, const char* str, int x0, int y0, unsigned long color)
{
  if (x0>=0) ctx->x = x0;
  if (y0>=0) ctx->y = y0;
  while(*str)
  {
    if (putchar_tft(ctx, *str, color)==0)
      return;
    str++;
  }
}

uint8_t is_valid(struct DISPLAY_CONTEXT* ctx, int16_t x, int16_t y)
{
  return !(x<0 || x>ctx->maxx || y<0 || y>ctx->maxy);  
}

void validate(struct DISPLAY_CONTEXT* ctx)
{
  if(ctx->x < 0) ctx->x = 0;
  else 
  if(ctx->x > ctx->maxx) ctx->x = ctx->maxx;
  if(ctx->y < 0) ctx->y = 0;
  else 
  if(ctx->y > ctx->maxy) ctx->y = ctx->maxy;
}

const struct FONT* select_font(struct DISPLAY_CONTEXT* ctx, const struct FONT* font)
{
  const struct FONT* old_font = ctx->font;
  ctx->font = font;
  return old_font;
}

void init_display_context(struct DISPLAY_CONTEXT* ctx)
{
  // initialize default display context
  ctx->x = 0;
  ctx->y = 0;
  ctx->maxx = LCD_WIDTH;
  ctx->maxy = LCD_HEIGHT;
  ctx->font = &font_8x5;
  ctx->auto_line_feed = 1;
  ctx->color_bg = 0xFFFFFF;
  ctx->color_fg = 0x000000;
}

struct SIZE get_text_size(struct DISPLAY_CONTEXT* ctx, const char* str)
{
  struct SIZE sz = {0, ctx->font->cy};
  for (;*str;str++)
  {
    sz.cx += ctx->font->cx;
    if (*(str+1))
      sz.cx += ctx->font->spacex;
  }
  return sz;
}

void fn_fontdraw_8x5(const struct DISPLAY_CONTEXT* ctx, char ch)
{
  uint8_t pixels;
	for(int8_t i=0 ; i < 5 ; i++ ) 
  {
    pixels = u8Font8x5[ch - 0x20][i];
    for(int8_t j=0; j<8; j++)
    {
      LCD_setpixel(ctx->x+i, ctx->y+j, pixels & 1 ? ctx->color_fg : ctx->color_bg);
      pixels>>=1;
    }  
	}
}
