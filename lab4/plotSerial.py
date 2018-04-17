import serial
import numpy
import matplotlib.pyplot as plt
from drawnow import *
import sys
leftrightA=[]
updownA=[]
dummyaxis=[]
ser = serial.Serial('/dev/ttyACM0', 9600)# timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) 
ser.flushInput()
ser.flushOutput()
plt.ion()

def dataPlot():
	plt.title("X-Y Co-ordinate data from Joystick")
	plt.grid(True)
	plt.ylabel("ASCII Values")
	plt.plot(leftrightA,updownA,'ro-', label="ASCII")
	plt.legend(loc='upper left')
	plt2=plt.twinx()
	plt.plot(updownA,'ro-', label="ASCII")

while True:
	while (ser.inWaiting()==0):
		pass
	#print "reading"
	dataStream = ser.read();
	#dataStream = ser.readline();
	#ser.read(dataStream)
	#print "hello"
	#print dataStream
	dataArray = dataStream.split()
	# print dataArray
	try:
		value = float(dataArray[0])
		leftrightA.append(value)
		updownA.append(value+1)
		#dummyaxis.append(2)
	except ValueError:
		pass

	#print dataArray[0]
	#print dataArray[1]
	#leftright = float ( dataArray[0] )
	#print leftright
	#updown = int ( dataArray[1] )
	#print leftright,",",updown
	#leftrightA.append(leftright)
	#updownA.append(updown)
	drawnow(dataPlot)

ser.close()
#print(leftrightA)
#print(updownA)
# plt.plot(leftrightA,updownA)
# drawnow(dataPlot)