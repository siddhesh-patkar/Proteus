#ifndef _MCP49x1_H
#define _MCP49x1_H

#include <SPI.h>
#include <Arduino.h>
#include <inttypes.h>

// Microchip MCP4901/MCP4911/MCP4921 DAC driver Thomas Backman, 2012
// Modified for Proteus Visual Designer.

// These are the supported DAC models:
//		MCP4901 = 1,
//		MCP4911 = 2,
//		MCP4921 = 3.

class MCP49x1 {
   public:

      MCP49x1(int _model, int _ss_pin, int _ldac_pin = -1);
      void setBuffer(boolean _buffer) { this->bufferVref = _buffer; }
      void setPortWrite(boolean _port_write) { this->port_write = _port_write; }
      void setGain(int _gain);
      void setSPIDivider(int _spi_divider);
      void shutdown(void);
      void output(unsigned short _out);
      void latch(void); // Actually change the output, if the LDAC pin isn't shorted to ground

   private:
      int ss_pin;
      int LDAC_pin;
      int bitwidth;
      boolean bufferVref;
      boolean gain2x;     /* false -> 1x, true -> 2x */
      boolean port_write; /* use optimized port writes? won't work everywhere! */
      int spi_divider;
 };

#endif
