#include <WString.h>
#include <serialinterface.h>
#include <mainwindow.h>
#include <cmath>

#ifndef ADRDUINO_H_INCLUDED
#define ADRDUINO_H_INCLUDED

#define ArduinoEmulator 1

#define ARDUINO 100
typedef void (*InterrupFunction)();

typedef unsigned char  byte;
typedef unsigned short word;
typedef bool boolean;

extern SerialInterface Serial;

#define A0  (getFirstAnalogPin()  + 0 )
#define A1  (getFirstAnalogPin()  + 1 )
#define A2  (getFirstAnalogPin()  + 2 )
#define A3  (getFirstAnalogPin()  + 3 )
#define A4  (getFirstAnalogPin()  + 4 )
#define A5  (getFirstAnalogPin()  + 5 )
#define A6  (getFirstAnalogPin()  + 6 )
#define A7  (getFirstAnalogPin()  + 7 )
#define A8  (getFirstAnalogPin()  + 8 )
#define A9  (getFirstAnalogPin()  + 9 )
#define A10 (getFirstAnalogPin() + 10 )
#define A11 (getFirstAnalogPin() + 11 )
#define A12 (getFirstAnalogPin() + 12 )
#define A13 (getFirstAnalogPin() + 13 )
#define A14 (getFirstAnalogPin() + 14 )
#define A16 (getFirstAnalogPin() + 15 )

#define PIN_SPI_SS    (10)
#define PIN_SPI_MOSI  (11)
#define PIN_SPI_MISO  (12)
#define PIN_SPI_SCK   (13)


static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define PROGMEM
#define F(TEXT) TEXT

extern MainWindow* gMainWindowPointer;

enum ePinType
{
    INPUT, OUTPUT, INPUT_PULLUP
};

enum eLevel
{
    LOW, HIGH
};

enum eInterruptMode
{
    RISING, FALLING, CHANGE
};

#define LED_BUILTIN 13
void doTest();

class Ui_MainWindow;
void initialize(Ui_MainWindow *ui);

void setup();
void loop();

const char* nameof_pintype(int);

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))


long map(long aValue, long aFromLow, long aFromHigh, long aToLow, long aToHigh);

int           millis();
unsigned long micros();

void delay(unsigned long aTime_ms);
void delayMicroseconds(unsigned int aTime_us);

int  getFirstAnalogPin();

void pinMode(int aPin, int aType);

int  digitalRead(int aPin);
void digitalWrite(int aPin, int aValue);

int  analogRead(int aPin);
void analogWrite(int aPin, int aValue);

void tone(int aPin, int aFrequency, int aLenght=0);
void noTone(int aPin);

int  pulseIn(int aPin, int aLevel, int aTimeout);

unsigned char shiftIn(int dataPin, int clockPin, int bitOrder);
void          shiftOut(int dataPin, int clockPin, int bitOrder, unsigned char value);

int  digitalPinToInterrupt(int aPin);
void attachInterrupt(int aInterrupt, InterrupFunction aFunction, int aInterruptMode);
void detachInterrupt(int aInterrupt);

bool isAlphaNumeric(int aChar);
bool isAlpha(int aChar);
bool isAscii(int aChar);
bool isWhitespace(int aChar);
bool isControl(int aChar);
bool isDigit(int aChar);
bool isGraph(int aChar);
bool isLowerCase(int aChar);
bool isPrintable(int aChar);
bool isPunct(int aChar);
bool isSpace(int aChar);
bool isUpperCase(int aChar);
bool isHexadecimalDigit(int aChar);

void randomSeed(long aSeed);
long random(long aMax);
long random(long aMin, long aMax);

byte lowByte(word aWord);
byte highByte(word aWord);
bool bitRead(long aValue, long aBit);
void bitWrite(long& aValue, long aBit, bool aSet);
void bitSet(long& aValue, long aBit);
void bitClear(long& aValue, long aBit);
long bit(long aBit);


#endif
