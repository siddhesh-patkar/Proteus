class RelayBoards
 { public:
      RelayBoards (uint8_t r1, uint8_t r2, uint8_t r3=2, uint8_t r4=3) 
       { pinR1 = r1; 
         pinMode(pinR1, OUTPUT); 
         pinR2 = r2;
         pinMode(pinR2, OUTPUT); 
         pinR3 = r3; 
         pinMode(pinR3, OUTPUT); 
         pinR4 = r4;
         pinMode(pinR4, OUTPUT); 
       }
      
      void relayOn(uint8_t n)          { digitalWrite(n==0? pinR1: n==1? pinR2 : n==2? pinR3 : pinR4, HIGH); }
      void relayOff(uint8_t n)         { digitalWrite(n==0? pinR1: n==1? pinR2 : n==2? pinR3 : pinR4, LOW); }
      void relaySet(uint8_t n, bool s) { digitalWrite(n==0? pinR1: n==1? pinR2 : n==2? pinR3 : pinR4, s ? HIGH : LOW); }
      bool relayGet(uint8_t n)         { return digitalRead(n==0? pinR1: n==1? pinR2 : n==2? pinR3 : pinR4); }
   
   private:
      uint8_t pinR1, pinR2, pinR3, pinR4;
 };
