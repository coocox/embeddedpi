import sys
import serial
import time
import glob
import pyxine
xine = pyxine.Xine()
stream = xine.stream_new()

mp3s = []
mp3s = glob.glob('./*.MP3')

mp3idx = 10
prev_input = 1

#ser = serial.Serial("/dev/ttyAMA0", 115200)
ser = serial.Serial("/dev/ttyAMA0", baudrate=115200, bytesize=8, parity='N', stopbits=1, xonxoff=0, timeout=1)
if ser.isOpen() == False:
	ser.open()

while True:
	ser.write("RPI")
	
#	print("Enter ch")
	data = ser.read(1)
	ser.write(data)
	
	if data == '0':
		mp3idx = 0
	if data == '1':
		mp3idx = 1
	if data == '2': 
		mp3idx = 2
	if data == '3': 
		mp3idx = 3
	if data == '4': 
		mp3idx = 4
	if data == '5': 
		mp3idx = 5
	if data == '6': 
		mp3idx = 6
	if data == '7':
		mp3idx = 7
	if data == '8':
		mp3idx = 8
	if mp3idx < 9:
		print("Playing " + data + " " + mp3s[mp3idx])
		stream.open(mp3s[mp3idx])
		stream.play()
		
	mp3idx = 10
	#time.sleep(1.5)
