#include <arduino.h>
#include <core.h>

typedef void (*ISR)();

class CPU
 { public:
      // Thunks to Arduino layer:
 	   static void pinMode(uint8_t pin, uint8_t mode) { ::pinMode(pin, mode); }
 	   static void digitalWrite(uint8_t pin, bool state) { ::digitalWrite(pin, state ? HIGH : LOW); }
 	   static bool digitalRead(uint8_t pin) { return ::digitalRead(pin) == HIGH; } 	      
 	   static long pulseIn(uint8_t pin, uint8_t type, long timeout) { return ::pulseIn(pin, type, timeout); }
 	   static long millis() { return ::millis(); }
	   static void randomSeed(long seed) { return ::randomSeed(seed); }
 	   static void analogWrite(uint8_t pin, int value) { ::analogWrite(pin, value); }
 	   static int  analogRead(uint8_t pin) { return ::analogRead(pin); }
 	   static void analogReference(uint8_t mode) { ::analogReference(mode); }  	   	
 	   // Interrupt hooks
 	   static void enableInterrupt(uint8_t i, uint8_t mode) { ::attachInterrupt(i, isrs[i], mode); }
 	   static void disableInterrupt(uint8_t i) { ::detachInterrupt(i); }
 	   static void attachResetHandler(ISR isr) { isr(); }
 	   template<int n> static void attachInterruptHandler(ISR isr) { isrs[n] = isr; }
 	   static void tone(uint8_t _pin, unsigned int frequency, unsigned long duration = 0) { ::tone(_pin, frequency, duration); }
 	   static void noTone(uint8_t _pin) { ::noTone(_pin); }

 	   // Debug output - simulation only.
 	   static PrintList &debug() { return output.begin(true, isVsm() ? (Print *)&SimLog : (Print *)NULL); }      

      //Built-in LED
	static void setBuiltInLED(bool s) { pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, s); }

      // VSM Specific
      static void suspend() { PHANTOM=PHANTOM_CMD_BREAK; }
 
	 private:
	   static ISR isrs[6]; // Up to 6 on the Mega
	   static PrintList output;
 	   
 };
 

 extern CPU Cpu;
 