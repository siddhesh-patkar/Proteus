#pragma once

#include <arduino.h>
 
class GroveButton
 { public:
      GroveButton (uint8_t id) { pin = id; }
      bool operator()() { return digitalRead(pin); }
 
   private:
      uint8_t pin;
 };
 
class GroveLED
 { public:
      GroveLED (uint8_t id) { pin = id; pinMode(pin,OUTPUT); }
      bool operator()() { return digitalRead(pin); }
      void on() { digitalWrite(pin,HIGH); }
      void off() { digitalWrite(pin,LOW); }
      void set(bool s) { digitalWrite(pin, s ? HIGH : LOW); }
      void dim(int v) { analogWrite(pin, v); }
      void toggle() { digitalWrite(pin,digitalRead(pin)? LOW:HIGH); }

   private:
      uint8_t pin;
 };
 
class GroveRelay
 { public:
      GroveRelay (uint8_t id) { pin = id; pinMode(pin,OUTPUT); }
      bool operator()() { return digitalRead(pin); }
      void on() { digitalWrite(pin,HIGH); }
      void off() { digitalWrite(pin,LOW); }
      void set(bool s) { digitalWrite(pin, s ? HIGH : LOW); }
   
   private:
      uint8_t pin;
 };

class GroveTemperatureSensor
 { public:
      GroveTemperatureSensor (uint8_t id) { pin = id; pinMode(analogInputToDigitalPin(pin),INPUT); }
      bool operator()(float trigger) { return readCelcius() >= trigger; }
      float readCelcius()  { return readKelvin()-273.15; }
      float readFarenheit() { return 9*readKelvin()/5 - 459.67; }
      float readKelvin() { return 1.0/(log(resistance()/10000)/3975+1/298.15); }

   private:
      float resistance() { int a = analogRead(pin); return (1023-a)*10000.0/a;   }
 	 
   private:
      uint8_t pin;
 }; 

class GroveBuzzer
 { public:
      GroveBuzzer (uint8_t id) { pin = id; pinMode(pin,OUTPUT); }
      void on() { digitalWrite(pin,HIGH); }
      void off() { digitalWrite(pin,LOW); }
      void set(bool s) { digitalWrite(pin, s ? HIGH : LOW); }
   
   private:
      uint8_t pin;
 };
 
class GroveTouchSensor
 { public:
      GroveTouchSensor (uint8_t id) { pin = id; }
      bool operator()() { return digitalRead(pin); }

   private:
      uint8_t pin;
 };

class GroveLightSensor
 { public:
      GroveLightSensor(uint8_t id) { Gamma = -0.858; 
                                     R1Lux = 127410.0; 
                                     Rref = 10000.0; 
                                     Vcc = 5.0; 
                                     pin = id; pinMode(analogInputToDigitalPin(pin),INPUT); }
      
      bool operator()(float trigger) { return readLight() >= trigger; }
      float readLight()  { return pow((resistance()/R1Lux), (1.0/Gamma)); }
      float readResistance() { return resistance(); }
      float readRaw() { return adcval(); }

   private:
      int adcval() { return analogRead(pin); }
      float resistance() { float vout = (Vcc/1024.0 * adcval()); return (Rref * Vcc/vout)-Rref; }
   
   private:
      uint8_t pin;
      float Gamma;   // Gamma coefficient. LDR model type dependent.
      float R1Lux;   // LDR resistance value @ 1 Lux. LDR model type dependent.
      float Rref;    // Value of reference divider resistor. 
      float Vcc;     // Arduino Power supply and ADC reference.     
 }; 

class GroveRotaryAngleSensor
 { public:
      GroveRotaryAngleSensor(uint8_t id, float angle) { pin = id; pinMode(analogInputToDigitalPin(pin),INPUT); Full_angle = angle; }
      bool operator()(float trigger) { return readAngle() >= trigger; }
      float readAngle()  { float vout = (float) adcval() / 1024 * Full_angle; return vout;}
      int readRaw() { return adcval(); }

   private:
      int adcval() { return analogRead(pin); }
 	 
   private:
      uint8_t pin;
      float Full_angle;
 }; 

class GroveSwitch
 { public:
      GroveSwitch (uint8_t id) { pin = id; }
      bool operator()() { return digitalRead(pin); }
 
   private:
      uint8_t pin;
 };

class GroveVoltageDivider
 { public:
      GroveVoltageDivider (uint8_t id, uint8_t Divisor) { divisor = Divisor? 10:3 ; pin = id; pinMode(analogInputToDigitalPin(pin),INPUT); }
      bool operator()(float trigger) { return readVoltage() >= trigger; }
      int readAnalog()  { return readValue(1); }
      int readAverageAnalog(int navr)  { return readValue(navr); }
      float readVoltage() { return divisor*readAverageAnalog(100)*5.0/1023.0; }
 	
   private:
      int readValue(int num_average) { an=0; for (idx=0; idx<num_average; idx++) an = an + analogRead(pin); return an/num_average; } 
  
   private:
      uint8_t pin;
      long an;
      int idx;
      int divisor;
 }; 

class GroveLuminanceSensor
 { public:
      GroveLuminanceSensor(uint8_t id) { pin = id; pinMode(analogInputToDigitalPin(pin),INPUT); }
      bool operator()(float trigger) { return readLuminance() >= trigger; }
      float readLuminance()  { vout = voltage(); float lux = -14.266*pow(vout,3)+76.189*pow(vout,2)+389.65*vout-0.8606; if (lux<0.0) lux=0.0; return lux; }
      float readVoltage()  { return voltage(); }

   private:
      int adcval() { return analogRead(pin); }
      float voltage() { return (5.0/1024.0 * adcval()); }
   
   private:
      uint8_t pin;
      float vout;
 }; 

class GroveInfraredProximitySensor
 { public:
      GroveInfraredProximitySensor(uint8_t id) { pin = id+1; pinMode(analogInputToDigitalPin(pin),INPUT); } // VO connects to SIG1, case 2942
      bool operator()(float trigger) { return readDistance() >= trigger; }
      float readVoltage()  { return voltage(); }
      float readDistance()  { return pow(voltage(),-1.219)*26.714; }

   private:
      float voltage() { an=0; for (idx=0; idx<20; idx++) an = an + analogRead(pin); return an/20 * 5.0/1024.0; }
   
   private:
      uint8_t pin;
      long an;
      int idx;
      float vout;
 }; 

class GroveDifferentialAmplifier
 { public:
      GroveDifferentialAmplifier (uint8_t id) { pin = id; pinMode(analogInputToDigitalPin(pin),INPUT); }
      bool operator()(float trigger) { return readVoltage() >= trigger; }
      int readAnalog()  { return readValue(1); }
      int readAverageAnalog(int navr)  { return readValue(navr); }
      float readVoltage() { return readAverageAnalog(100)*5.0/1023.0; }
 	
   private:
      int readValue(int num_average) { an=0; for (idx=0; idx<num_average; idx++) an = an + analogRead(pin); return an/num_average; } 
  
   private:
      uint8_t pin;
      long an;
      int idx;
 }; 

class GroveSoundSensor
 { public:
      GroveSoundSensor (uint8_t id) { pin = id; pinMode(analogInputToDigitalPin(pin),INPUT); }
      bool operator()(int trigger) { return readLevel() >= trigger; }
      int readLevel()  { return analogRead(pin); }
 	 
   private:
      uint8_t pin;
 };
 
class ThemocoupleAmplifier
 { public:
      ThemocoupleAmplifier (uint8_t id) { pin = id; pinMode(analogInputToDigitalPin(pin), INPUT); } 
      bool operator()(int trigger) { return readCelsius() >= trigger; }
      float readCelsius() { return (readAdc()*0.00488-1.25)/0.005; }
      float readFahrenheit() { return readCelsius()*1.8+32.0; }
      
      int readAdc() { return analogRead(pin); }

   private:
      uint8_t pin;
 };
 
class Mpx4250PressureGauge
 { public:
      Mpx4250PressureGauge (uint8_t id) { pin = id; pinMode(analogInputToDigitalPin(pin), INPUT); } 
      bool operator()(int trigger) { return readPressure() >= trigger; }
      float readPressure() { return ((readAdc()/1024.0-0.05274)/0.00368568); }
      float convertPressure(uint8_t units) 
       { switch (units) 
          { case 0 : k = 100.0;    break; // bar
            case 1 : k = 101.325;  break; // Atm
            case 2 : k = 0.13330;  break; // mmHg
            case 3 : k = 6.89476;  break; // Psi 
          
          }
         return ((float) readPressure()/k); 
       }
      
      int readAdc() { return analogRead(pin); }

   private:
      uint8_t pin;
      float  k;
 }; 