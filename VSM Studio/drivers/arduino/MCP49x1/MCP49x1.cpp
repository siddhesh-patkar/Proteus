/*
 * Microchip MCP4901 / MCP4911 / MCP4921 8/10/12-bit DAC driver
 * Thomas Backman, 2012-07 (made a proper library 2011-07-30, 3 weeks after initial)
 * serenity@exscape.org
 
 * Code license: BSD/MIT. Whatever; I *prefer* to be credited,
 * and you're *not* allowed to claim you wrote this from scratch.
 * Apart from that, do whatever you feel like. Commercial projects,
 * code modifications, etc.

 Pins used: 
 * Arduino pin 11 (for Uno; for Mega: 51) to device SDI (pin 4) - fixed pin
 * Arduino pin 13 (for Uno; for Mega: 52) to device SCK (pin 3) - fixed pin
 * Any digital pin to device LDAC (DAC pin 5)  (except with PortWrite, see README)
 * Any digital pin to device CS   (DAC pin 2)  (as above)
 *
 * Other DAC wirings:  
 * Pin 1: VDD, to +5 V
 * Pin 5: LDAC, either to an Arduino pin, or ground to update vout automatically
 * Pin 6: VREF, to +5 V (or some other reference voltage 0 < VREF <= VDD)
 * Pin 7: VSS, to ground
 * Pin 8: vout
 * (Pin 9, for the DFN package only: VSS)

 * Only tested on MCP4901 (8-bit), but it should work on the others as well.
 * Tested on an Arduino Uno R3.
 */

// Modified for Proteus Visual Designer.

#include <SPI.h>
#include "MCP49x1.h"

MCP49x1::MCP49x1(int _model, int _ss_pin, int _LDAC_pin) : bufferVref(false), gain2x(false), port_write(false), spi_divider(SPI_CLOCK_DIV8)
{
  this->ss_pin = _ss_pin;
  if (_LDAC_pin>0)
     this->LDAC_pin = _LDAC_pin;

  // The only difference between these DACs is the number of bits of resolution
  switch (_model) {
	  case 1:
		  bitwidth = 8;
		  break;
	  case 2:
		  bitwidth = 10;
		  break;
	  case 3:
		  bitwidth = 12;
		  break;
	  default:
		  bitwidth = 8;
	}

	pinMode(ss_pin, OUTPUT); // Ensure that SS is set to SPI master mode
	pinMode(LDAC_pin, OUTPUT);

	digitalWrite(ss_pin, HIGH); // Unselect the device
	digitalWrite(LDAC_pin, HIGH); // Un-latch the output

	SPI.begin();  
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(spi_divider);
}

// Sets the gain. These DACs support 1x and 2x gain.
// vout = x/2^n * gain * VREF, where x = the argument to out(), n = number of DAC bits
// Example: with 1x gain, set(100) on a 8-bit (256-step) DAC would give
// an output voltage of 100/256 * VREF, while a gain of 2x would give
// vout = 100/256 * VREF * 2
void MCP49x1::setGain(int _gain) 
 {
 	if (_gain==2)
		// Gain 2x.
		gain2x = true;
	else
		// Gain 1x.
		gain2x = false;
 }

// Sets the SPI clock frequency. The final frequency will be the 
// main Arduino clock divided by the divider selected here.

// SPI_CLOCK_DIV4   0x00
// SPI_CLOCK_DIV16  0x01
// SPI_CLOCK_DIV64  0x02
// SPI_CLOCK_DIV128 0x03
// SPI_CLOCK_DIV2   0x04
// SPI_CLOCK_DIV8   0x05
// SPI_CLOCK_DIV32  0x06

void MCP49x1::setSPIDivider(int _div) {
	switch (_div) {
		case SPI_CLOCK_DIV2:
		case SPI_CLOCK_DIV4:
		case SPI_CLOCK_DIV8:
		case SPI_CLOCK_DIV16:
		case SPI_CLOCK_DIV32:
		case SPI_CLOCK_DIV64:
		case SPI_CLOCK_DIV128:
			spi_divider = _div;
			SPI.setClockDivider(_div);
		   break;
		default:
			spi_divider = SPI_CLOCK_DIV8;
			SPI.setClockDivider(SPI_CLOCK_DIV8);
		   break;
	}
}

// Shuts the DAC down. Shutdown current is about 1/50 (typical) of active mode current.
// My measurements say ~160-180 µA active (unloaded vout), ~3.5 µA shutdown.
// Time to settle on an output value increases from ~4.5 µs to ~10 µs, though (according to the datasheet).
void MCP49x1::shutdown(void) {
	// Drive chip select low
	if (this->port_write)
		PORTB &= 0xfb; // Clear PORTB pin 2 = arduino pin 10
	else
		digitalWrite(ss_pin, LOW);

	// Sending all zeroes should work, too, but I'm unsure whether there might be a switching time
	// between buffer and gain modes, so we'll send them so that they have the same value once we
	// exit shutdown.
	unsigned short out = (bufferVref << 14) | ((!(gain2x)) << 13); // gain == 0 means 2x, so we need to invert it
	SPI.transfer((out & 0xff00) >> 8);
	SPI.transfer(out & 0xff);

	// Return chip select to high
	if (this->port_write)
		PORTB |= (1 << 2); // set PORTB pin 2 = arduino pin 10
	else
		digitalWrite(ss_pin, HIGH);
}

// Send a new value for the DAC to output.
// Note that the output is only actually changed when latch() is called,
// if that functionality is used. If it's not, that pin (LDAC) can be
// shorted to ground, and the above note doesn't apply.
void MCP49x1::output(unsigned short data) {
	// Truncate the unused bits to fit the 8/10/12 bits the DAC accepts
	if (this->bitwidth == 12)
		data &= 0xfff;
	else if (this->bitwidth == 10)
		data &= 0x3ff;
	else if (this->bitwidth == 8)
		data &= 0xff;

	// Drive chip select low
	if (this->port_write)
		PORTB &= 0xfb; // Clear PORTB pin 2 = arduino pin 10
	else
		digitalWrite(ss_pin, LOW); 

	// bit 15: always 0 (1 means "ignore this command")
	// bit 14: buffer VREF?
	// bit 13: gain bit; 0 for 1x gain, 1 for 2x (thus we NOT the wariable)
	// bit 12: shutdown bit. 1 for active operation
	// bits 11 through 0: data 
	uint16_t out = (this->bufferVref << 14) | ((!this->gain2x) << 13) | (1 << 12) | (data << (12 - this->bitwidth));
  
	// Send the command and data bits
	SPI.transfer((out & 0xff00) >> 8);
	SPI.transfer(out & 0xff);

	// Return chip select to high
	if (this->port_write)
		PORTB |= (1 << 2); // set PORTB pin 2 = arduino pin 10
	else
		digitalWrite(ss_pin, HIGH);
}

// These DACs have a function where you can change multiple DACs at the same
// time: you call output() "sequentially", one DAC at a time, and *then*,
// when they've all received the output data, pull the LDAC pin low on
// all DACs at once. This function pulls the LDAC pin low for long enough 
// for the DAC(s) to change the output.
// If this function is undesired, you can simply tie the LDAC pin to ground.
// When tied to ground, you need *NOT* call this function!
void MCP49x1::latch(void) {
	// The datasheet says CS must return high for 40+ ns before this function
	// is called: no problems, that'd be taken care of automatically, as one
	// clock cycle at 16 MHz is longer... and there'll be a delay of multiple.

	if (LDAC_pin < 0)
		return;

	// We then need to hold LDAC low for at least 100 ns, i.e ~2 clock cycles.

	if (this->port_write) {
		// This gives ~180 ns (three clock cycles, most of which is spent low) of 
		// low time on a Uno R3 (16 MHz), measured on a scope to make sure
		PORTD &= ~(1 << 7); // Uno: digital pin 7; Mega: digital pin 38
		asm volatile("nop");
		PORTD |= (1 << 7);
	}
	else {
		// This takes far, FAR longer than the above despite no NOP; digitalWrite
		// is SLOW! For comparison: the above takes 180 ns, this takes... 3.8 us,
		// or 3800 ns, 21 times as long - WITHOUT having a delay in there!
		digitalWrite(LDAC_pin, LOW);
		digitalWrite(LDAC_pin, HIGH);
	}
}
