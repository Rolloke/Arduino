#include <texttools.h>
#include <EEPROM.h>

#include "Input.h"
#include "Output.h"


#define DEBUG_OUT 1
#define PRINT_HELP 1

#ifdef DEBUG_OUT
bool      gDebugOut = false;
#define DEBUG_PRINT(X) if (gDebugOut) Serial.print(X);
#define DEBUG_PRINTLN(X) if (gDebugOut) Serial.println(X);
#else
#define DEBUG_PRINT(X) //
#define DEBUG_PRINTLN(X) //
#endif

#ifdef PRINT_HELP
#define HELP_PRINT(X) Serial.print(X);
#define HELP_PRINTLN(X) Serial.println(X);
#else
#define HELP_PRINT(X) //
#define HELP_PRINTLN(X) //
#endif

const int gNoOfPins = 12;
OutputPin gOutputPins[gNoOfPins];
InputPin  gInputPins[gNoOfPins];
uint32_t  gBaudRate = 115200;
bool      gHighPrecision = false;
uint16_t  g_ms_per_byte = 2;

enum eKeys
{
  Pin     = 0,//  0
  Input,      //  1
  Output,     //  2
  Invert,     //  3
  Repeat,     //  4
  Delay,      //  5
  Pulse,      //  6
  Binary,     //  7
  Analog,     //  8
  Value,      //  9
  Connect,    // 10
  Edge,       // 11
  Rising,     // 12
  Falling,    // 13
  Threshold,  // 14
  Switch,     // 15
  Config,     // 16
  Toggle,     // 17
  Report,     // 18
  Interrupt,  // 19
  Low,        // 20
  High,       // 21
  Tone,       // 22
  Change,     // 23
  Store,      // 24
  Clear,      // 25
  Help,       // 26
  Baud,       // 27
#ifdef DEBUG_OUT
  Debug,
#endif
  LastKey
};

const char* gKeywords[] = 
{
  "PIN",      // 0
  "INPUT",    // 1
  "OUTPUT",   // 2
  "INVERT",   // 3
  "REPEAT",   // 4
  "DELAY",    // 5
  "PULSE",    // 6
  "BINARY",   // 7
  "ANALOG",   // 8
  "VALUE",    // 9
  "CONNECT",  // 10
  "EDGE",     // 11
  "RISING",   // 12
  "FALLING",  // 13
  "THRESHOLD",// 14
  "SWITCH",   // 15
  "CONFIG",   // 16
  "TOGGLE",   // 17
  "REPORT",   // 18
  "INTERRUPT",// 19
  "LOW",      // 20
  "HIGH",     // 21
  "TONE",     // 22
  "CHANGE",   // 23
  "STORE",    // 24
  "CLEAR",    // 25
  "HELP",     // 26
  "BAUD",     // 27
#ifdef DEBUG_OUT
  "DEBUG", 
#endif
  0
};


const char  gEEPROMid[] = "DelayTimer";

void  check_sentence(String& fString);
const char* read_from_serial();
int   limitNo(int aNo);


void setup() 
{
    int i, len = strlen(gEEPROMid);

    for (i=0; i<len; ++i)
    {
      char fByte = EEPROM.read(i);
      if (fByte != gEEPROMid[i]) break;
    }

    if (i==len)
    {
      len = sizeof(gOutputPins);
      char *fBlock = (char*) &gOutputPins;
      for (int j=0; j<len; ++j, ++i)
      {
        fBlock[j] = EEPROM.read(i);
      }
      for (int j=0; j<gNoOfPins; ++j, ++i)
      {
        gOutputPins[j].init();
        uint8_t fConnectedIndex = EEPROM.read(i);
        if (fConnectedIndex)
        {
          gOutputPins[j].setConnection(&gOutputPins[fConnectedIndex-1]);
        }
      }

      len = sizeof(gInputPins);
      fBlock = (char*) &gInputPins;
      for (int j=0; j<len; ++j, ++i)
      {
        fBlock[j] = EEPROM.read(i);
      }
      for (int j=0; j<gNoOfPins; ++j, ++i)
      {
        gInputPins[j].init();
        uint8_t fConnectedIndex = EEPROM.read(i);
        if (fConnectedIndex)
        {
          gInputPins[j].setConnection(&gOutputPins[fConnectedIndex-1]);
        }
      }

      len = sizeof(gBaudRate);
      fBlock = (char*) &gBaudRate;
      for (int j=0; j<len; ++j, ++i)
      {
        fBlock[j] = EEPROM.read(i);
      }
      gHighPrecision = EEPROM.read(i);
    }
    Serial.begin(gBaudRate);
    //initKeys();
}

void loop() 
{
  String fSentence = read_from_serial();
  check_sentence(fSentence);
  unsigned long fNow;
  fNow = gHighPrecision ? micros() : millis();
  for (int i=0; i < gNoOfPins; ++i)
  {
    gInputPins[i].tick(fNow);
    gOutputPins[i].tick(fNow);
  }
}

uint8_t getOutputPinIndex(OutputPin* pPin)
{
    for (int i=0; i < gNoOfPins; ++i)
    {
      if (&gOutputPins[i] == pPin)
      {
          return i+1;
      }
    }
    return 0;
}

void check_sentence(String& fString)
{
  if (fString.length())
  {
    fString.toUpperCase();

    DEBUG_PRINT(F("received: "))
    DEBUG_PRINTLN(fString)

    const int fMaxArray = 5;
    int fKey, fNo, fPos = 0, fStartPos = 0;
    int fKeys[fMaxArray] = { 0 };
    int fNumbers[fMaxArray] = { 0 };
    
    for (int fIndex = 0; fPos != -1 && fIndex < fMaxArray; fIndex++)
    {
      fPos = fString.indexOf(':', fStartPos);

      DEBUG_PRINT(F("StartPos "))
      DEBUG_PRINT(fStartPos)
      DEBUG_PRINT(F(", Pos "))
      DEBUG_PRINT(fPos)

      String fKeyWord;
      if (fPos > 0)
      {
        fKeyWord = fString.substring(fStartPos, fPos);
        fStartPos = fPos+1;
      }
      else 
      {
         fKeyWord = fString.substring(fStartPos);
      }

      fKey = findKey(fKeyWord, gKeywords, fNo);
      if (fKey == None && !isDigit(fKeyWord[0]) && fNo != Request)
      {
        Serial.print(fKeyWord);
        Serial.println(F(": unknown key"));
      }

      fKeys[fIndex]    = fKey;
      fNumbers[fIndex] = fNo;

      DEBUG_PRINT(F(", Index "))
      DEBUG_PRINT(fIndex)
      DEBUG_PRINT(F(", Key "))
      DEBUG_PRINT(fKeys[fIndex])
      DEBUG_PRINT(F(", No "))
      DEBUG_PRINTLN(fNumbers[fIndex])
    }
    int fPinNo = limitNo(fNumbers[0]);
    switch (fKeys[0])
    {
      case Output:
      {
        switch(fKeys[1])
        {
          case Invert: gOutputPins[fPinNo].setInvert(fNumbers[2]); break;
          case Toggle: gOutputPins[fPinNo].setToggle(fNumbers[2]); break;
          case Repeat: gOutputPins[fPinNo].setRepeat(fNumbers[2]); break;
          case Delay : gOutputPins[fPinNo].setDelay(fNumbers[2]);  break;
          case Pulse : gOutputPins[fPinNo].setPulse(fNumbers[2]);  break;
          case Pin   : gOutputPins[fPinNo].setPin(fNumbers[1], fKeys[2] == Binary ? OutputPin::Binary : fKeys[2] == Analog ? OutputPin::Analog : OutputPin::Tone); break;
          case Edge  : gOutputPins[fPinNo].setEdge(fKeys[2] == Rising ? OutputPin::Rising : fKeys[2] == Falling ? OutputPin::Falling : OutputPin::Change);  break;
          case Connect:gOutputPins[fPinNo].setConnection(&gOutputPins[limitNo(fNumbers[2])]); break;
          case Value : 
          if (fNumbers[1] == Request)
          {
            Serial.println(gOutputPins[fPinNo].getValue());
          }
          else
          {
            gOutputPins[fPinNo].setValue(fNumbers[2]);
            gOutputPins[fPinNo].setValue(fNumbers[2], true);
          }
          break;
          default:
          if (fNumbers[1] == Request)
          {
            Serial.println(gOutputPins[fPinNo].info());
          }
          break;
        }
      }
      break;
      case Input:
      {
        switch(fKeys[1])
        {
          case Pin:
            gInputPins[fPinNo].setPin(fNumbers[1], fKeys[2] != Analog);
            if      (fKeys[2] == Switch)    gInputPins[fPinNo].setEdge(InputPin::Switch);
            else if (fKeys[2] == Interrupt) gInputPins[fPinNo].setEdge(InputPin::interrupt);
          break;
          case Edge:
          {
            InputPin::edge fEdge = InputPin::no_edge;
            switch (fKeys[2])
            {
              case Rising : fEdge = InputPin::rising; break;
              case Falling: fEdge = InputPin::falling; break;
              case Low    : fEdge = InputPin::low; break;
              case High   : fEdge = InputPin::high; break;
              case Change : fEdge = InputPin::change; break;
              case Pulse  :
              {
                Serial.print(F("waiting "));
                Serial.print(fNumbers[4]);
                Serial.print(F(" ms for "));
                Serial.print(fKeys[3] == High ? F("high") : F("low"));
                Serial.println(F(" pulse"));
                uint32_t fTime = gInputPins[fPinNo].getPulseLength(fKeys[3] == High, fNumbers[4]);
                Serial.print(F("pulse length: "));
                Serial.print(fTime);
                Serial.println(F(" µs"));
                return;
              }
            }
            gInputPins[fPinNo].setEdge(fEdge);
            if (gInputPins[fPinNo].isAnalog() && fEdge == InputPin::change && fNumbers[3] != None)
            {
              gInputPins[fPinNo].setDebounce(fNumbers[3]);
            }
          }
          break;
          case Connect  : gInputPins[fPinNo].setConnection(&gOutputPins[limitNo(fNumbers[2])]); break;
          case Threshold: gInputPins[fPinNo].setThreshold(fNumbers[2]); 
                          if (fNumbers[3] != None) gInputPins[fPinNo].setDebounce(fNumbers[3]);
              break;
          case Pulse    : gInputPins[fPinNo].setDebounce(fNumbers[2]); break;
          case Delay    : gInputPins[fPinNo].setDelay(fNumbers[2]);    break;
          case Repeat   : gInputPins[fPinNo].setRepeat(fNumbers[2]);   break;
          case Report   : gInputPins[fPinNo].setReport(fNumbers[2]);   break;
          case Value  :
            if (fNumbers[1] == Request)
            {
              Serial.println(gInputPins[fPinNo].getValue());
            }
            else
            {
              gInputPins[fPinNo].setValue(fNumbers[2]);
            }
            break;
          default:
            if (fNumbers[1] == Request)
            {
              Serial.println(gInputPins[fPinNo].info());
            }
            break;
        }
      }
      break;
      case Baud:
        gBaudRate = fNumbers[1];
        break;
#ifdef DEBUG_OUT
      case Debug: gDebugOut = fNumbers[1] != 0 ? true : false; break;
#endif
      case Config:
        if (fNumbers[0] == Request)
        {
          Serial.println(F("Inputs:"));
          for (int i=0; i < gNoOfPins; ++i)
          {
            if (gInputPins[i].pin())
            {
              Serial.print(i+1);
              Serial.print(F(" :"));
              Serial.println(gInputPins[i].info());
            }
          }
          Serial.println(F("Outputs:"));
          for (int i=0; i < gNoOfPins; ++i)
          {
            if (gOutputPins[i].pin())
            {
              Serial.print(i+1);
              Serial.print(F(" :"));
              Serial.println(gOutputPins[i].info());
            }
          }
          Serial.print(F("Baudrate :"));
          Serial.println(gBaudRate);
          if (gHighPrecision)
          {
              Serial.println(F("Time accuracy is micro seconds"));
          }
        }
        else if (fKeys[1] == Store)
        {
            int i, len = strlen(gEEPROMid);
            for (i=0; i<len; ++i)
            {
              EEPROM.write(i, gEEPROMid[i]);
            }
            if (i==len)
            {
              char *fBlock = (char*) &gOutputPins;
              len = sizeof(gOutputPins);
              for (int j=0; j<len; ++j, ++i)
              {
                EEPROM.write(i, fBlock[j]);
              }
              for (int j=0; j<gNoOfPins; ++j, ++i)
              {
                  EEPROM.write(i, getOutputPinIndex(gOutputPins[j].getConnection()));
              }

              fBlock = (char*) &gInputPins;
              len =sizeof(gInputPins);
              for (int j=0; j<len; ++j, ++i)
              {
                EEPROM.write(i, fBlock[j]);
              }
              for (int j=0; j<gNoOfPins; ++j, ++i)
              {
                  EEPROM.write(i, getOutputPinIndex(gInputPins[j].getConnection()));
              }
              fBlock = (char*) &gBaudRate;
              len =sizeof(gBaudRate);
              for (int j=0; j<len; ++j, ++i)
              {
                EEPROM.write(i, fBlock[j]);
              }
              EEPROM.write(i, gHighPrecision);
            }
        }
        else if (fKeys[1] == Clear)
        {
            EEPROM.write(0, 0);
        }
        else if (fKeys[1] == High)
        {
            gHighPrecision = fNumbers[2] == 1;
        }
      break;
      default: break;

      case Help:
      {
        Serial.print(F("Maximum number of In/Outputs: "));
        Serial.println(gNoOfPins);
        Serial.println(F("List of Keywords:"));
        for (int i=0; i<LastKey; ++i)
        {
          Serial.println(gKeywords[i]);
        }
        Serial.println(F("Command or value separator :"));
        Serial.println(F("Request ?"));
        Serial.println(F("Output or pin number [digit]"));
        Serial.println(F("Values digit"));

#ifdef PRINT_HELP  
        Serial.print(F("Pins: 14 DIO 5V, 20 mA, 8 Analog input\n"
         "-14:DIO:Bu1:A0\n"
         "-15:DIO:Bu2:A1\n"
         "-16:DIO:Bu3:A2\n"
         "- 3:DIO:Bu4:PWM:ExtInt\n"
         "- 5:DIO:Bu5:PWM:SCL\n"
         "- 6:DIO:Bu6:PWM\n"
         "- 4:DIO:Switch1:SDA\n"
         "- 7:DIO:Switch2\n"
         "- 8:DIO:Switch3\n"
         "- 9:DIO:LED1:PWM\n"
         "-10:DIO:LED2:PWM:SS\n"
         "-11:DIO:LED3:PWM:MOSI\n"
         "- 2:DIO:ExtInt\n"
         "-12:DIO:MISO\n"
         "-13:DIO:SCK:LED\n"
         "-17:DIO:A3\n"
         "-18:DIO:A4\n"
         "-19:DIO:A5\n"
         "-20:A6\n"
         "-21:A7\n"
         "Usage:\n"
         "OUTPUT[1]:PIN[13]:BINARY|ANALOG|TONE\n"
         "INPUT[1]:PIN[12]:BINARY|ANALOG|SWITCH|INTERRUPT\n"
         "-BINARY: DIO\n"
         "-ANALOG: Input A0-A7 or Output PWM\n"
         "-SWITCH: Switch to ground with internal pullup resistor\n"
         "-OUTPUT[n]:?/INVERT/TOGGLE/DELAY:ms/PULSE:ms/VALUE:0-255,?/REPEAT:m\n"
         "-INPUT[n]:?/EDGE:CHANGE|RISING|FALLING|LOW|HIGH/THRESHOLD:0-4095/VALUE:?/\n"
         " + CONNECT:OUTPUT[m]/REPORT/\n"
         " + PULSE:ms/DELAY:ms/REPEAT:ms\n"));
#endif
       }
      break;
    }
  }
}

const char* read_from_serial()
{
  static const uint16_t gBufferLen = 128;
  static char gBuffer[gBufferLen];
  static int  gBufferPos = 0;
  static unsigned int  gLastTime = 0;
  unsigned int fNow = millis();
  while (Serial.available() && gBufferPos < gBufferLen-1)
  {
    char fChar = Serial.read();
    gBuffer[gBufferPos++] = fChar;
    gLastTime = fNow;
    if (fChar == '\n')
    {
      gBuffer[gBufferPos-1] = 0;
      gBufferPos = 0;
      return &gBuffer[0];
    }
  }
  if (gBufferPos > 0 && (fNow - gLastTime) > g_ms_per_byte)
  {
      gBuffer[gBufferPos] = 0;
      gBufferPos = 0;
      return &gBuffer[0];
  }
  return "";
}

int   limitNo(int aNo)
{
  if (aNo == None)
  {
      DEBUG_PRINTLN(F("No Number"))
  }
  else if (aNo == Request)
  {
      DEBUG_PRINTLN(F("Request"))
  }
  else
  {
      --aNo;
      if (aNo < 0)
      {
        HELP_PRINT(F("Pin Number too low:"))
        HELP_PRINTLN(aNo)
        aNo = 0;
      }
      else if (aNo >= gNoOfPins)
      {
        HELP_PRINT(F("Pin Number too high:"));
        HELP_PRINTLN(aNo);
        aNo = 0;
      }
  }
  return aNo;
}
