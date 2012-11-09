import time
import serial
import datetime

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
	port='/dev/ttyUSB0',
	baudrate=4800,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE	
)

ser.open()
ser.isOpen()

temps=datetime.datetime.now()

ser.write('settime')
time.sleep(1)
ser.write('\r')
time.sleep(1)

ser.write(str(temps.hour))
time.sleep(1)
ser.write('\r')
time.sleep(1)

ser.write(str(temps.minute))
time.sleep(1)
ser.write('\r')
time.sleep(1)

ser.write(str(temps.second))
time.sleep(1)
ser.write('\r')




