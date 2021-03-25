# PCF8591 ADC/DAC Driver

import wiringpi

def readData(address, channel) :
   i2c = wiringpi.I2C()
   file = i2c.setupInterface("/dev/i2c-1", address)
   return i2c.readReg8(file, 0x40 + channel)

def writeData(address, value) :
   i2c = wiringpi.I2C()
   file = i2c.setupInterface("/dev/i2c-1", address)
   i2c.writeReg8(file, 0x40, value)
   return
