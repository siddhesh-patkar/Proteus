// Wrapper for the I2C port

#pragma once

#include <core.h>
#include <wire.h>
#include "i2cport.h"

I2CPort I2C;

I2CrxList &I2CrxList::arg (long &i)
 { if (Wire.available())
      i = Wire.read();
   return *this;         
 }
   
I2CrxList &I2CrxList::arg(float &f)
 { if (Wire.available())
      f = (int8_t)(Wire.read());
   return *this;         
 }
 
I2CrxList &I2CrxList::arg(String &s)
 { while (Wire.available())
      s += char(Wire.read());
 }      
