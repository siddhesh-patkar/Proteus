#include "Zumo.h"

COMPASS *COMPASS::instance = 0;
COMPASS *COMPASS::getInstance()
 { return instance;
 }

 
COMPASS::COMPASS()
    : x_min(0.0f)
    , x_max(0.0f)
    , x_centre(0.0f)
    , y_centre(0.0f)
    , y_min(0.0f)
    , y_max(0.0f)
    , z_min(0.0f)
    , z_max(0.0f)
    , isCalibrated(false)
 { instance = this;
 }

void COMPASS::begin()
 { if (TWCR == 0)
      Wire.begin();
   lsm303.init();
   lsm303.enableDefault();
 }
 
void COMPASS::readMagneticField(float *magX, float *magY, float *magZ)
 { uint8_t ctrl6 = lsm303.readReg(LSM303::CTRL6);
   uint8_t mfs = (ctrl6 & 0x60) >> 5;
   lsm303.readMag();
   
   switch (mfs)
    { case 0:
         *magX = (float)lsm303.m.x / 32767.0f * 2.0f;
         *magY = (float)lsm303.m.y / 32767.0f * 2.0f;
         *magZ = (float)lsm303.m.z / 32767.0f * 2.0f;
         break;
      case 1:
         *magX = (float)lsm303.m.x / 32767.0f * 4.0f;
         *magY = (float)lsm303.m.y / 32767.0f * 4.0f;
         *magZ = (float)lsm303.m.z / 32767.0f * 4.0f;
         break;
      case 2:
         *magX = (float)lsm303.m.x / 32767.0f * 8.0f;
         *magY = (float)lsm303.m.y / 32767.0f * 8.0f;
         *magZ = (float)lsm303.m.z / 32767.0f * 8.0f;
         break;
      case 3:
         *magX = (float)lsm303.m.x / 32767.0f * 12.0f;
         *magY = (float)lsm303.m.y / 32767.0f * 12.0f;
         *magZ = (float)lsm303.m.z / 32767.0f * 12.0f;
         break;
    }
 }

void COMPASS::calibrateHeading()
 { float magX, magY, magZ;
   readMagneticField(&magX, &magY, &magZ);
   float magXN = magX;
   float magYN = magY;
   float magZN = magZ;
   x_min = x_max = magX;
   y_min = y_max = magY;
   z_min = z_max = magZ;
   
   DRIVE *drive = DRIVE::getInstance();
   drive->turn(100);
 
   short newMinMaxCount = 0;
   for (int count=0; count<100; count++)
    { readMagneticField(&magX, &magY, &magZ);
 
      if (magX < x_min)
       { x_min = magX;
         newMinMaxCount = count;
       }
      if (magY < y_min)
       { y_min = magY;
         newMinMaxCount = count;
       }
      if (magZ < z_min)
       { z_min = magZ;
         newMinMaxCount = count;
       }
       
      if (magX > x_max)
       { x_max = magX;
         newMinMaxCount = count;
       }
      if (magY > y_max)
       { y_max = magY;
         newMinMaxCount = count;
       }
      if (magZ > z_max)
       { z_max = magZ;
         newMinMaxCount = count;
       }
       
      // break if no new min max values after 500ms
      if (count - newMinMaxCount > 5)
         break;
      
      delay(100);
    }
    
   drive->stop();
    
   // now find the centre
   x_centre = (x_max + x_min) / 2.0f;
   y_centre = (y_max + y_min) / 2.0f;
   
   // and the angle for North
   angleNorth = atan2((magYN - y_centre) / (x_max - x_min), (magXN - x_centre) / (y_max - y_min));
   isCalibrated = true;
 }

float COMPASS::readHeading()
 { if (!isCalibrated)
     return -1.0f;
    
   float magX, magY, magZ;
   readMagneticField(&magX, &magY, &magZ);
 
   float angle = atan2((magY - y_centre) / (x_max - x_min), (magX - x_centre) / (y_max - y_min));
   float h = (angle - angleNorth) / M_PI * 180.0;
   if (h < -180.0f)
      h += 360.0f;
   else if (h > +180.0f)
      h -= 360.0f;
     
   return h;
 }
