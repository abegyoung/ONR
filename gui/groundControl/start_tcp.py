#!/usr/bin/python2.7
import sys
import time
import socket
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

Upport = 9801
Dnport = 9802
serverip = '10.130.133.108'
UpTCP = 0
DnTCP = 0

heading = 0

def recv_end(the_socket, End):
  total_data=[];data=''
  while True:
    try:
      data=the_socket.recv(8192)
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
                self.nextModeBtn.clicked.connect(self.nextModeBtn_clicked)
                self.powerBtn_1.clicked.connect(self.powerBtn_1_clicked)
                self.powerBtn_2.clicked.connect(self.powerBtn_2_clicked)
                self.powerBtn_3.clicked.connect(self.powerBtn_3_clicked)
                self.powerBtn_4.clicked.connect(self.powerBtn_4_clicked)
                self.powerBtn_5.clicked.connect(self.powerBtn_5_clicked)
                self.powerBtn_6.clicked.connect(self.powerBtn_6_clicked)
                self.powerBtn_7.clicked.connect(self.powerBtn_7_clicked)
                self.powerBtn_8.clicked.connect(self.powerBtn_8_clicked)
                self.tuneUpBtn.clicked.connect(self.tuneUpBtn_clicked)
                self.tuneDnBtn.clicked.connect(self.tuneDnBtn_clicked)

		self.timer = QtCore.QTimer(self)
		self.timer.setInterval(1)
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
          if (1) :
            data = recv_end(DnTCP, '\n')
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
          global DnTCP
          global UpTCP
          DnTCP=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
          DnTCP.connect((serverip, Dnport))
          UpTCP=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
          UpTCP.connect((serverip, Upport))

          self.timer.start()

        def stopBtn_clicked(self):
          global DnTCP
          global UpTCP
          self.timer.stop()
          DnTCP.close()
          DnTCP = 0
          UpTCP.close()
          UpTCP = 0

        def powerBtn_1_clicked(self):
          global UpTCP
          cmd = "0"
          UpTCP.send(cmd)

        def powerBtn_2_clicked(self):
          global UpTCP
          cmd = "1"
          UpTCP.send(cmd)

        def powerBtn_3_clicked(self):
          global UpTCP
          cmd = "2"
          UpTCP.send(cmd)

        def powerBtn_4_clicked(self):
          global UpTCP
          cmd = "3"
          UpTCP.send(cmd)

        def powerBtn_5_clicked(self):
          global UpTCP
          cmd = "4"
          UpTCP.send(cmd)

        def powerBtn_6_clicked(self):
          global UpTCP
          cmd = "5"
          UpTCP.send(cmd)

        def powerBtn_7_clicked(self):
          global UpTCP
          cmd = "6"
          UpTCP.send(cmd)

        def powerBtn_8_clicked(self):
          global UpTCP
          cmd = "7"
          UpTCP.send(cmd)

        def nextModeBtn_clicked(self):
          global UpTCP
          cmd = "8"
          UpTCP.send(cmd)

        def tuneUpBtn_clicked(self):
          global UpTCP
          cmd = "9"
          UpTCP.send(cmd)

        def tuneDnBtn_clicked(self):
          global UpTCP
          cmd = "10"
          UpTCP.send(cmd)

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

