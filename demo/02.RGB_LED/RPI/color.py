#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
ZetCode PyQt4 tutorial 

In this example, we create three toggle buttons.
They will control the background color of a 
QtGui.QFrame. 

author: Jan Bodnar
website: zetcode.com 
last edited: September 2011
"""

import sys
import serial
from PyQt4 import QtGui, QtCore

class CommandInterface:
    def open(self, aport='/dev/ttyAMA0', abaudrate=115200) :
        self.sp = serial.Serial(
            port=aport,
            baudrate=abaudrate,     # baudrate
            bytesize=8,             # number of databits
            parity=serial.PARITY_EVEN,
            stopbits=1,
            xonxoff=0,              # enable software flow control
            rtscts=0,               # disable RTS/CTS flow control
            timeout=5               # set a timeout value, None for waiting forever
        )
        
    def write(self, cmd):
        self.sp.write(chr(cmd))

class Example(QtGui.QWidget):


    
    def __init__(self):
        super(Example, self).__init__()
        
        self.initUI()
        
    def initUI(self):   

        conf = {
                'port': '/dev/ttyAMA0',
                'baud': 115200,
                'address': 0x08000000,
                'erase': 0,
                'write': 0,
                'verify': 0,
                'read': 0,
            }    

        self.col = QtGui.QColor(0, 0, 0)  
        self.uart = CommandInterface()
        self.uart.open(conf['port'], conf['baud'])
		
        RedLabel=QtGui.QLabel("Red",self)
        RedLabel.setGeometry(60, 20, 30, 10)
        
        BlueLabel=QtGui.QLabel("Blue",self)
        BlueLabel.setGeometry(55, 60, 40, 10)
        
        GreenLabel=QtGui.QLabel("Green",self)
        GreenLabel.setGeometry(50, 100, 50, 10)
       
        
        Redsld = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        Redsld.setFocusPolicy(QtCore.Qt.NoFocus)
        Redsld.setGeometry(100, 20, 260, 30)
        Redsld.valueChanged[int].connect(self.RedchangeValue)	

        Bluesld = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        Bluesld.setFocusPolicy(QtCore.Qt.NoFocus)
        Bluesld.setGeometry(100, 60, 260, 30)
        Bluesld.valueChanged[int].connect(self.BluechangeValue)	 

        Greensld = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        Greensld.setFocusPolicy(QtCore.Qt.NoFocus)
        Greensld.setGeometry(100, 100, 260, 30)
        Greensld.valueChanged[int].connect(self.GreenchangeValue)	

        Redsld.setRange(0,255) 
        Bluesld.setRange(0,255) 
        Greensld.setRange(0,255) 

        Redsld.setObjectName("Red")
        Bluesld.setObjectName("Blue")
        Greensld.setObjectName("Green")
        
        GreenLabel=QtGui.QLabel("Color show below:",self)
        GreenLabel.setGeometry(100, 180, 200, 10)
        
        self.square = QtGui.QFrame(self)
        self.square.setGeometry(100, 200, 250, 250)
        self.square.setStyleSheet("QWidget { background-color: %s }" %  
            self.col.name())
        
        self.setGeometry(500, 500, 500, 500)
        self.setWindowTitle('color set')
        self.show()
            
    def RedchangeValue(self, value):
        if value >255:
            value =0
        print("Red value = %(value)d " % {'value': value})    
        self.col.setRed(value) 
        self.uart.write(165)
        self.uart.write(64)
        self.uart.write(5)
        self.uart.write(1)
        self.uart.write(1)
        self.uart.write(value)
        self.uart.write(90)
        self.square.setStyleSheet("QFrame { background-color: %s }" %
            self.col.name())  

    def BluechangeValue(self, value):
        if value >255:
            value =0
        print("Blue value = %(value)d " % {'value': value})     
        self.col.setBlue(value)
        self.uart.write(165)
        self.uart.write(64)
        self.uart.write(5)
        self.uart.write(3)
        self.uart.write(1)
        self.uart.write(value)
        self.uart.write(90)
        self.square.setStyleSheet("QFrame { background-color: %s }" %
            self.col.name())  
        
    def GreenchangeValue(self, value):
        if value >255:
            value =0
        print("Green value = %(value)d " % {'value': value})      
        self.col.setGreen(value)
        self.uart.write(165)
        self.uart.write(64)
        self.uart.write(5)
        self.uart.write(2)
        self.uart.write(1)
        self.uart.write(value)
        self.uart.write(90)  
        self.square.setStyleSheet("QFrame { background-color: %s }" %
            self.col.name())  
        
def main():
    
    app = QtGui.QApplication(sys.argv)
    
    ex = Example()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()