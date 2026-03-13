#!/opt/local/bin/python2.7
from colorsys import rgb_to_hls, hls_to_rgb
from PyQt4.QtGui import QApplication, QWidget, QPainter, QGridLayout, QSizePolicy, QStyleOption
from PyQt4.QtCore import pyqtSignal, Qt, QSize, QTimer, QByteArray, QRectF, pyqtProperty
from PyQt4.QtSvg import QSvgRenderer
import QLed
from sys import argv, exit

execfile('QLedInclude.py')


class Test(QWidget):
	def __init__(self, parent=None):
	   QWidget.__init__(self, parent)
	   self.setWindowTitle("QLed Test")
	   _l=QGridLayout()
	   self.setLayout(_l)
	   self.leds=[]
	   led=QLed(self, onColour=1, shape=1)
	   _l.addWidget(led, 1, 1, Qt.AlignCenter)
	   self.leds.append(led)

	   self.toggleLeds()

	def toggleLeds(self):
	   for led in self.leds: led.toggleValue()
	   QTimer.singleShot(1000, self.toggleLeds)

a=QApplication(argv)
t=Test()
t.show()
t.raise_()
exit(a.exec_())

