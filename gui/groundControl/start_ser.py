#!/usr/bin/python2.7
import sys
import time
import serial
from string import split
from StringIO import StringIO
from PyQt4 import QtCore, QtGui, uic

import QLed
from sys import argv, exit
from PyQt4 import QtGui, QtCore
from PyQt4.QtGui import QApplication, QWidget, QPainter, QGridLayout, QSizePolicy, QStyleOption
from PyQt4.QtCore import pyqtSignal, Qt, QSize, QTimer, QByteArray, QRectF, pyqtProperty
from PyQt4.QtSvg import QSvgRenderer
from colorsys import rgb_to_hls, hls_to_rgb

execfile('QLedInclude.py')

from PyQt4.Qwt5.qplt import *

form_class = uic.loadUiType("mainwindow.ui")[0]

UpSer = 0
DnSer = 0
heading = 0

def read_end(the_port, End):
  total_data=[];data=''
  while True:
    try:
      data=the_port.read(1)
    except:
      break
    if End in data:
      total_data.append(data[:data.find(End)])
      break
    total_data.append(data)
    if len(total_data)>1:
      # check if end_of_data was split
      last_pair=total_data[-2]+total_data[-1]
      if End in last_pair:
        total_data[-2]=last_pair[:last_pair.find(End)]
        total_data.pop()
        break
  return ''.join(total_data)

class MyWindowClass(QtGui.QMainWindow, form_class):
	def __init__(self, parent=None):
		QtGui.QWidget.__init__(self, parent)
		self.setupUi(self)
		self.startBtn.clicked.connect(self.startBtn_clicked)
		self.stopBtn.clicked.connect(self.stopBtn_clicked)
		self.powerBtn_1.clicked.connect(self.powerBtn_1_clicked)# setpower vco55
		self.powerBtn_2.clicked.connect(self.powerBtn_2_clicked)# setpower vco15
		self.powerBtn_3.clicked.connect(self.powerBtn_3_clicked)# setpower pa1
		self.powerBtn_4.clicked.connect(self.powerBtn_4_clicked)# setpower pa2 
		self.powerBtn_5.clicked.connect(self.powerBtn_5_clicked)# setpower msb
		self.powerBtn_6.clicked.connect(self.powerBtn_6_clicked)# setpower lsb
		self.powerBtn_7.clicked.connect(self.powerBtn_7_clicked)# setpower usevco
		self.powerBtn_8.clicked.connect(self.powerBtn_8_clicked)# setpower pa3

		self.cpuReboot.clicked.connect(self.cpuRebootBtn_clicked)#reboot cpu
		self.cipCommRestart.clicked.connect(self.cipCommRestartBtn_clicked)# restart cip server
		self.txServerRestart.clicked.connect(self.txServerRestartBtn_clicked)# restart tx serer
		self.antennaMode.clicked.connect(self.antennaModeBtn_clicked)# Reserved

		self.radioBtn.clicked.connect(self.radioBtn_clicked)# start CC1100 transmitting
		self.tuneUpBtn.clicked.connect(self.tuneUpBtn_clicked)# setfreq dwn
		self.tuneDnBtn.clicked.connect(self.tuneDnBtn_clicked)# setfreq up
		self.nextModeBtn.clicked.connect(self.nextModeBtn_clicked)# pointing mode (lighthouse, GS1, GS2)

		self.timer = QtCore.QTimer(self)
		self.timer.setInterval(10)
		self.timer.timeout.connect(self.blink)
		#self.timer.timeout.connect(self.update)

		self.hourHand = QtGui.QPolygon([
		QtCore.QPoint(7, 8),
		QtCore.QPoint(-7, 8),
		QtCore.QPoint(0, -40)
		])
		self.minuteHand = QtGui.QPolygon([
		QtCore.QPoint(7, 8),
		QtCore.QPoint(-7, 8),
		QtCore.QPoint(0, -70)
		])

	def start(self):
	  self.timer.start()

	def stop(self):
	  self.timer.stop()

        @QtCore.pyqtSlot()
        def blink(self):
          if (DnSer.isOpen() and DnSer.inWaiting()>0) :
            data = read_end(DnSer, '\n')
            print data
            if(data.split()[0]=='TIME:'): 
              self.textRawHK.setText(data)
            data = data.split()
            if(data[0]=='UDP:'): 
              telesX = float(data[1])
              telesY = float(data[2])
              telesZ = float(data[3])
              global heading
              heading = telesX
              self.update()
              self.textEdit_Lat.setText("{:.3f}".format(float(data[4])))
              self.textEdit_Lon.setText("{:.3f}".format(float(data[5])))
            if(data[0]=='PWR:'): 
              pwr = int(data[3])
              for i in range(0,8):
                if (pwr>>i)&1:
                  self.led.leds[i].valueTrue()
                else:
                  self.led.leds[i].valueFalse()
            if(data[0]=='ENV:'): 
              temp1 = "{:.2f}".format(float(data[1]))
              temp2 = "{:.2f}".format(float(data[2]))
              self.textEdit_T1.setText(str(temp1))
              self.textEdit_T2.setText(str(temp2))
            if(data[0]=='VCO:'): 
              freq = "{:.3f}".format(float(data[1]))
              self.textEdit_freq.setText(str(freq))
            if(data[0]=='WARN:'):
              self.PAtempLabel.setStyleSheet('color: black')
              if(len(data) > 1):
                if(data[3] == "hot."):
                  data=data[3:len(data)]
                  self.PAtempLabel.setStyleSheet('color: red')
              if(len(data) > 1):
                self.textEdit.append(str(' '.join(data[1:len(data)])))
          
        def startBtn_clicked(self):
          global DnSer
          global UpSer
          DnSer = serial.Serial(
            port='/dev/ttyUSB0',
            baudrate=9600,
            timeout=1
          )
          UpSer = serial.Serial(
            port='/dev/ttyACM0',
            baudrate=9600,
            timeout=1
          )
          self.timer.start()

        def stopBtn_clicked(self):
          global DnSer
          global UpSer
          self.timer.stop()
          DnSer.close()
          DnSer = 0
          UpSer.close()
          UpSer = 0

        def powerBtn_1_clicked(self):
          global UpSer
          cmd = "0\r"
          UpSer.write(cmd)

        def powerBtn_2_clicked(self):
          global UpSer
          cmd = "1\r"
          UpSer.write(cmd)

        def powerBtn_3_clicked(self):
          global UpSer
          cmd = "2\r"
          UpSer.write(cmd)

        def powerBtn_4_clicked(self):
          global UpSer
          cmd = "3\r"
          UpSer.write(cmd)

        def powerBtn_5_clicked(self):
          global UpSer
          cmd = "4\r"
          UpSer.write(cmd)

        def powerBtn_6_clicked(self):
          global UpSer
          cmd = "5\r"
          UpSer.write(cmd)

        def powerBtn_7_clicked(self):
          global UpSer
          cmd = "6\r"
          UpSer.write(cmd)

        def powerBtn_8_clicked(self):
          global UpSer
          cmd = "7\r"
          UpSer.write(cmd)




	def cpuRebootBtn_clicked(self):
          global UpSer
          cmd = "8\r"
          UpSer.write(cmd)

	def cipCommRestartBtn_clicked(self):
          global UpSer
          cmd = "9\r"
          UpSer.write(cmd)

	def txServerRestartBtn_clicked(self):
          global UpSer
          cmd = "10\r"
          UpSer.write(cmd)

	def antennaModeBtn_clicked(self):
          global UpSer
          cmd = "11\r"
          UpSer.write(cmd)

	def radioBtn_clicked(self):
          global UpSer
          cmd = "12\r"
          UpSer.write(cmd)

	def tuneDnBtn_clicked(self):
          global UpSer
          cmd = "13\r"
          UpSer.write(cmd)

	def tuneUpBtn_clicked(self):
          global UpSer
          cmd = "14\r"
          UpSer.write(cmd)

	def nextModeBtn_clicked(self):
          global UpSer
          cmd = "15\r"
          UpSer.write(cmd)




        def paintEvent(self, event):
          global heading

          side = min(150,150)

          painter = QtGui.QPainter()
          painter.begin(self)
          painter.setRenderHint(QtGui.QPainter.Antialiasing)
          painter.translate(225, 150)
          painter.scale(side / 200.0, side / 200.0)

          painter.drawConvexPolygon(self.hourHand)
          painter.rotate( heading )

          #Make hour ticks
          painter.drawText(0,-80,'N')
          painter.drawText(0,80,'S')
          painter.drawText(80,0,'E')
          painter.drawText(-80,0,'W')
          painter.drawText(25,-65,'30')
          painter.drawText(55,-35,'60')
          painter.drawText(25,65,'120')
          painter.drawText(55,35,'150')
          painter.drawText(-40,65,'210')
          painter.drawText(-70,35,'240')
          painter.drawText(-40,-65,'300')
          painter.drawText(-70,-35,'330')
          for i in range(0, 12):
            painter.drawLine(88, 0, 96, 0)
            painter.rotate(30.0)

          #Make minute ticks
          for j in range(0, 60):
            if (j % 5) != 0:
              painter.drawLine(92, 0, 96, 0)
            painter.rotate(6.0)

          painter.end()

        #def updateTime(self):
        #  pass

def main():
  app = QtGui.QApplication(sys.argv)
  myWindow = MyWindowClass(None)
  myWindow.show()

  app.exec_()
  
if __name__ == '__main__':
  main()

