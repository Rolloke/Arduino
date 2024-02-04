#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++17

QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined

unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy
TARGET = ArduinoSimulator
TEMPLATE = app


SOURCES += main.cpp \
           mainwindow.cpp \
           Arduino.cpp \
           serialinterface.cpp \
           WString.cpp \
           EEPROM.cpp \
           Print.cpp \
           LiquidCrystal.cpp \
           Stream.cpp \
           Wire.cpp \
           logic_analyser.cpp \
           logicview.cpp \
           logger.cpp \
           arduinoworker.cpp

HEADERS  += mainwindow.h \
            Arduino.h \
            serialinterface.h \
            WString.h \
            EEPROM.h \
            WProgram.h \
            unittest.h \
            Print.h \
            Printable.h \
            LiquidCrystal.h \
            Stream.h \
            Wire.h \
            logic_analyser.h \
            logicview.h \
            logger.h \
            arduinoworker.h

FORMS    += mainwindow.ui \
            logic_analyser.ui

ARDUINO_LIBRARY_PATH = /opt/arduino-1.8.16/libraries
ARDUINO_LOCAL_LIBRARY_PATH = ../libraries
