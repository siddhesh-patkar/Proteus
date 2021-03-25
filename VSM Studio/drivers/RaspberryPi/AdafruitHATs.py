#Motor Hat
from Adafruit_MotorHAT import Adafruit_MotorHAT, Adafruit_DCMotor
import atexit
#Servo Hat
import Adafruit_PCA9685

class Adafruit_DC_AND_Stepper_HAT:  
   def __init__(self):
      try:
         self.setAddress(0x60)
      except:
         print("You need to change the HAT Address")
         sys.quit()
      self.myStepper = [None,None]

      
   def setAddress(self,Address):
      self.mh = Adafruit_MotorHAT(addr=Address)

   def setDcMotor(self,motor,direction,speed):
      
      if direction == 2:
         self.mh.getMotor(motor).run(Adafruit_MotorHAT.FORWARD)
      elif direction == 0:
         self.mh.getMotor(motor).run(Adafruit_MotorHAT.BACKWARD)
      else:
         self.mh.getMotor(motor).run(Adafruit_MotorHAT.RELEASE)
      
      myMotor = self.mh.getMotor(motor)
      speed = speed*255
      myMotor.setSpeed(int(speed))
      
   def setUpStepper(self,steps,portNum):
      self.myStepper[portNum-1] = self.mh.getStepper(steps,portNum)
      
   def stepStepper(self,steps,dir,type,portNum):    
       if dir == 2:
         direction = 2 # backward
       elif dir == 1:
         direction = 4 # release
       elif dir == 0:
         direction = 1 # forward
         # 3 = break
       self.myStepper[portNum-1].step(steps, direction, (type + 1))
         
class Adafruit_16_Channel_Servo_HAT:
   def __init__(self):    
      try:
         self.setAddress(0x40)
      except:
         print("You need to change the HAT Address")
         sys.quit()
         pass
      
      #Creates an array of which to store all the max and min values for the servo motors
      self.maxMinAngle = []
      self.maxMinPulse = []
      for x in range(0,17):
         self.maxMinAngle.append([90,-90])
         self.maxMinPulse.append([491,245])

   def setAddress(self,Address):
      self.pwm = Adafruit_PCA9685.PCA9685(Address)
      
   def setMaxMinAngle(self,servo,max,min):
      self.maxMinAngle[servo][0] = max
      self.maxMinAngle[servo][1] = min
      
   def setMaxMinPulse(self,servo,max,min):
      self.maxMinPulse[servo][0] = max*245.5
      self.maxMinPulse[servo][1] = min*245.5
      
   def setAngle(self,servo,angle):
      #sets the frequency
      self.pwm.set_pwm_freq(60)
      #Pulse Max = 491 Pulse Min = 254
      range = self.maxMinAngle[servo][0] - self.maxMinAngle[servo][1]
      
      #Prevents motors from grinding
      if angle > range/2:
         angle = range/2
      elif -angle > range/2:
         angle = -(range/2)
      
      total = self.maxMinPulse[servo][0] - self.maxMinPulse[servo][1]
   
      pulse = self.maxMinPulse[servo][1]+(((angle+range/2)/range)*total)
      self.pwm.set_pwm(servo, 0, int(pulse))
   