#include <Arduino.h>

#include <QThread>

#include <EEPROM.h>

long map(long aValue, long aFromLow, long aFromHigh, long aToLow, long aToHigh)
{
  return (aValue - aFromLow) * (aToHigh - aToLow) / (aFromHigh - aFromLow) + aToLow;
}

void delay(unsigned long aTime_ms)
{
    QThread::msleep(aTime_ms);
}

void delayMicroseconds(unsigned int aTime_us)
{
    QThread::usleep(aTime_us);
}



const char* nameof_pintype(int aPintype)
{
    switch (aPintype)
    {
    case INPUT:        return "INPUT";
    case OUTPUT:       return "OUTPUT";
    case INPUT_PULLUP: return "INPUT_PULLUP";
    }
    return "";
}

int millis()
{
    return MainWindow::elapsed();
}

unsigned long micros()
{
    return MainWindow::elapsed()*1000;
}



void pinMode(int aPin, int aType)
{
    MainWindow::pinMode(aPin, aType);
}

int  digitalRead(int aPin)
{
    return MainWindow::digitalRead(aPin);
}

void digitalWrite(int aPin, int aValue)
{
    MainWindow::digitalWrite(aPin, aValue);
}

int  analogRead(int aPin)
{
    return  MainWindow::analogRead(aPin);
}

void analogWrite(int aPin, int aValue)
{
    MainWindow::analogWrite(aPin, aValue);
}

void tone(int aPin, int aFrequency, int aLength)
{
     MainWindow::tone(aPin, aFrequency, aLength);
}

void noTone(int aPin)
{
     MainWindow::noTone(aPin);
}

int  pulseIn(int aPin, int aLevel, int aTimeout)
{
    return  MainWindow::pulseIn(aPin, aLevel, aTimeout);
}

int  digitalPinToInterrupt(int aPin)
{
     return aPin;
}

int  getFirstAnalogPin()
{
    return MainWindow::getFirstAnalogPin();
}

void attachInterrupt(int /* aInterrupt */, InterrupFunction /* aFunction */, int /* aInterruptMode */)
{
    // TODO: implement
}

void detachInterrupt(int /* aInterrupt */)
{
    // TODO: implement
}

bool isAlphaNumeric(int aChar)
{
    return isalnum(aChar);
}

bool isAlpha(const int aChar)
{
    return isalpha(aChar);
}

bool isAscii(int aChar)
{
    return isascii(aChar);
}

bool isWhitespace(int aChar)
{
    return isblank(aChar);
}

bool isSpace(int aChar)
{
    return isspace(aChar);
}

bool isControl(int aChar)
{
    return iscntrl(aChar);
}

bool isDigit(int aChar)
{
    return isdigit(aChar);
}

bool isGraph(int aChar)
{
    return isgraph(aChar);
}

bool isLowerCase(int aChar)
{
    return islower(aChar);
}

bool isPrintable(int aChar)
{
    return isprint(aChar);
}

bool isPunct(int aChar)
{
    return ispunct(aChar);
}

bool isUpperCase(int aChar)
{
    return isupper(aChar);
}

bool isHexadecimalDigit(int aChar)
{
    return isxdigit(aChar);
}

void randomSeed(long aSeed)
{
    srand(aSeed);
}

long random(long aMax)
{
    return map(rand(), 0, RAND_MAX, 0, aMax);
}

long random(long aMin, long aMax)
{
    return map(rand(), 0, RAND_MAX, aMin, aMax);
}

byte lowByte(word aWord)
{
    return (aWord & 0x00ff);
}

byte highByte(word aWord)
{
    return ((aWord & 0xff00) >> 8);
}

bool bitRead(long aValue, long aBit)
{
    return (aValue & bit(aBit)) != 0;
}

void bitWrite(long& aValue, long aBit, bool aSet)
{
    if (aSet) aValue |=  bit(aBit);
    else      aValue &= ~bit(aBit);
}

void bitSet(long &aValue, long aBit)
{
    bitWrite(aValue, aBit, true);
}

void bitClear(long &aValue, long aBit)
{
    bitWrite(aValue, aBit, false);
}

long bit(long aBit)
{
    return 1 << aBit;
}
