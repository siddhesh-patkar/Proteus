/*
  LED bar library V2.0
  2010 Copyright (c) Seeed Technology Inc.  All right reserved.

  Original Author: LG

  Modify: Loovee, 2014-2-26
  User can choose which Io to be used.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/*
  Modify: Long, 2015-01-07
  User can change the brightness level for each LED segment
  Rename constant to avoid name conflict
*/
#ifndef LED_Bar_H
#define LED_Bar_H

#include <Arduino.h>

// Avoid name conflict
#define GLB_CMDMODE 0x00  // Work on 8-bit mode
#define GLB_ON      0xff  // 8-bit 1 data
#define GLB_OFF     0x00  // 8-bit 0 data

class LED_Bar
{

private:

  unsigned int __pinClock;  // Clock pin
  unsigned int __pinData;   // Data pin
  bool __greenToRed;        // Orientation (0 = red to green, 1 = green to red)
  unsigned char __state[10];// Led state, brightness for each LED

  void sendData(unsigned int data);  // Send a word to led bar
  void latchData(void);              // Load data into the latch register
  void setData(unsigned char bits[]);//Set data array

public:

  void begin(unsigned char pinData);
  void setGreenToRed(bool greenToRed);             // (Re)set orientation.
  void setLevel(float level);                      // Set level, range from 0 to 10.
  void setLed(unsigned char led, bool state);      // Set state for a single led.
  void toggleLed(unsigned char led);               // Toggle a single led.
  void setBits(unsigned int bits);                 // Set leds to match given bits mask.
  unsigned int const getBits();                    // Get the current bits mask state.
};

#endif