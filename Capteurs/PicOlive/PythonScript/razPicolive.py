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

ser.write('erase')
time.sleep(1)
ser.write('\r')

