#!/usr/bin/env python
from PyQt4 import QtCore, QtGui

class PyAnalogClock(QtGui.QWidget):
    # Emitted when the clock's time changes.
    timeChanged = QtCore.pyqtSignal(QtCore.QTime)

    def __init__(self, parent=None):

        super(PyAnalogClock, self).__init__(parent)

        timer = QtCore.QTimer(self)
        timer.timeout.connect(self.update)
        timer.timeout.connect(self.updateTime)
        timer.start(1000)

        self.resize(200, 200)

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

    def paintEvent(self, event):

        side = min(self.width(), self.height())
        time = QtCore.QTime.currentTime()

        painter = QtGui.QPainter()
        painter.begin(self)
        painter.setRenderHint(QtGui.QPainter.Antialiasing)
        painter.translate(self.width() / 2, self.height() / 2)
        painter.scale(side / 200.0, side / 200.0)

        painter.save()
        painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)))
        painter.drawConvexPolygon(self.hourHand)
        painter.restore()

        for i in range(0, 12):
            painter.drawLine(88, 0, 96, 0)
            painter.rotate(30.0)

        painter.save()
        painter.rotate(6.0 * (time.minute() + time.second() / 60.0))
        painter.drawConvexPolygon(self.minuteHand)
        painter.restore()

        for j in range(0, 60):
            if (j % 5) != 0:
                painter.drawLine(92, 0, 96, 0)
            painter.rotate(6.0)

        painter.end()

    def updateTime(self):

        self.timeChanged.emit(QtCore.QTime.currentTime())

if __name__ == "__main__":

    import sys

    app = QtGui.QApplication(sys.argv)
    clock = PyAnalogClock()
    clock.show()
    sys.exit(app.exec_())
