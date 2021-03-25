/*************************************************** 
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#if defined(__AVR__)
 #define WIRE Wire
#elif defined(CORE_TEENSY) // Teensy boards
 #define WIRE Wire
#else // Arduino Due
 #define WIRE Wire1
#endif

Adafruit_PWMServoDriver::Adafruit_PWMServoDriver(uint8_t addr) {
  _i2caddr = addr;
}
 
void Adafruit_PWMServoDriver::begin(void) {
 _i2caddr += a;
 WIRE.begin();
 reset();
}


void Adafruit_PWMServoDriver::reset(void) {
 write8(PCA9685_MODE1, 0x0);
}

void Adafruit_PWMServoDriver::setPWMFreq(float freq) {
  //Serial.print("Attempting to set freq ");
  //Serial.println(freq);
  freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
  float prescaleval = 25000000;
  prescaleval /= 4096;
  prescaleval /= freq;
  prescaleval -= 1;

  uint8_t prescale = floor(prescaleval + 0.5);
  
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  delay(5);
  write8(PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment.
                                          // This is why the beginTransmission below was not working.
}

void Adafruit_PWMServoDriver::setPWM(uint8_t num, uint16_t on, uint16_t off) {
  //Serial.print("Setting PWM "); Serial.print(num); Serial.print(": "); Serial.print(on); Serial.print("->"); Serial.println(off);

  WIRE.beginTransmission(_i2caddr);
  WIRE.write(LED0_ON_L+4*num);
  WIRE.write(on);
  WIRE.write(on>>8);
  WIRE.write(off);
  WIRE.write(off>>8);
  WIRE.endTransmission();
}

// Sets pin without having to deal with on/off tick placement and properly handles
// a zero value as completely off.  Optional invert parameter supports inverting
// the pulse for sinking to ground.  Val should be a value from 0 to 4095 inclusive.
void Adafruit_PWMServoDriver::setPin(uint8_t num, uint16_t val, bool invert)
{
  // Clamp value between 0 and 4095 inclusive.
  val = min(val, 4095);
  if (invert) {
    if (val == 0) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 4095) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, 4095-val);
    }
  }
  else {
    if (val == 4095) {
      // Special value for signal fully on.
      setPWM(num, 4096, 0);
    }
    else if (val == 0) {
      // Special value for signal fully off.
      setPWM(num, 0, 4096);
    }
    else {
      setPWM(num, 0, val);
    }
  }
}

uint8_t Adafruit_PWMServoDriver::read8(uint8_t addr) {
  WIRE.beginTransmission(_i2caddr);
  WIRE.write(addr);
  WIRE.endTransmission();

  WIRE.requestFrom((uint8_t)_i2caddr, (uint8_t)1);
  return WIRE.read();
}

void Adafruit_PWMServoDriver::write8(uint8_t addr, uint8_t d) {
  WIRE.beginTransmission(_i2caddr);
  WIRE.write(addr);
  WIRE.write(d);
  WIRE.endTransmission();
}

void Adafruit_PWMServoDriver::dimLed(uint8_t num, uint8_t min, uint8_t max, uint16_t dTime, bool invert)
 { uint8_t brightness = 0;
   bool cycleEnd;
   uint16_t step = 4096/256;
   
   cycleEnd = false;
   brightness = min;
   
   // Limit led number from 0 to 15, i.e. 16 single colour leds per board.
   if (num>15)
      num = 15;

   while (!cycleEnd)
    { setPin(num, brightness*step, invert);
      delay(dTime);

      if (min<max)
       { ++brightness;
         if (brightness >= max)
          { brightness = max;
            cycleEnd = true;
          }
       }
      else 
       { --brightness;
         if (brightness <= max)
          { brightness = max;
            cycleEnd = true;
          }
       }
    }
 } 

void Adafruit_PWMServoDriver::dimRgb(uint8_t num, uint8_t rMin, uint8_t rMax, uint8_t gMin, uint8_t gMax, uint8_t bMin, uint8_t bMax, uint16_t dTime, bool invert)
 { uint8_t rBrightness = 0;
   uint8_t gBrightness = 0;
   uint8_t bBrightness = 0;
   uint16_t step = 4096/256;

   bool allEnd, rEnd, gEnd, bEnd;
   rEnd = gEnd = bEnd = false;
   
   allEnd = rEnd && gEnd && bEnd;
  
   rBrightness = rMin;
   gBrightness = gMin;
   bBrightness = bMin;
   
   // Limit group number from 0 to 4, i.e. 5 rgb leds maximum per board.
   if (num>4)
      num = 4;
   
   while (!allEnd)
    { setPin(num*3+0, rBrightness*step, invert);
      setPin(num*3+1, gBrightness*step, invert);
      setPin(num*3+2, bBrightness*step, invert);
      
      delay(dTime);

      // Dim RED:
      if (rMin<rMax)
       { ++rBrightness;
         if (rBrightness >= rMax)
          { rBrightness = rMax;
            rEnd = true;
          }
       }
      else 
       { --rBrightness;
         if (rBrightness <= rMax)
          { rBrightness = rMax;
            rEnd = true;
          }
       }
      
      // Dim GREEN:
      if (gMin<gMax)
       { ++gBrightness;
         if (gBrightness >= gMax)
          { gBrightness = gMax;
            gEnd = true;
          }
       }
      else
       { --gBrightness;
         if (gBrightness <= gMax)
          { gBrightness = gMax;
            gEnd = true;
          }
       }
      // Dim BLUE:
      if (bMin<bMax)
       { ++bBrightness;
         if (bBrightness >= bMax)
          { bBrightness = bMax;
            bEnd = true;
          }
       }
      else
       { --bBrightness;
         if (bBrightness <= bMax)
          { bBrightness = bMax;
            bEnd = true;
          }
       }
     // Have all dimming cycles been completed ?
     allEnd = rEnd && gEnd && bEnd;
    }
 } 
