#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------
include (ArduinoSimulator.pro)

EMULATED=1
DEFINES += EMULATED

ARDUINO_PATH = ../Sketches/Joystick
INCLUDEPATH += $${ARDUINO_PATH}
#HEADERS += $${ARDUINO_PATH}/ioAt328.h
SOURCES += Joystick/Joystickino.cpp
DISTFILES += $${ARDUINO_PATH}/Joystick.ino

# Arduino Libraries
LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Button/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/button.cpp
HEADERS += $${LIBRARY1}/button.h

LIBRARY2 = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextTools/src
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/texttools.cpp
HEADERS += $${LIBRARY2}/texttools.h

#LIBRARY2 = $${ARDUINO_LIBRARY_PATH}/Time
#INCLUDEPATH += $${LIBRARY2}
#SOURCES += $${LIBRARY2}/Time.cpp
#HEADERS += $${LIBRARY2}/Time.h
#SOURCES += $${LIBRARY2}/DateStrings.cpp

