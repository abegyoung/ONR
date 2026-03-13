#!/opt/local/bin/python
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


# added BJS#################################
import argparse
from serial.tools.list_ports import comports
def __searchForDeviceID(deviceid):
  ## pass '2303' or an the FTDI serial string (do not use more than one PL2303 in your system!!)
  ## returns device path
  portmatches = [info[0] for info in comports() if deviceid in info[2]]
  if len(portmatches) > 0: 
    return  portmatches[0]
  return None

PORTUP_DFLT = '/dev/tty.usbmodem8491', 
PORTDN_DFLT = '/dev/tty.usbserial-A105Z981'

parser = argparse.ArgumentParser()
parser.add_argument("-u", "--uplinkid", help="UPlink USB/Serial ID info to search for", 
                    default=PORTUP_DFLT)
parser.add_argument("-d", "--dnlinkid", help="DOWNlink USB/Serial ID info to search for", 
                    default=PORTDN_DFLT)
args = parser.parse_args()
PORTUP = __searchForDeviceID(args.uplinkid)
PORTDN = __searchForDeviceID(args.dnlinkid)
print "Uplink    --  using device at", PORTUP
print "Downlink  --  using device at", PORTDN
# added BJS#################################




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
    if len(total_data)>4:
      # check if end_of_data was split
      last_pair=total_data[-5]+total_data[-4]+total_data[-3]+total_data[-2]+total_data[-1]
      if End in last_pair:
        total_data[-5]=last_pair[:last_pair.find(End)]
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
                self.radioBtn.clicked.connect(self.radioBtn_clicked)
                self.genBtn.clicked.connect(self.genBtn_clicked)
                self.txServerRestart.clicked.connect(self.txServerRestart_clicked)
                self.cipCommRestart.clicked.connect(self.cipCommRestart_clicked)
                self.cpuReboot.clicked.connect(self.cpuReboot_clicked)

		self.timer = QtCore.QTimer(self)
		self.timer.setInterval(250)
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
            data = read_end(DnSer, 'EOF\n')
            print data
            data = data.splitlines()

            self.textRawHK.setText(data[0])

            line = data[1].split()
            if(line[0] == 'UDP:'):
              global heading
              heading = float(line[1])
              self.update()
              self.textEdit_Lat.setText("{:.3f}".format(float(line[4])))
              self.textEdit_Lon.setText("{:.3f}".format(float(line[5])))

            line = data[2].split()
	    if(line[0] == 'PWR:'):
              pwr = int(line[3])
              for i in range(0,8):
                if (pwr>>i)&1:
                  self.led.leds[i].valueTrue()
                else:
                  self.led.leds[i].valueFalse()

            line = data[3].split()
	    if(line[0] == 'ENV:'):
              temp1 = "{:.2f}".format(float(line[1]))
              temp2 = "{:.2f}".format(float(line[2]))
              self.textEdit_T1.setText(str(temp1))
              self.textEdit_T2.setText(str(temp2))

            line = data[4].split()
	    if(line[0] == 'MODE:'):
              self.point_0.setStyleSheet('color: black')
              self.point_1.setStyleSheet('color: black')
              self.point_2.setStyleSheet('color: black')
              self.radioBtn.setStyleSheet('color: black')
              if(line[1]=='0'): self.point_0.setStyleSheet('color: red')
              if(line[1]=='1'): self.point_1.setStyleSheet('color: red')
              if(line[1]=='2'): self.point_2.setStyleSheet('color: red')
              if(line[2]=='1'): self.radioBtn.setStyleSheet('background-color: green')

            line = data[5].split()
	    if(line[0] == 'WARN:'):
              self.PAtempLabel.setStyleSheet('color: black')
              if(len(line) > 1):
                if(line[3] == "hot."):
                  line=line[3:len(line)]
                  self.PAtempLabel.setStyleSheet('color: red')
              if(len(line) > 1):
                self.textEdit.append(str(' '.join(line[1:len(line)])))

            line = data[6].split()
	    if(line[0] == 'VCO:'):
              freq = "{:.3f}".format(float(line[1]))
              self.textEdit_freq.setText(str(freq))
          
        def startBtn_clicked(self):
          global DnSer
          global UpSer
          DnSer = serial.Serial(
            port=PORTDN, #'/dev/tty.usbserial-A105Z981',   #Placed this as default for cmd line options --BJS
            baudrate=9600,
            timeout=1
          )
          UpSer = serial.Serial(
            port=PORTUP, #'/dev/tty.usbmodem8491',   #Placed this as default for cmd line options --BJS
            baudrate=1200, #9600,
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
          cmd = "S004C 01KS004C 01KS004C 01K\r\n"  #"0\n" ## Bit 0 == 004C
          UpSer.write(cmd)

        def powerBtn_2_clicked(self):
          global UpSer
          cmd = "S004D 01KS004D 01KS004D 01K\r\n"  #"1\n" ## Bit 1 == 004D
          UpSer.write(cmd)

        def powerBtn_3_clicked(self):
          global UpSer
          cmd = "S004E 01KS004E 01KS004E 01K\r\n"  #"2\n" ## Bit 2 == 004E
          UpSer.write(cmd)

        def powerBtn_4_clicked(self):
          global UpSer
          cmd = "S004F 01KS004F 01KS004F 01K\r\n"  #"3\n" ## Bit 3 == 004F
          UpSer.write(cmd)

        def powerBtn_5_clicked(self):
          global UpSer
          cmd = "S0050 01KS0050 01KS0050 01K\r\n"  #"4\n" ## Bit 4 == 0050
          UpSer.write(cmd)

        def powerBtn_6_clicked(self):
          global UpSer
          cmd = "S0051 01KS0051 01KS0051 01K\r\n"  #"5\n" ## Bit 5 == 0051
          UpSer.write(cmd)

        def powerBtn_7_clicked(self):
          global UpSer
          cmd = "S0052 01KS0052 01KS0052 01K\r\n"  #"6\n" ## Bit 6 == 0052
          UpSer.write(cmd)

        def powerBtn_8_clicked(self):
          global UpSer
          cmd = "S0053 01KS0053 01KS0053 01K\r\n"  #"7\n" ## Bit 7 == 0053
          UpSer.write(cmd)

        def nextModeBtn_clicked(self):
          global UpSer
          cmd = "S0054 01KS0054 01KS0054 01K\r\n"  #"8\n" ## Bit 8 == 0054
          UpSer.write(cmd)

        def tuneUpBtn_clicked(self):
          global UpSer
          cmd = "S0055 01KS0055 01KS0055 01K\r\n"  #"9\n" ## Bit 0 == 004C
          UpSer.write(cmd)

        def tuneDnBtn_clicked(self):
          global UpSer
          cmd = "S0056 01KS0056 01KS0056 01K\r\n"  #"10\n" ## Bit 0 == 004C
          UpSer.write(cmd)

        def radioBtn_clicked(self):
          global UpSer
          cmd = "S0057 01KS0057 01KS0057 01K\r\n"  #"11\n" ## Bit11 == 0057
          UpSer.write(cmd)

        def genBtn_clicked(self):
          global UpSer
          cmd = "S0058 01KS0058 01KS0058 01K\r\n"  #"12\n" ## Bit12 == 0058
          UpSer.write(cmd)

        def txServerRestart_clicked(self):
          global UpSer
          cmd = "S005A 01KS005A 01KS005A 01K\r\n"  #"13\n" ## Bit13 == 005A
          UpSer.write(cmd)

        def cipCommRestart_clicked(self):
          global UpSer
          cmd = "S005B 01KS005B 01KS005B 01K\r\n"  #"14\n" ## Bit14 == 005B
          UpSer.write(cmd)

        def cpuReboot_clicked(self):
          global UpSer
          cmd = "S005C 01KS005C 01KS005C 01K\r\n"  #"15\n" ## Bit15 == 005C
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
          painter.rotate( -heading )

          #Make hour ticks
          painter.drawConvexPolygon(self.minuteHand)
          painter.drawText(0,-80,'N')
          painter.drawText(0,80,'S')
          painter.drawText(80,0,'E')
          painter.drawText(-80,0,'W')
          painter.drawText(25,-65,'30')
          painter.drawText(55,-35,'60')
          painter.drawText(55,35,'120')
          painter.drawText(25,65,'150')
          painter.drawText(-40,65,'210')
          painter.drawText(-70,35,'240')
          painter.drawText(-70,-35,'300')
          painter.drawText(-40,-65,'330')
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

