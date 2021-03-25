

#ifndef _PIMAGE_H
#define _PIMAGE_H

class Adafruit_GFX;

#if defined(__SD_H__)  // Arduino SD library


/// This class mimics Processing's PImage, but with fewer
/// capabilities. It allows an image stored in the SD card to be
/// drawn to the display.
/// @author Enrico Gueli <enrico.gueli@gmail.com>
class PImage {
public:
  PImage() : 
    _bmpWidth(0), 
    _bmpHeight(0), 
    _valid(false)
    { }
  
  void draw(Adafruit_GFX & glcd, int16_t x, int16_t y);
  
  static PImage loadImage(const char * fileName);
  
  
  bool isValid() { return _valid; }
  
  int16_t width() { return _bmpWidth; }
  int16_t height() { return _bmpHeight; }

   void close()
   { if (_bmpFile)
        _bmpFile.close();
   }
 
private:
  friend class Adafruit_GFX;
  
  SDLib::File _bmpFile;
  uint16_t _bmpWidth, _bmpHeight;  // W+H in pixels
  uint8_t  _bmpDepth;              // Bit depth (currently must be 24)
  uint32_t _bmpImageoffset;        // Start of image data in file
  uint32_t _rowSize;               // Not always = bmpWidth; may have padding
  bool     _flip; 
  
  bool     _valid;
  
  PImage(SDLib::File & bmpFile, int bmpWidth, int bmpHeight, uint8_t  bmpDepth, uint32_t bmpImageoffset, uint32_t rowSize, bool     flip) : 
  _bmpFile(bmpFile),
  _bmpWidth(bmpWidth),
  _bmpHeight(bmpHeight),
  _bmpDepth(bmpDepth),
  _bmpImageoffset(bmpImageoffset),
  _rowSize(rowSize),
  _flip(flip),
  _valid(true) // since Adafruit_GFX is friend, we could just let it write the variables and save some CPU cycles
  { }
  
  static uint16_t read16(SDLib::File f);
  static uint32_t read32(SDLib::File f);
};

#endif

#endif // _PIMAGE_H
