// Generic GPS driver for Proteus Visual Designer
// derived from the Adafruit GPS library - see licence.txt

#include "core.h"
#include "gps.h"


/*********************************************************************
**** Top Level / Visual Designer API ****
****************************************/

GPS::GPS (uint8_t rx, uint8_t tx)
// The constructor uses the pin ids to determine whether we can use hardware or software serial.
 { if (rx == 0 && tx == 1)
    { serial = gpsHwSerial = &Serial;
      gpsSwSerial = NULL;      
    }
   else
    { serial = gpsSwSerial = new SoftwareSerial(rx, tx);
      gpsHwSerial = NULL;
    }
      
   // Reset all variables:   
   hour = minute = seconds = year = month = day =
   fixquality = satellites = 0; 
   lat = lon = mag = 0; 
   fix = false; 
   milliseconds = 0; 
   latitude = longitude = geoidheight = altitude =
   speed = angle = magvariation = HDOP = 0.0;     
   
   // Reset line buffers:
   currentline = line1;
   lastline    = line2;
   lineidx     = 0;
  
 }
 
GPS::~GPS()
 { delete gpsSwSerial;
 } 

void GPS::begin(uint16_t baud)
// Pass the baud rate to the serial interface:
 { if(gpsSwSerial) 
    { gpsSwSerial->begin(baud);
      #if F_CPU < 16000000L
          // Force lower baud rate for simulation with sofware serials
          sendCommand(PMTK_SET_BAUD_4800);
          delay(100);
          gpsSwSerial->begin(4800);                       
      #endif
    }
   else 
      gpsHwSerial->begin(baud);
   enableInterrupt(true);  
   delay(10);
   sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
 }

void GPS::end() 
 { enableInterrupt(false);
   if(gpsSwSerial) 
     gpsSwSerial->end();
   else 
     gpsHwSerial->end();   
 }

boolean GPS::operator() (void)
// Update fix info from latest sentence (if any) and return TRUE if there is a valid fix based on GGA and RMC messages.
 { if (newNMEAreceived())
      parse(lastNMEA());
   return fix && fixquality > 0;
 }

void GPS::waitForFix (boolean waitForNew, uint16_t timeout)
// Wait until we have new fix data. If waitForNew is true we clear the validity flags such
// that we must then wait until we have received new GGA and RMC sentences.   
 { uint32_t start = millis();
   if (waitForNew)
      fix = false, fixquality = 0;
   while ((!fix || !fixquality) && (timeout == 0 || millis() < start+timeout))
    { if (newNMEAreceived())
         if (!parse(lastNMEA()))
            delay(10);
      if (millis() < start)
         start = millis();      
    }    
 }

float GPS::getDistance(float lat1, float lng1, float lat2, float lng2)
// Return haversine distance between two locations.   
 { float R = 6371000; // metres
   float phi1 = PI*lat1/180.0;
   float phi2 = PI*lat2/180.0;
   float dphi = PI*(lat2-lat1)/180.0;
   float dlambda = PI*(lng2-lng1)/180.0;
   float a = sin(dphi/2) * sin(dphi/2) + cos(phi1) * cos(phi2) * sin(dlambda/2) * sin(dlambda/2);
   float c = 2 * atan2(sqrt(a), sqrt(1-a));   
   return R*c;
 }
      
float GPS::getBearing(float lat1, float lng1, float lat2, float lng2)
// return the initial bearing between two positions.
 { float phi1 = PI*lat1/180.0;
   float phi2 = PI*lat2/180.0;
   float dlamda = PI*(lng2-lng1)/180.0;   
   float y = sin(dlamda) * cos(phi2);
   float x = cos(phi1)*sin(phi2) - sin(phi1)*cos(phi2)*cos(dlamda);
   return 180.0*atan2(y, x)/PI;
 }
 
void GPS::setUpdateRate (uint16_t rate)
 { switch (rate)
    { case 10000 : sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ); break;
      case 5000  : sendCommand(PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ); break;
      case 1000  : sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); break;
      case 500   : sendCommand(PMTK_SET_NMEA_UPDATE_2HZ); break;
      case 200   : sendCommand(PMTK_SET_NMEA_UPDATE_5HZ); break;
    }    
 }

void GPS::setFixRate (uint16_t rate)
 { switch (rate)
    { case 10000 : sendCommand(PMTK_API_SET_FIX_CTL_100_MILLIHERTZ); break;
      case 5000  : sendCommand(PMTK_API_SET_FIX_CTL_200_MILLIHERTZ); break;
      case 1000  : sendCommand(PMTK_API_SET_FIX_CTL_1HZ); break;
      case 500   : sendCommand(PMTK_API_SET_FIX_CTL_2HZ); break;
      case 200   : sendCommand(PMTK_API_SET_FIX_CTL_5HZ); break;
    }    
 }
      
void GPS::sleep (boolean b)
 { if (b)
    { if (!inStandbyMode) 
       { sendCommand(PMTK_STANDBY);
         inStandbyMode = true;         
       }    
    }
   else 
    { sendCommand("");  // send byte to wake it up
      inStandbyMode = false;      
    }
 }


/*********************************************************************
**** Low Level - NMEA/PMTK Interface ****
****************************************/

void GPS::sendCommand(const char *str) 
 { serial->println(str);
 }

boolean GPS::waitForSentence(const char *wait4me, uint8_t max) 
 { char str[20];

   uint8_t i=0;
   while (i < max) 
    { if (newNMEAreceived()) 
       { char *nmea = lastNMEA();
         strncpy(str, nmea, 20);
         str[19] = 0;
         i++;  

         if (strstr(str, wait4me))
	         return true;
       }
    }
   return false;
 }

boolean GPS::parse(char *nmea) 
 { // do checksum check
   // first look if we even have one
   if (nmea[strlen(nmea)-4] == '*') 
    { uint16_t sum = parseHex(nmea[strlen(nmea)-3]) * 16;
      sum += parseHex(nmea[strlen(nmea)-2]);
        
      // check checksum 
      for (uint8_t i=1; i < (strlen(nmea)-4); i++) 
         sum ^= nmea[i];

      if (sum != 0) 
       { // bad checksum :(
         return false;
       }
    }

   int32_t degree;
   long minutes;
   char degreebuff[10];
  
   // look for a few common sentences
   if (strstr(nmea, "$GPGGA")) 
    { // found GGA
      char *p = nmea;
  
      // get time hhmmss.nnn
      p = strchr(p, ',')+1;
      float timef = atof(p);
      uint32_t time = timef;
      hour = time / 10000;
      minute = (time % 10000) / 100;
      seconds = (time % 100);

      milliseconds = fmod(timef, 1.0) * 1000;
      
      // parse out latitude (dddmm.nnnn - degrees/decimal minutes)
      if (p = strchr(p, ',')+1, ',' != *p)
       { strncpy(degreebuff, p, 2);
         p += 2;
         degreebuff[2] = '\0';
         degree = atol(degreebuff) * 10000000;
         strncpy(degreebuff, p, 2); // minutes
         p += 3; // skip decimal point
         strncpy(degreebuff + 2, p, 4);
         degreebuff[6] = '\0';
         minutes = 50 * atol(degreebuff) / 3;
      
         latitude_fixed = degree + minutes;
         latitude = degree / 100000 + minutes * 0.000006F;
         latitudeDegrees = (latitude-100*int(latitude/100))/60.0;
         latitudeDegrees += int(latitude/100);
       }
    
      // Parse out sign - N/S
      if (p = strchr(p, ',')+1, ',' != *p)
       { if (p[0] == 'S') 
            latitudeDegrees *= -1.0;
         if (p[0] == 'N') 
            lat = 'N';
         else if (p[0] == 'S') 
            lat = 'S';
         else if (p[0] == ',') 
            lat = 0;
         else 
            return false;
       }
    
      // parse out longitude
      if (p = strchr(p, ',')+1, ',' != *p)
       { strncpy(degreebuff, p, 3);
         p += 3;
         degreebuff[3] = '\0';
         degree = atol(degreebuff) * 10000000;
         strncpy(degreebuff, p, 2); // minutes
         p += 3; // skip decimal point
         strncpy(degreebuff + 2, p, 4);
         degreebuff[6] = '\0';
         minutes = 50 * atol(degreebuff) / 3;
         
         longitude_fixed = degree + minutes;
         longitude = degree / 100000 + minutes * 0.000006F;
         longitudeDegrees = (longitude-100*int(longitude/100))/60.0;
         longitudeDegrees += int(longitude/100);
       }
    
      // parse sign - E/W
      if (p = strchr(p, ',')+1, ',' != *p)
       { if (p[0] == 'W') 
            longitudeDegrees *= -1.0;
         if (p[0] == 'W') 
            lon = 'W';
         else if (p[0] == 'E') 
            lon = 'E';
         else if (p[0] == ',') 
            lon = 0;
         else 
            return false;
       }
    
      // Parse fix quality and altitude data:
      if (p = strchr(p, ',')+1, ',' != *p)
         fixquality = atoi(p);   
      if (p = strchr(p, ',')+1, ',' != *p)
         satellites = atoi(p);  
      if (p = strchr(p, ',')+1, ',' != *p)
         HDOP = atof(p);    
      if (p = strchr(p, ',')+1, ',' != *p)
         altitude = atof(p);   
      if (p = strchr(p, ',')+1, p = strchr(p, ',')+1, ',' != *p)
         geoidheight = atof(p);
      return true;
    }
   
   if (strstr(nmea, "$GPRMC")) 
    { // found RMC
      char *p = nmea;

      // get time hhmmss.nnn
      p = strchr(p, ',')+1;
      float timef = atof(p);
      uint32_t time = timef;
      hour = time / 10000;
      minute = (time % 10000) / 100;
      seconds = (time % 100);
   
      milliseconds = fmod(timef, 1.0) * 1000;

      // Parse fix/no-fix
      p = strchr(p, ',')+1;
      if (p[0] == 'A') 
         fix = true;
      else if (p[0] == 'V')
         fix = false;
      else
         return false;

      // parse out latitude
      if (p = strchr(p, ',')+1, ',' != *p)
       { strncpy(degreebuff, p, 2);
         p += 2;
         degreebuff[2] = '\0';
         long degree = atol(degreebuff) * 10000000;
         strncpy(degreebuff, p, 2); // minutes
         p += 3; // skip decimal point
         strncpy(degreebuff + 2, p, 4);
         degreebuff[6] = '\0';
         long minutes = 50 * atol(degreebuff) / 3;
       
         latitude_fixed = degree + minutes;
         latitude = degree / 100000 + minutes * 0.000006F;
         latitudeDegrees = (latitude-100*int(latitude/100))/60.0;
         latitudeDegrees += int(latitude/100);
       }
    
      // Parse out sign - N/S
      if (p = strchr(p, ',')+1, ',' != *p)
       { if (p[0] == 'S') 
            latitudeDegrees *= -1.0;
         if (p[0] == 'N') 
            lat = 'N';
         else if (p[0] == 'S') 
            lat = 'S';
         else if (p[0] == ',') 
            lat = 0;
         else 
            return false;
       }
    
      // parse out longitude
      if (p = strchr(p, ',')+1, ',' != *p)
       { strncpy(degreebuff, p, 3);
         p += 3;
         degreebuff[3] = '\0';
         degree = atol(degreebuff) * 10000000;
         strncpy(degreebuff, p, 2); // minutes
         p += 3; // skip decimal point
         strncpy(degreebuff + 2, p, 4);
         degreebuff[6] = '\0';
         minutes = 50 * atol(degreebuff) / 3;
      
         longitude_fixed = degree + minutes;
         longitude = degree / 100000 + minutes * 0.000006F;
         longitudeDegrees = (longitude-100*int(longitude/100))/60.0;
         longitudeDegrees += int(longitude/100);
       }
    
      // parse sign E/W
      if (p = strchr(p, ',')+1, ',' != *p)
       { if (p[0] == 'W') 
            longitudeDegrees *= -1.0;
         if (p[0] == 'W') 
            lon = 'W';
         else if (p[0] == 'E') 
            lon = 'E';
         else if (p[0] == ',') 
            lon = 0;
         else 
            return false;
       }

      // Parse speed/bearing
      if (p = strchr(p, ',')+1, ',' != *p)
         speed = atof(p);
      if (p = strchr(p, ',')+1, ',' != *p)
         angle = atof(p);
    
      // Parse date-stamp
      if (p = strchr(p, ',')+1, ',' != *p)
       { uint32_t fulldate = atof(p);
         day = fulldate / 10000;
         month = (fulldate % 10000) / 100;
         year = (fulldate % 100);
       }
    
      // Update the system time if not set by another source.
      // This leaves open the issue of time zones because the time-code is utc.
      // It appears that the ZDA record may include local time info.
      if (timeStatus() == timeNotSet)
         if (fix)
          { time_t t = makeTime(2000+year, month, day, hour, minute, seconds);
            setTime(t);
          }
    
      // we dont parse the remaining, yet!
      return true;
    }

   // Unsupported NMEA sentence
   return false;
 }

// read a Hex value and return the decimal equivalent
uint8_t GPS::parseHex(char c) {
    if (c < '0')
      return 0;
    if (c <= '9')
      return c - '0';
    if (c < 'A')
       return 0;
    if (c <= 'F')
       return (c - 'A')+10;
    // if (c > 'F')
    return 0;
}


/*********************************************************************
**** Interrupt Based Serial Reader ****
**************************************/

/* JKJ - this looks a bit flawed to me. If parse() is called just before we complete the filling of he second buffer, the reader will switch
   buffers and start to overwrite the sentence being parsed which could result in corrupted parsing. Best not to use this code in an airliner ;-)
*/   

char *debug;

// Max length of NMEA sentence
#define MAXLINELENGTH 120

// Serial reader buffers:
Stream *GPS::serial;      

volatile char GPS::line1[MAXLINELENGTH];
volatile char GPS::line2[MAXLINELENGTH];
      
// our index into filling the current line
volatile uint8_t GPS::lineidx;
      
// pointers to the double buffers
volatile char *GPS::currentline;
volatile char *GPS::lastline;
volatile boolean GPS::recvdflag;

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) 
 { GPS::read();
 }

void GPS::enableInterrupt(boolean v) 
 { if (v) 
    { // Timer0 is already used for millis() - we'll just interrupt somewhere
      // in the middle and call the "Compare A" function above
      OCR0A = 0xAF;
      TIMSK0 |= _BV(OCIE0A);
    } 
   else 
    { // do not call the interrupt function COMPA anymore
      TIMSK0 &= ~_BV(OCIE0A);
    } 
 }
   
void GPS::read()   
 { while (serial->available())
    { char c = serial->read();

      if (c == '\n') 
       { // Got EOL - switch line buffers: 
         currentline[lineidx] = 0;

         if (currentline == line1) 
         { currentline = line2;
           lastline = line1;
         }
        else 
         { currentline = line1;
           lastline = line2;
         }
   
         //Serial.println("----");
         lineidx = 0;
         recvdflag = true;
       }
      else 
       { currentline[lineidx++] = c;
         if (lineidx >= MAXLINELENGTH)
            lineidx = MAXLINELENGTH-1;
       }
    }     
 }

boolean GPS::newNMEAreceived(void) 
 { return recvdflag;
 }

char *GPS::lastNMEA(void) 
 { recvdflag = false;
   debug = (char *)lastline;
   return (char *)debug;
 }


