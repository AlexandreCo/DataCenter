import time
import serial

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
	port='/dev/ttyUSB0',
	baudrate=4800,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE	
)

ser.open()
ser.isOpen()
# send the character to the device
# (note that I happend a \r\n carriage return and line feed to the characters - this is requested by my device)
ser.write('read')
time.sleep(1)
ser.write('\r')
out = ''
# let's wait one second before reading output (let's give device time to answer)
time.sleep(1)
while ser.inWaiting() > 0:
	out += ser.read(1)
	time.sleep(0.001)
print out

