import wiringpi

class Timer:
   _period = -1
   _running = False
   _now = 0
   _time = 0
   _counter = 0
   _hook = None

   def initialize(self, period):
      self._period = period

   def setPeriod(self, period):
      if period > 0:
         self._period = period

   def start(self):
      if not self._running:
         self._time = self._now
         if (self._period > 0):
            self._running = True

   def stop(self):
      self._running = False

   def restart(self):
      self._counter = 0
      self.start()

   def resume(self):
      if (self._period > 0):
         self._running = True

   def read(self):
      return self._counter

   def attachInterrupt(self, hook, period=-1):
      self._hook = hook;
      self.setPeriod(period)

   # Event handler
   def _eventHandler(self):
      self._counter+=1
      # Restart
      self.start()
      if (self._hook):
         self._hook()

   def poll(self):
      self._now = wiringpi.millis()
      if self._running:
         passed = self._now - self._time
         if passed > self._period:
            self._time += self._period
            self._eventHandler()
