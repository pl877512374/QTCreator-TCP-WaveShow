#-------------------------------------------------
#
# Project created by QtCreator 2019-08-15T20:41:27
#
#-------------------------------------------------

QT       += core gui printsupport
QT       += network

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       +=charts
TARGET = TCPTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    workthread.cpp \
    functool.cpp \
    chartview.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    workthread.h \
    functool.h \
    chartview.h \
    qcustomplot.h

FORMS    += mainwindow.ui
