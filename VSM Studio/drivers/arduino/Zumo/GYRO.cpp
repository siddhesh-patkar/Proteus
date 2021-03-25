#include "Zumo.h"

GYRO *GYRO::instance = 0;
GYRO *GYRO::getInstance()
 { return instance;
 }
 
 
GYRO::GYRO()
 { instance = this;
 }

void GYRO::begin()
 { if (TWCR == 0)
      Wire.begin();
   
   l3g.init();
   l3g.enableDefault();
 }

void GYRO::readAngularAcc(float *angX, float *angY, float *angZ)
 { uint8_t ctrl4 = l3g.readReg(L3G::CTRL4);
   uint8_t mfs = (ctrl4 & 0x0C) >> 4;
   l3g.read();
   
   switch (mfs)
    { case 0:
         *angX = (float)l3g.g.x / 32767.0 * 245.0;
         *angY = (float)l3g.g.y / 32767.0 * 245.0;
         *angZ = (float)l3g.g.z / 32767.0 * 245.0;
         break;
      case 1:
         *angX = (float)l3g.g.x / 32767.0 * 500.0;
         *angY = (float)l3g.g.y / 32767.0 * 500.0;
         *angZ = (float)l3g.g.z / 32767.0 * 500.0;
         break;
      case 2:
         *angX = (float)l3g.g.x / 32767.0 * 2000.0;
         *angY = (float)l3g.g.y / 32767.0 * 2000.0;
         *angZ = (float)l3g.g.z / 32767.0 * 2000.0;
         break;
    }
  }

void GYRO::readLinearAcc(float *linX, float *linY, float *linZ)
 { COMPASS *compass = COMPASS::getInstance();
   if (!compass)
      return;
   
   uint8_t ctrl2 = compass->lsm303.readReg(LSM303::CTRL2);
   uint8_t afs = (ctrl2 & 0x38) >> 3;
   compass->lsm303.readAcc();
   
   switch (afs)
    { case 0:
         *linX = (float)compass->lsm303.a.x / 32767.0f * 2.0f;
         *linY = (float)compass->lsm303.a.y / 32767.0f * 2.0f;
         *linZ = (float)compass->lsm303.a.z / 32767.0f * 2.0f;
         break;
      case 1:
         *linX = (float)compass->lsm303.a.x / 32767.0f * 4.0f;
         *linY = (float)compass->lsm303.a.y / 32767.0f * 4.0f;
         *linZ = (float)compass->lsm303.a.z / 32767.0f * 4.0f;
         break;
      case 2:
         *linX = (float)compass->lsm303.a.x / 32767.0f * 6.0f;
         *linY = (float)compass->lsm303.a.y / 32767.0f * 6.0f;
         *linZ = (float)compass->lsm303.a.z / 32767.0f * 6.0f;
         break;
      case 3:
         *linX = (float)compass->lsm303.a.x / 32767.0f * 8.0f;
         *linY = (float)compass->lsm303.a.y / 32767.0f * 8.0f;
         *linZ = (float)compass->lsm303.a.z / 32767.0f * 8.0f;
         break;
      case 4:
         *linX = (float)compass->lsm303.a.x / 32767.0 * 16.0f;
         *linY = (float)compass->lsm303.a.y / 32767.0 * 16.0f;
         *linZ = (float)compass->lsm303.a.z / 32767.0 * 16.0f;
         break;
    }
 }
