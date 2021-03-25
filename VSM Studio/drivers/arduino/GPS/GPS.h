// Generic GPS driver for Proteus Visual Designer
// derived from the Adafruit GPS library - see licence.txt

#include <arduino.h>
#include <SoftwareSerial.h>

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
// Note that these only control the rate at which the position is echoed, to actually speed up the
// position fix you must also send one of the position fix rate commands below too.
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ  "$PMTK220,10000*2F" // Once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ  "$PMTK220,5000*1B"  // Once every 5 seconds, 200 millihertz.
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_2HZ  "$PMTK220,500*2B"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"

// Not used in visual designer - too fast.
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"

// Position fix update rate commands.
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ  "$PMTK300,10000,0,0,0,0*2C" // Once every 10 seconds, 100 millihertz.
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ  "$PMTK300,5000,0,0,0,0*18"  // Once every 5 seconds, 200 millihertz.
#define PMTK_API_SET_FIX_CTL_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_API_SET_FIX_CTL_2HZ  "$PMTK300,500,0,0,0,0*28"
#define PMTK_API_SET_FIX_CTL_5HZ  "$PMTK300,200,0,0,0,0*2F"
// Can't fix position faster than 5 times a second!


#define PMTK_SET_BAUD_4800 "$PMTK251,4800*14"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"
#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"


#define PMTK_ENABLE_SBAS "$PMTK313,1*2E"
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E"

// standby command & boot successful message
#define PMTK_STANDBY "$PMTK161,0*28"
#define PMTK_STANDBY_SUCCESS "$PMTK001,161,3*36"  // Not needed currently
#define PMTK_AWAKE "$PMTK010,002*2D"

// ask for the release and version
#define PMTK_Q_RELEASE "$PMTK605*31"

// request for updates on antenna status 
#define PGCMD_ANTENNA "$PGCMD,33,1*6C" 
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D" 

// how long to wait when we're looking for a response
#define MAXWAITSENTENCE 5

class GPS 
 { public:  
      GPS (uint8_t rx, uint8_t tx); 
      ~GPS ();
      void begin(uint16_t baud); 
      void end();

      boolean operator() (void);
      void waitForFix (boolean waitForNew, uint16_t timeout);

      void getPosition (float *lat, float *lng) { *lat = latitudeDegrees, *lng = longitudeDegrees; }
      void getVelocity (float *s, float *b) { *s = speed, *b = angle; }
      float getAltitude() { return altitude; }
      int getSatellites() { return satellites; }
    
      float getDistance(float lat1, float lng1, float lat2, float lng2);  
      float getBearing(float lat1, float lng1, float lat2, float lng2);  
 
      void  setUpdateRate (uint16_t rate);
      void  setFixRate (uint16_t rate);
      void  sleep(boolean b);
 

   private:        
      void sendCommand(const char *);
      
      boolean waitForSentence(const char *wait4me, uint8_t max);
      boolean parse(char *);              
      uint8_t parseHex(char c);
      
      void enableInterrupt(boolean r);      
      
   public:
      static void read();
      static char *lastNMEA(void);
      static boolean newNMEAreceived();
        
   private:
      SoftwareSerial *gpsSwSerial;
      HardwareSerial *gpsHwSerial;
      boolean inStandbyMode;
   
      // Timestamp:
      uint8_t hour, minute, seconds, year, month, day;
      uint16_t milliseconds;
  
      // Floating point latitude and longitude value in degrees.
      float latitude, longitude;

      // Fixed point latitude and longitude value with degrees stored in units of 1/100000 degrees,
      // and minutes stored in units of 1/100000 degrees.  See pull #13 for more details:
      //   https://github.com/adafruit/Adafruit-GPS-Library/pull/13
      int32_t latitude_fixed, longitude_fixed;
      float latitudeDegrees, longitudeDegrees;
      float geoidheight, altitude;
      float speed, angle, magvariation, HDOP;
      char lat, lon, mag;
      boolean fix;
      uint8_t fixquality, satellites;
      
      // Serial reader buffers:
      static Stream *serial;      
      static volatile char line1[];
      static volatile char line2[];
      
      // our index into filling the current line
      static volatile uint8_t lineidx;
      
      // pointers to the double buffers
      static volatile char *currentline;
      static volatile char *lastline;
      static volatile boolean recvdflag;

  };
        
      


