#pragma once

#include <Wire.h>
#include <Servo.h>

#include <QTRSensors.h>
#include <LSM303.h>
#include <L3G.h>

class DRIVE
 { public:
      enum Direction
       { Backwards = 0,
         Forwards = 1
       };
       
      enum Wheel
       { Left   = 1,
         Right  = 2,
         Both   = Left | Right
       };

   public:
      DRIVE(uint8_t _dir_l,
                uint8_t _spd_l,
                uint8_t _dir_r,
                uint8_t _spd_r)
          : dir_l(_dir_l)
          , spd_l(_spd_l)
          , dir_r(_dir_r)
          , spd_r(_spd_r)
          , isDriving(false)
          , x_min(0) , x_max(0)
          , y_min(0) , y_max(0)
          , z_min(0) , z_max(0)
          , centre_x(0) , centre_y(0)
          , angleNorth(0.0)
       {
       }

      bool operator() () { return isDriving; }
      void begin();
      
      void drive(uint8_t wheel=Both, uint8_t dir=Forwards, uint8_t speed=255);
      void forwards(uint8_t speed=255);
      void backwards(uint8_t speed=255);
      void turn(int16_t speed=255);
      void turnDegrees(int16_t degrees, int16_t speed=255);
      void stop() { forwards(0); }
      
      static DRIVE *getInstance();
      
   private:
      static DRIVE *instance;
      
      uint8_t dir_l, spd_l;
      uint8_t dir_r, spd_r;
      bool isDriving;
      short x_min, x_max;
      short y_min, y_max;
      short z_min, z_max;
      short centre_x, centre_y;
      float angleNorth;
};

class LINESENSOR : public QTRSensorsRC
 { public:
      LINESENSOR(uint8_t _lh0,
                  uint8_t _lh1, 
                  uint8_t _lh2, 
                  uint8_t _lh3, 
                  uint8_t _lh4, 
                  uint8_t _lh5, 
                  uint8_t _lh_emit)
          : lh0(_lh0)
          , lh1(_lh1)
          , lh2(_lh2)
          , lh3(_lh3)
          , lh4(_lh4)
          , lh5(_lh5)
          , lh_emit(_lh_emit)
       {
       }

      void begin();
      bool operator() (int min, int max);
      int readLinePos();
      int readRawValue(int index);
      
   private:
      uint8_t lh0;
      uint8_t lh1; 
      uint8_t lh2; 
      uint8_t lh3; 
      uint8_t lh4; 
      uint8_t lh5; 
      uint8_t lh_emit;
 };

class COMPASS
 { public:
      COMPASS();
      void begin();
      void readMagneticField(float *magX, float *magY, float *magZ);
      void calibrateHeading();
      float readHeading();
      
      static COMPASS *getInstance();
      
   private:
      static COMPASS *instance;
      float x_min, x_max, x_centre;
      float y_min, y_max, y_centre;
      float z_min, z_max;
      float angleNorth;
      bool isCalibrated;
      friend class GYRO;
      LSM303 lsm303;
  };

class GYRO
 { public:
      GYRO();
      void begin();
      void readAngularAcc(float *angX, float *angY, float *angZ);
      void readLinearAcc(float *linX, float *linY, float *linZ);
      
      static GYRO *getInstance();
      
   private:
      static GYRO *instance;
      L3G l3g;
  };
 