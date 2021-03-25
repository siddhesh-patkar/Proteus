
#include "keypad.h"

Keypad::Keypad (uint8_t r, uint8_t c, const char *t) : numrows(r), numcols(c), keytable(t) {}

void Keypad::begin ()
// Initialize for scanning.
 { for (uint8_t i=0; i<numrows; ++i)
    { pinMode(i, OUTPUT);
      digitalWrite(i, HIGH);
    }
   for (uint8_t i=0; i<numcols; ++i)
      pinMode(numrows+i, INPUT_PULLUP);
 }
 
String Keypad::getKey (bool wait)
// Scan the keypad, return any valid keypress as a string.
 { String result;
   char key;
   do key = scan();
      while (key == 0 && wait);         
   if (key > 0)
      result += char(key);
   return result;
 }

void Keypad::waitPress()
// Wait for any key to be pressed.
 { while (scan() == 0);
 }

void Keypad::waitRelease()
 { while (scan() != 0);
 }

int Keypad::scan ()
// Perform a scan of the keypad and return the character that is pressed.
// Returns 0 if no key pressed, -1 if several keys pressed otherwise the ascii character code.
 { int result = 0;
   uint8_t count = 0, i, j;
    
   // Drive each row in turn low, then test the rows 
   for (i=0; i<numrows; ++i)
    { digitalWrite(i, LOW);

      for (j=0; j<numcols; ++j)
         if (digitalRead(numrows+j) == false)
          { if (count++ == 0)
               result = keytable[i*numcols+j];
            else
               result = -1; // multiple keys down
          }

      digitalWrite(i, HIGH);
    }
  
   return result;     
 }
