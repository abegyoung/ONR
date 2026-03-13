#!/opt/local/bin/python2.7
from colorsys import rgb_to_hls, hls_to_rgb
from PyQt4 import QtGui, QtCore

from PyQt4.QtGui import QApplication, QWidget, QPainter, QGridLayout, QSizePolicy, QStyleOption
from PyQt4.QtCore import pyqtSignal, Qt, QSize, QTimer, QByteArray, QRectF, pyqtProperty
from PyQt4.QtSvg import QSvgRenderer
import QLed
from sys import argv, exit

execfile('QLedInclude.py')



class LedWidget(QWidget):
	def __init__(self, parent=None):
	  QWidget.__init__(self, parent)

	  self.vbl = QtGui.QGridLayout()
	  self.setLayout(self.vbl)
	  self.leds=[]

	  self.led=QLed(self, onColour=QLed.Green, shape=QLed.Circle)
	  self.vbl.addWidget(self.led, 1, 1, Qt.AlignCenter)
	  self.leds.append(self.led)
	  self.led=QLed(self, onColour=QLed.Green, shape=QLed.Circle)
	  self.vbl.addWidget(self.led, 2, 1, Qt.AlignCenter)
	  self.leds.append(self.led)
          self.led=QLed(self, onColour=QLed.Green, shape=QLed.Circle)
          self.vbl.addWidget(self.led, 3, 1, Qt.AlignCenter)
          self.leds.append(self.led)
          self.led=QLed(self, onColour=QLed.Green, shape=QLed.Circle)
          self.vbl.addWidget(self.led, 4, 1, Qt.AlignCenter)
          self.leds.append(self.led)
          self.led=QLed(self, onColour=QLed.Green, shape=QLed.Circle)
          self.vbl.addWidget(self.led, 5, 1, Qt.AlignCenter)
          self.leds.append(self.led)
          self.led=QLed(self, onColour=QLed.Green, shape=QLed.Circle)
          self.vbl.addWidget(self.led, 6, 1, Qt.AlignCenter)
          self.leds.append(self.led)
          self.led=QLed(self, onColour=QLed.Green, shape=QLed.Circle)
          self.vbl.addWidget(self.led, 7, 1, Qt.AlignCenter)
          self.leds.append(self.led)
          self.led=QLed(self, onColour=QLed.Green, shape=QLed.Circle)
          self.vbl.addWidget(self.led, 8, 1, Qt.AlignCenter)
          self.leds.append(self.led)
	    
	  #self.toggleLeds()

	def toggleLeds(self):
	   for led in self.leds: led.toggleValue()
	   QTimer.singleShot(1000, self.toggleLeds)

	def toggleTrue(self):
           for led in self.leds: led.valueTrue()

        def toggleFalse(self):
           for led in self.leds: led.valueFalse()

