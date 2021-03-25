# MAX31855 Thermocouple ADC Driver

import spidev

def readValue() :
   spi = spidev.SpiDev()
   spi.open(0, 0)
   to_send = [0xFF, 0xFF, 0xFF, 0xFF]
   data = spi.xfer2(to_send)
   return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]

def readTemperatureRaw() :
   value = readValue()
   temp = (value >> 18) & 0x1FFF;
   if (value & 0x80000000) :
   	temp = -temp
   return temp

def readError() :
   value = readValue()
   return value & 0x07

def readTemperatureC() :
   value = readValue()
   temp = (value >> 18) & 0x1FFF;
   if (value & 0x80000000) :
   	temp = -temp
   return int(((temp * 50) + 1) / 20)

def readTemperatureF() :
   value = readValue()
   temp = (value >> 18) & 0x1FFF;
   if (value & 0x80000000) :
   	temp = -temp
   return int(((temp * 45) + 3201) / 10)
