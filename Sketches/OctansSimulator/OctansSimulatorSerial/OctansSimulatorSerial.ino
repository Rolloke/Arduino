#include <texttools.h>

#include <EEPROM.h>
#include <Time.h>


#define DEBUG_OUT 1
// #define PRINT_HELP 1

#ifdef DEBUG_OUT
bool      gDebugOut = true;
#define DEBUG_PRINT(X) if (gDebugOut) Serial.print(X);
#define DEBUG_PRINTLN(X) if (gDebugOut) Serial.println(X);
#else
#define DEBUG_PRINT //
#define DEBUG_PRINTLN //
#endif


#ifdef PRINT_HELP
#define HELP_PRINT(X) Serial.print(X);
#define HELP_PRINTLN(X) Serial.println(X);
#else
#define HELP_PRINT //
#define HELP_PRINTLN //
#endif

struct SimulatorItem
{
  uint16_t mInterval;
  float    mMin;
  float    mMax;
  float    mValue;
  float    mAcc;
};

struct PersistentData
{
  long mBaudrate;
  uint16_t mInterval;     // ms;
  uint8_t  mProtocoll;    // Off, PASHR, TSS1, SSV
  uint8_t  mSimulate;     // simulation on, off
  SimulatorItem mHeave;
  SimulatorItem mRoll;
  SimulatorItem mPitch;
  SimulatorItem mHeading;
  SimulatorItem mSSV;
};

const char* gKeywords[] = 
{
  "BAUDRATE",     // 0
  "INTERVAL",     // 1
  "PROTOCOL",     // 2
  "PASHR",        // 3
  "TSS1",         // 4
  "HEAVE",        // 5
  "ROLL",         // 6
  "PITCH",        // 7
  "HEADING",      // 8
  "SSV",          // 9
  "MIN",          // 10
  "MAX",          // 11
  "CONFIG",       // 12
  "STORE",        // 13
  "OFF",          // 14
  "VALUE",        // 15
  "ACCURACY",     // 16
#ifdef DEBUG_OUT
  "DEBUG", 
#endif
  NULL
};

enum eKeys
{
  Baudrate = 0, // 0
  Interval,     // 1
  Protocol,     // 2
  Pashr,        // 3
  Tss1,         // 4
  Heave,        // 5
  Roll,         // 6
  Pitch,        // 7
  Heading,      // 8
  SSV,          // 9
  Min,          // 10
  Max,          // 11
  Config,       // 12
  Store,        // 13
  Off,          // 14
  Value,        // 15
  Accuracy,     // 16
#ifdef DEBUG_OUT
  Debug,      // 
#endif
  LastKey
};

void   check_sentence(String& fString);
const char* read_from_serial();
void   printPASHR();
void   printTSS1();
void   printSSV();
void   readValues();
void   modifyValues(unsigned long aNow);
void   storeConfig();
void   printConfig();

PersistentData gData;
char  gEEPROMid[]   = "Octans";
int   g_ms_per_byte = 2;

const uint8_t gProtocollPin  = 10;

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
    len = sizeof(PersistentData);
    char *fBlock = (char*) &gData;
    for (int j=0; j<len; ++j, ++i)
    {
      fBlock[j] = EEPROM.read(i);
    }
  }
  else // init default
  {
    gData.mBaudrate  = 38400; // Bd
    gData.mInterval  = 1000;   // ms
    gData.mProtocoll = Off;
    gData.mSimulate  = 1;
    gData.mHeave   = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mRoll    = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mPitch   = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mHeading = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mSSV     = { 10000, 1450, 1550, 1500, 0.1 };
  }

  pinMode(gProtocollPin, INPUT);
  digitalWrite(gProtocollPin, HIGH);
  
  Serial.begin(gData.mBaudrate);
  
}

unsigned long gNextOutput = 0;
unsigned long gProtocollPinTime = 0;
int8_t        gProtocollPinState = 0;

void loop() 
{
  String fSentence = read_from_serial();
  check_sentence(fSentence);
  unsigned long fNow = millis();
  if (digitalRead(gProtocollPin) == LOW)
  {
    if (gProtocollPinState == 0)
    {
      gProtocollPinState = 1;
      gProtocollPinTime = fNow + 50;
    }
    if (gProtocollPinState == 1 && fNow > gProtocollPinTime)
    {
      switch (gData.mProtocoll)
      {
        case Off:   gData.mProtocoll = Pashr; break;
        case Pashr: gData.mProtocoll = Tss1;  break;
        case Tss1:  gData.mProtocoll = SSV;   break;
        case SSV:   gData.mProtocoll = Off;   break;
      }
      gProtocollPinState = 2;
    }
  }
  else
  {
    gProtocollPinState = 0;
  }
  if (fNow >= gNextOutput)
  {
    gNextOutput = fNow + gData.mInterval;
    if (!gData.mSimulate)
    {
      readValues();      
    }
    switch (gData.mProtocoll)
    {
      case Pashr: printPASHR(); break;
      case Tss1:  printTSS1(); break;
      case SSV:   printSSV(); break;
    }
    if (gData.mSimulate)
    {
      modifyValues(fNow);
    }
    // measure times
  }
}

void  printPASHR()
{
  String sPASHR;
  // sPASHR = "$PASHR,200345.00,78.00,T,-3.00,+2.00,+0.00,1.000,1.000,1.000,1,1*32";
  time_t fNow = now();
  tmElements_t fTime;
  breakTime(fNow, fTime);
      
  sPASHR = "$PASHR,";
  if (fTime.Hour < 10) sPASHR.concat('0');
  sPASHR.concat(static_cast<int>(fTime.Hour));
  if (fTime.Minute < 10) sPASHR.concat('0');
  sPASHR.concat(static_cast<int>(fTime.Minute));
  if (fTime.Second < 10) sPASHR.concat('0');
  sPASHR.concat(static_cast<int>(fTime.Second));
  sPASHR.concat(','); // Heading HHH.HH
  sPASHR.concat(printFloat(gData.mHeading.mValue, 0, 2, false));
  sPASHR.concat(",T,"); // True Heading, Roll RRR.RR
  sPASHR.concat(printFloat(gData.mRoll.mValue, 0, 2));
  sPASHR.concat(','); // Pitch PPP.PP
  sPASHR.concat(printFloat(gData.mPitch.mValue, 0, 2));
  sPASHR.concat(','); // Pitch PPP.PP
  sPASHR.concat(printFloat(gData.mHeave.mValue, 0, 2));
  sPASHR.concat(',');  // Roll Accuracy
  sPASHR.concat(printFloat(gData.mRoll.mAcc, 0, 3));
  sPASHR.concat(',');  // Pitch Accuracy
  sPASHR.concat(printFloat(gData.mPitch.mAcc, 0, 3));
  sPASHR.concat(',');  // Heading Accuracy
  sPASHR.concat(printFloat(gData.mHeading.mAcc, 0, 3));
  sPASHR.concat(",1"); // No Position
  sPASHR.concat(",1,*"); // INS Status Flag
  uint8_t fChkSum = 0;
  for (uint16_t i=0; i<sPASHR.length(); ++i)
  {
    fChkSum = fChkSum ^ sPASHR[i];
  }
  sPASHR.concat(String(fChkSum, HEX)); // INS Status Flag
  Serial.println(sPASHR);
}

void  printTSS1()
{
//String sTSS1 = ":013D11 0000U-00510013"
  String sTSS1 = ":000000";
  sTSS1.concat(gData.mHeave.mValue < 0 ? "-" : " ");
  sTSS1.concat(printFloat(gData.mHeave.mValue*100, 4, 0, false));
  sTSS1.concat("U");
  sTSS1.concat(gData.mRoll.mValue < 0 ? "-" : " ");
  sTSS1.concat(printFloat(gData.mRoll.mValue*100, 4, 0, false));
  sTSS1.concat(gData.mPitch.mValue < 0 ? "-" : " ");
  sTSS1.concat(printFloat(gData.mPitch.mValue*100, 4, 0, false));
  Serial.println(sTSS1);
}

void  printSSV()
{
  Serial.println(printFloat(gData.mSSV.mValue, 1, 2, false));
}

void printItem(int idItem, SimulatorItem* pItem)
{
  switch (idItem)
  {
    case Heave:   Serial.print("Heave");  break;
    case Roll:    Serial.print("Roll");   break;
    case Pitch:   Serial.print("Pitch");  break;
    case Heading: Serial.print("Heading"); break;
    case SSV:     Serial.print("SSV");     break;
    default:      Serial.println("unknown"); break;
  }
  Serial.print(": Itervall: ");
  Serial.print(pItem->mInterval);
  Serial.print(", min: ");
  Serial.print(printFloat(pItem->mMin, 0, 1));
  Serial.print(", max: ");
  Serial.print(printFloat(pItem->mMax, 0, 1));
  Serial.print(", value: ");
  Serial.print(printFloat(pItem->mValue, 0, 1));
  Serial.print(", accuracy: ");
  Serial.println(printFloat(pItem->mAcc, 0, 2));
}

void   printConfig()
{
  Serial.print("mBaudrate: ");
  Serial.println(gData.mBaudrate);
  Serial.print("Protocoll: ");
  switch (gData.mProtocoll)
  {
    case Off:   Serial.println("Off"); break;
    case Pashr: Serial.println("PASHR"); break;
    case Tss1:  Serial.println("TSS1"); break;
    case SSV:   Serial.println("SSV"); break;
    default:    Serial.println("unknown"); break;
  }
  printItem(Heave  , &gData.mHeave);
  printItem(Roll   , &gData.mRoll);
  printItem(Pitch  , &gData.mPitch);
  printItem(Heading, &gData.mHeading);
  printItem(SSV    , &gData.mSSV);
}

void   storeConfig()
{
  int i, len = strlen(gEEPROMid);
  for (i=0; i<len; ++i)
  {
    EEPROM.write(i, gEEPROMid[i]);
  }
  if (i==len)
  {
    len = sizeof(PersistentData);
    char *fBlock = (char*) &gData;
    for (int j=0; j<len; ++j, ++i)
    {
       EEPROM.write(i, fBlock[j]);
    }
  }
  Serial.println("Configuration stored");
}

void  readValues()
{
  // http://playground.arduino.cc/Main/MPU-6050
}

void calculateValue(SimulatorItem* aItem, unsigned long aNow)
{
  if (aItem->mInterval && (aItem->mMax - aItem->mMin) != 0)
  {
    uint16_t fArg =  aNow % aItem->mInterval;
    aItem->mValue = (aItem->mMax + aItem->mMin) * 0.5 +  (aItem->mMax - aItem->mMin) * 0.5 * sin(6.28*(float)fArg/(float)aItem->mInterval);
  }
}

void modifyValues(unsigned long aNow)
{
  switch (gData.mProtocoll)
  {
    case Pashr: 
      calculateValue(&gData.mHeave  , aNow);
      calculateValue(&gData.mRoll   , aNow);
      calculateValue(&gData.mPitch  , aNow);
      calculateValue(&gData.mHeading, aNow);
    break;
    case Tss1: 
      calculateValue(&gData.mHeave, aNow);
      calculateValue(&gData.mRoll , aNow);
      calculateValue(&gData.mPitch, aNow);
    break;
    case SSV:
      calculateValue(&gData.mSSV, aNow);
    break;
  }
}


void check_sentence(String& fString)
{
  if (fString.length())
  {
    fString.toUpperCase();

    DEBUG_PRINT("received: ");
    DEBUG_PRINTLN(fString);

    const int fMaxArray = 5;
    int fKey, fNo, fPos = 0, fStartPos = 0, fIndex = 0;
    int fKeys[fMaxArray] = { 0 };
    int fNumbers[fMaxArray] = { 0 };
    while (fPos != -1)
    {
      fPos = fString.indexOf(':', fStartPos);

      DEBUG_PRINT("StartPos ");
      DEBUG_PRINT(fStartPos);
      DEBUG_PRINT(", Pos ");
      DEBUG_PRINT(fPos);

      if (fPos > 0)
      {
        fKey = findKey(fString.substring(fStartPos, fPos), gKeywords, fNo);
        fStartPos = fPos+1;
      }
      else 
      {
        fKey = findKey(fString.substring(fStartPos), gKeywords, fNo);
      }
      fKeys[fIndex]    = fKey;
      fNumbers[fIndex] = fNo;

      DEBUG_PRINT(", Index ");
      DEBUG_PRINT(fIndex);
      DEBUG_PRINT(", Key ");
      DEBUG_PRINT(fKeys[fIndex]);
      DEBUG_PRINT(", No ");
      DEBUG_PRINTLN(fNumbers[fIndex]);

      fIndex++;
    }
    SimulatorItem *pItem = 0;
    switch (fKeys[0])
    {
      case Baudrate:  gData.mBaudrate = fNumbers[1]; break;
      case Interval:  gData.mInterval = fNumbers[1]; break;
      case Protocol: gData.mProtocoll = fKeys[1];   break;
      case Heave:     pItem = &gData.mHeave;         break;
      case Roll:      pItem = &gData.mRoll;          break;
      case Pitch:     pItem = &gData.mPitch;         break;
      case Heading:   pItem = &gData.mHeading;       break;
      case SSV:       pItem = &gData.mSSV;           break;
      case Store:     storeConfig();                 break;
      case Config:    printConfig();                 break;
      default: Serial.println("wrong key 1");        break;
    }
    if (pItem)
    {
      switch (fKeys[1])
      {
        case Interval: pItem->mInterval = fNumbers[2];       break;
        case Min:      pItem->mMin      = fNumbers[2] * 0.1; break;
        case Max:      pItem->mMin      = fNumbers[2] * 0.1; break;
        case Value:    pItem->mValue    = fNumbers[2] * 0.1; break;
        case Accuracy: pItem->mAcc      = fNumbers[2] * 0.01; break;
        default:
        if (fNumbers[1] == Request) printItem(fKeys[0], pItem);
        else Serial.println("wrong key 2"); break;
      }
    }
  }
}

const char* read_from_serial()
{
  static const uint16_t gBufferLen = 128;
  static char gBuffer[gBufferLen];
  static int  gBufferPos = 0;
  static int  gLastTime = 0;
  unsigned int fNow = millis();
  while (Serial.available())
  {
    char fChar = Serial.read();
    gBuffer[gBufferPos++] = fChar;
    gLastTime = fNow;
    switch (fChar)
    {
      case '\n':
      gBuffer[gBufferPos-1] = 0;
      gBufferPos = 0;
      return &gBuffer[0];
    }
  }
  if (gBufferPos > 0 && (int)(fNow - gLastTime) > g_ms_per_byte)
  {
      gBuffer[gBufferPos] = 0;
      gBufferPos = 0;
      return &gBuffer[0];
  }
  return "";
}

