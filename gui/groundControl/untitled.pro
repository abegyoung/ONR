#-------------------------------------------------
#
# Project created by QtCreator 2015-11-04T15:20:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

include (/usr/local/qwt-6.1.3-svn/features/qwt.prf)
