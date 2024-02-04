#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------

include (ArduinoSimulator.pro)

EMULATED=1
DEFINES += EMULATED

# Aduino Project Files
SKETCH_NAME = OctansSimulatorDisplay
ARDUINO_PATH = ../Sketches/OctansSimulator/$${SKETCH_NAME}
INCLUDEPATH += $${ARDUINO_PATH}
SOURCES += OctanSimulator/OctansSimulatorino.cpp
SOURCES += $${ARDUINO_PATH}/$${SKETCH_NAME}.ino
SOURCES += $${ARDUINO_PATH}/MPU6050_control.cpp
HEADERS += $${ARDUINO_PATH}/MPU6050_control.h
SOURCES += $${ARDUINO_PATH}/OctansSimulatorMenu.cpp
HEADERS += $${ARDUINO_PATH}/OctansSimulatorMenu.h


# Libraries
LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextTools/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/texttools.cpp
HEADERS += $${LIBRARY1}/texttools.h

LIBRARY2 = $${ARDUINO_LIBRARY_PATH}/Time
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/Time.cpp
SOURCES += $${LIBRARY2}/DateStrings.cpp
HEADERS += $${LIBRARY2}/Time.h

# Libraries
LIBRARY3 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Button/src
INCLUDEPATH += $${LIBRARY3}
SOURCES += $${LIBRARY3}/button.cpp
HEADERS += $${LIBRARY3}/button.h

LIBRARY4 = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextMenu/src
INCLUDEPATH += $${LIBRARY4}
SOURCES += $${LIBRARY4}/menu.cpp
HEADERS += $${LIBRARY4}/menu.h

#LIBRARY5 = $${ARDUINO_LOCAL_LIBRARY_PATH}/MPU6050
#INCLUDEPATH += $${LIBRARY5}
#SOURCES += $${LIBRARY5}/MPU6050.cpp
#HEADERS += $${LIBRARY5}/MPU6050.h
#HEADERS += $${LIBRARY5}/helper_3dmath.h
#HEADERS += $${LIBRARY5}/MPU6050_6Axis_MotionApps20.h

