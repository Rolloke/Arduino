# Arduino
This repository contains Arduino libraries an sketches

### *libraries*

- Button: a button implementation for asynchronous detection of triggered events like<br>(pressed, released, delayed, repeated)
- Melody: a melody playing library that is controlled asynchronously
- PinController: library to control digital and analog pins.
- TextMenu: a menu for editing value, selection of list item, command execution that my contain also submenus
- TextTools: some text functions for parsing and printing

### *Sketches*

- ConfigurableDelayTimer
- Joystick
- OctansSimulator

### *Simulator* 

Qt-project for simulation, testing and debugging of Arduino Sketches  
It is not complete and supports what was needed so far for the developed sketches and libraries. 

- Basic Arduino Simulator  
  - Project: ArduinoSimulator.pro 
  - Contains some basic Arduino emulation devices
    - Arduino: provides basic arduino functions and defines
    - Wire: TWI/I2C library for Wiring & Arduino
    - Stream: parsing methods for Stream class
    - WString: String library
    - Print: Base class that provides print() and println()
    - EEPROM: emulates EEPROM Device using a file
    - SerialInterface: emulates Serial interface in Qt-Simulator App
    - LiquidCrystal: emulates LCD Diplay  in Qt-Simulator App
    - Logic_analyser: shows pin states graphically

- The Projects include ArduinoSimulator.pro to emulate the Arduino device  
  - The ino connction ccp-file contains also functions called by the Qt-Simulator-App
    - UI-Initalization: void initialize(Ui_MainWindow* ui)
    - Unittest: void doTest()
  - The pro-File of the simulated sketch enhances the Qt-Project by
    - INCLUDEPATH (to Arduino sketch and libraries)
    - HEADERS [optional] for sketch and libraries
    - SOURCES [optional] for sketch and libraries
    - DISTFILES (to ino-file)
- Sketches
  - ConfigurableDelayTimer  
    - Project: SimulateConfigurableDelayTimer.pro
    - Connection to ino-file: configurabledelaytimerino.cpp
  - Joystick  
    - Project: SimulateJoystick.pro
    - Connection to ino-file: Joystickino.cpp
  - OctansSimulator  
    - Project: SimulateOctansSimulator.pro
    - Connection to ino-file: OctansSimulatorino.cpp
