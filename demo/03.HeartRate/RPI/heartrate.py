#!/usr/bin/python  
# -*- coding: utf-8 -*-  
# Filename: HeartRate.py  

import sys
import serial
import time

ISOTIMEFORMAT = '%Y-%m-%d %X'
 
ser = serial.Serial("/dev/ttyAMA0", baudrate=115200, bytesize=8, parity='N', stopbits=1, xonxoff=0, timeout=1)
if ser.isOpen() == False:
	ser.open()
	
f = file('heart_data.txt')  
while True:  
    line = f.readline()   
    if len(line) == 0:  
        break  
    print line,  
f.close() 

print("Reading is completion, Now is accepting new data!\n")

f = file('heart_data.txt', 'a+') 
while True:
	s = ser.read(1) 
	t = time.strftime(ISOTIMEFORMAT, time.localtime(time.time()))
	f.write(t+ " " + s + "\n")   
f.close() 