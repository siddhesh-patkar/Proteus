/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****                   Core / Utility functions               *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "core.h"

String toString (long n)
// Convert a signed integer to a string.
 { char buf[8 * sizeof(long) + 2]; // Assumes 8-bit chars plus negative zero byte.
   char *str = &buf[sizeof(buf) - 1];
   bool negative = n<0;
   
   *str = '\0';
   
   if (negative)
      n = -n;
   do 
    { unsigned long m = n;
      n /= 10;
      char c = m - 10 * n;
       *--str = c + '0';
    } while(n);
   if (negative)
      *--str = '-';
  
   return String(str);    
 }

String toString (unsigned long n)
// Convert an unsigned integer to a string.
 { char buf[8 * sizeof(long) + 2]; // Assumes 8-bit chars plus negative zero byte.
   char *str = &buf[sizeof(buf) - 1];

   *str = '\0';

   do 
    { unsigned long m = n;
      n /= 10;
      char c = m - 10 * n;
       *--str = c + '0';
    } while(n);
  
   return String(str);    
 }

String toString (float number)
// Convert a floating point number to a string with up to 6 decimal places
 { if (isnan(number)) return "nan";
   if (isinf(number)) return "inf";
   if (number > 4294967040.0) return "ovf";  // constant determined empirically
   if (number <-4294967040.0) return "ovf";  // constant determined empirically
  
   // Handle negative numbers 
   String s;
   if (number < 0.0)
    { s = "-";
      number = -number;
    }

   // Round correctly so that print(1.999, 2) prints as "2.00"
   uint8_t digits = 6;
   double rounding = 0.5;
   for (uint8_t i=0; i<digits; ++i)
      rounding /= 10.0;
  
   number += rounding;

   // Extract the integer part of the number and print it
   unsigned long int_part = (unsigned long)number;
   double remainder = number - (double)int_part;
   s += toString(int_part);

   // Print the decimal point, but only if there are digits beyond
   if (remainder > rounding)
      s += ".";

   // Extract digits from the remainder one at a time
   while (digits-- > 0 && remainder > rounding)
    { remainder *= 10.0;
      rounding *= 10.0;
      int toPrint = int(remainder);
      s += char(toPrint + '0');
      remainder -= toPrint; 
    }
   
   return s;
 }
 