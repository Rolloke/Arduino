
#include <EEPROM.h>

#include <button.h>

#define PARALLEL 1
#define I2C 2
#define USE_LIQUID_CRYSTAL PARALLEL

#define LCD_XDIM 16
#define LCD_YDIM 2

#if USE_LIQUID_CRYSTAL == PARALLEL

#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal LCD(rs, en, d4, d5, d6, d7);

#elif USE_LIQUID_CRYSTAL == I2C

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD(0x27,LCD_XDIM,LCD_YDIM);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#endif

#include "OctansSimulatorMenu.h"
#include "MPU6050_control.h"

#define  BTN_LEFT    1
#define  BTN_RIGHT   2
#define  BTN_UP      3
#define  BTN_DOWN    4
#define  BTN_ESCAPE  5
#define  BTN_ENTER   6

#define BTN_BIT0 10
#define BTN_BIT1 11
#define BTN_BIT2 12

//#define BTN_BIT0 31
//#define BTN_BIT1 33
//#define BTN_BIT2 35

void   storeConfig();

void printLineToLCD(String& fString, int line=0);
void displayMenuItem();

char  gEEPROMid[]   = "Octans";
int   g_ms_per_byte = 2;
bool  gInvalid = true;

void notifyMenu(uint8_t aDir, uint8_t aID);
// Button implementation
void triggerButton(uint8_t aState, uint8_t aID);
uint8_t gButtonPins[3] = { BTN_BIT0, BTN_BIT1, BTN_BIT2 };
Button gButtons(gButtonPins, sizeof(gButtonPins), triggerButton);

void setup() 
{
    MenuItem::setNotificationFunction(notifyMenu);
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
        initDefaultData();
    }

    getProtocolItem().select(gData.mProtocoll);
    String f(gData.mBaudrate);
    getBaudrateItem().select(f.c_str());

    gButtons.setDeBounce(50);
    gButtons.setDelay(1000);
    gButtons.setRepeat(250);

#if USE_LIQUID_CRYSTAL == PARALLEL
    LCD.begin(LCD_XDIM, LCD_YDIM);
#else
    LCD.init();                      // initialize the lcd 
    LCD.init();
    LCD.backlight();
#endif
   
    setupMPU6050();

    Serial.begin(gData.mBaudrate);

}

unsigned long gNextOutput = 0;

void loop() 
{
    unsigned long fNow = millis();
    gButtons.tick(fNow);

    if (fNow >= gNextOutput)
    {
        gNextOutput = fNow + gData.mInterval;
        if (gData.mOperationMode != Simulation)
        {
            readMPU6050Values();
        }
        switch (gData.mProtocoll)
        {
        case Pashr: printPASHR(); break;
        case Tss1:  printTSS1(); break;
#if USE_SSV == 1
        case SSV:   printSSV(); break;
#endif
        }
        if (gData.mOperationMode == Simulation)
        {
            modifyValues(fNow);
        }
    }
    if (gInvalid)
    {
        displayMenuItem();
        gInvalid = false;
    }
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
}


MenuItem::dir::to toMenuBtn(uint8_t aPin)
{
    switch(aPin)
    {
    case BTN_LEFT:   return MenuItem::dir::left;
    case BTN_RIGHT:  return MenuItem::dir::right;
    case BTN_UP:     return MenuItem::dir::up;
    case BTN_DOWN:   return MenuItem::dir::down;
    case BTN_ESCAPE: return MenuItem::dir::escape;
    case BTN_ENTER:  return MenuItem::dir::enter;
    default:         return MenuItem::dir::unknown;
    }
}

void triggerButton(uint8_t aState, uint8_t aPin)
{
    MenuItem::dir::to  menu_btn = toMenuBtn(aPin);
    switch (aState)
    {
    case Button::pressed:
    case Button::delayed:
    case Button::repeated:
        if (menu_btn)
        {
            getMenu().pressBtn(menu_btn);
            displayMenuItem();
        }
        break;
    }
}


void displayMenuItem()
{
    static String fLine0;
    static String fLine1;
    int fCursor = 0;
    String fString = getMenu().getText(&fCursor);
    fillStringWithSpaces(fString, LCD_XDIM);
    if (fLine1 != fString)
    {
        LCD.setCursor(0, 1);
        LCD.print(fString);
        fLine1 = fString;
    }

    fString = getMenu().getPath();
    if (fString.length())
    {
        int pos = fString.indexOf("...");
        if (pos != -1)
        {
            fString = fString.substring(0, pos);
        }
    }
    else
    {
        fString  = getOperationMode(gData.mOperationMode);
        fString += ":";
        fString += getProtocol(gData.mProtocoll);
    }
    fillStringWithSpaces(fString, LCD_XDIM);
    if (fLine0 != fString)
    {
        LCD.setCursor(0, 0);
        LCD.print(fString);
        fLine0 = fString;
    }

    if (fCursor)
    {
        LCD.setCursor(fCursor, 1);
        LCD.cursor();
    }
    else
    {
        LCD.noCursor();
    }
}

void printLineToLCD(String& fString, int line)
{
    fillStringWithSpaces(fString, LCD_XDIM);
    LCD.setCursor(0, line);
    LCD.print(fString);
}

void notifyMenu(uint8_t aDir, uint8_t aID)
{
    switch (aID)
    {
    case Baudrate:
    {
        int fNewBaudrate = atoi(getBaudrate(aDir));
        if (fNewBaudrate != gData.mBaudrate)
        {
            Serial.begin(fNewBaudrate);
            gData.mBaudrate = fNewBaudrate;
        }
    } break;
    case Protocol: gData.mProtocoll = aDir; break;
    case Settings:
        if (aDir == Store)
        {
            storeConfig();
        }
        else if (aDir == Clear)
        {
            EEPROM.write(0, 0);
        }
        break;
    default:
        break;
    }
}
