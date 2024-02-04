
#ifndef EMULATED
#include <Mouse.h>
#include <Keyboard.h>
#include <Joystick.h>
#else
#define KEY_UP_ARROW 1
#define KEY_DOWN_ARROW 2
#define KEY_LEFT_ARROW 3
#define KEY_RIGHT_ARROW 4
#define KEY_PAGE_UP 5
#define KEY_PAGE_DOWN 6
#endif

#include <button.h>
#include <texttools.h>
#include <EEPROM.h>

void triggerButton(uint8_t, uint8_t);
const char* read_from_serial();
void check_sentence(String& fString);
int find_pin(int pin);
String fillStringWithSpaces(const char* aStr, unsigned int aLen);

const char  gEEPROMid[]        = "Joystick1.0";

struct Pins
{
    enum
    {
        up, down, left, right, vdd, f1, sel, gnd, f2, last, first=up, nc=last
    };
    static const char* Name[];
    static const int PinCon[last+1];
    static int No[last];
};

const char* Pins::Name[] =
{
    "UP",
    "DOWN",
    "LEFT",
    "RIGHT",
    "VDD",
    "F1",
    "SEL",
    "GND",
    "F2",
    "NC",
    0
};

/// @brief Pin Configuration for Arduino Micro
// connector on board
// Con1Pin: 1  3  5  7  9
// Con2Pin: 2  4  6  8
// connector to joystick
// ---------------------
// \ 1   2   3   4   5 /
//  \  6   7   8   9  /
//   -----------------
const int Pins::PinCon[Pins::last+1] =
{
    8,  // 1 -> 0
    9,  // 2 -> 1
    7,  // 3 -> 2
    A0, // 4 -> 3
    6,  // 5 -> 4
    2,  // 6 -> 5
    5,  // 7 -> 6
    3,  // 8 -> 7
    4,  // 9 -> 8
    0   // nc
};


// auf dem Schrott gefunden, nicht genormt
// -----------------------
// \  NC F1  L   D   R  /
//  \   U  NC  F2 GND  /
//   -------------------
int Pins::No[Pins::last] =
{
    Pins::PinCon[5],    // up
    Pins::PinCon[3],    // down
    Pins::PinCon[2],    // left
    Pins::PinCon[4],    // right
    0,                  // vdd
    Pins::PinCon[1],    // f1
    0,                  // sel
    Pins::PinCon[8],    // gnd
    Pins::PinCon[7],    // f2
};


const int ConfigPin1 = A1;
const int ConfigPin2 = A2;
const int ConfigPin3 = A3;


Button ButtonRight(Pins::No[Pins::right], triggerButton);
Button ButtonLeft (Pins::No[Pins::left],  triggerButton);
Button ButtonUp   (Pins::No[Pins::up],    triggerButton);
Button ButtonDown (Pins::No[Pins::down],  triggerButton);
Button ButtonBtn1 (Pins::No[Pins::f1],    triggerButton);
Button ButtonBtn2 (Pins::No[Pins::f2],    triggerButton);

int8_t XAxis = 0;
int8_t YAxis = 0;
bool   isJoystick     = false;
bool   isKeyboard     = false;
bool   isMouse        = false;
bool   isSerialConfig = false;

struct Keys
{
    enum e
    {
          joystick
        , up_key
        , down_key
        , left_key
        , right_key
        , fire1_key
        , fire2_key
        , fire_auto_delay
        , fire_auto_repeat
        , debounce_time
        , delay_time
        , repetition_time
        , mouse_speed
        , baud
        , pins
        , store_eeprom
        , clear_eeprom
        , help
    };
};

const char* gKeywords[] = 
{
    "JOYSTICK",       // 0
    "KEYUP",          // 1
    "KEYDOWN",        // 2
    "KEYLEFT",        // 3
    "KEYRIGHT",       // 4
    "KEYFIRE1",       // 5
    "KEYFIRE2",       // 6
    "AUTOFIREDELAY",  // 7
    "AUTOFIREREPEAT", // 8
    "DEBOUNCETIME",   // 9
    "DELAYTIME",      // 10
    "REPETITIONTIME", // 11
    "MOUSESPEED",     // 12
    "BAUD",           // 13
    "PIN",            // 14
    "STORE",
    "CLEAR",
    "HELP",
    0
};


struct Joystick
{
    enum e
    {
          no_standard
        , Msx_Joystick
        , Sega3and6
        , AmigaCD32
        , UserDefined
        , Last
        , First = no_standard
    };
    static const char* Name[];
};

const char* Joystick::Name[] =
{
      "No Standard"
    , "382px-MSX-Joystick"
    , "Sega 3&6 button controller"
    , "Amiga (CD32)"
    , "UserDefined"
    , 0
};


struct StoredInEEPROM
{
    StoredInEEPROM()
        : joystick(0)
        , up_key(   KEY_UP_ARROW)
        , down_key( KEY_DOWN_ARROW)
        , left_key( KEY_LEFT_ARROW)
        , right_key(KEY_RIGHT_ARROW)
        , fire1_key(KEY_PAGE_UP)
        , fire2_key(KEY_PAGE_DOWN)
        , fire_auto_delay(0)
        , fire_auto_repeat(0)
        , debounce_time(20)
        , delay_time(10)
        , repetition_time(10)
        , mouse_speed(5)
        , baud(115200)
    {
        for (int i=0; i<Pins::last;++i)
        {
            pins[i] = 0;
        }
    }

    bool set(int key, int value);
    int  get(int key);
    int  size();

    uint8_t joystick;
    uint8_t up_key;
    uint8_t down_key;
    uint8_t left_key;
    uint8_t right_key;
    uint8_t fire1_key;
    uint8_t fire2_key;
    uint16_t fire_auto_delay;
    uint16_t fire_auto_repeat;
    uint16_t debounce_time;
    uint16_t delay_time;
    uint16_t repetition_time;
    uint16_t mouse_speed;
    uint32_t baud;
    uint8_t  pins[10];
};

StoredInEEPROM stored;

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
        len = stored.size();
        char *fBlock = (char*) &stored;
        for (int j=0; j<len; ++j, ++i)
        {
            fBlock[j] = EEPROM.read(i);
        }
    }

    if (stored.joystick == Joystick::Msx_Joystick)
    {
        // -----------------------
        // \ U   D   L   R   VCC /
        //  \  F1  F2 Sel GND   /
        //   ------------------
        Pins::No[Pins::up]     = Pins::PinCon[0];
        Pins::No[Pins::down]   = Pins::PinCon[1];
        Pins::No[Pins::left]   = Pins::PinCon[2];
        Pins::No[Pins::right]  = Pins::PinCon[3];
        Pins::No[Pins::f1]     = Pins::PinCon[5];
        Pins::No[Pins::f2]     = Pins::PinCon[6];
        Pins::No[Pins::vdd]    = Pins::PinCon[4];
        Pins::No[Pins::gnd]    = Pins::PinCon[8];
        Pins::No[Pins::sel]    = Pins::PinCon[7];
    }
    else if (stored.joystick == Joystick::Sega3and6)
    {
        // -----------------------
        // \ U   D   L   R   VCC /
        //  \ F1  Sel GND  F2   /
        //   ------------------
        Pins::No[Pins::up]     = Pins::PinCon[0];
        Pins::No[Pins::down]   = Pins::PinCon[1];
        Pins::No[Pins::left]   = Pins::PinCon[2];
        Pins::No[Pins::right]  = Pins::PinCon[3];
        Pins::No[Pins::vdd]    = Pins::PinCon[4];
        Pins::No[Pins::f1]     = Pins::PinCon[5];
        Pins::No[Pins::sel]    = Pins::PinCon[6];
        Pins::No[Pins::gnd]    = Pins::PinCon[7];
        Pins::No[Pins::f2]     = Pins::PinCon[8];
    }
    else if (stored.joystick == Joystick::AmigaCD32)
    {
        // ------------------------
        // \ U   D   L   R  LATCH /
        //  \ F1  VCC GND  F2    /
        //   -------------------
        Pins::No[Pins::up]     = Pins::PinCon[0];
        Pins::No[Pins::down]   = Pins::PinCon[1];
        Pins::No[Pins::left]   = Pins::PinCon[2];
        Pins::No[Pins::right]  = Pins::PinCon[3];
        Pins::No[Pins::sel]    = Pins::PinCon[4];
        Pins::No[Pins::f1]     = Pins::PinCon[5];
        Pins::No[Pins::vdd]    = Pins::PinCon[6];
        Pins::No[Pins::gnd]    = Pins::PinCon[7];
        Pins::No[Pins::f2]     = Pins::PinCon[8];
    }
    else if (stored.joystick == Joystick::UserDefined)
    {
        for (int i=0; i<Pins::last; ++i)
        {
            Pins::No[i] = stored.pins[i];
        }
    }

    if (stored.joystick != 0)
    {
        ButtonRight.setPin(Pins::No[Pins::right]);
        ButtonLeft.setPin (Pins::No[Pins::left]);
        ButtonUp.setPin   (Pins::No[Pins::up]);
        ButtonDown.setPin (Pins::No[Pins::down]);
        ButtonBtn1.setPin (Pins::No[Pins::f1]);
        ButtonBtn2.setPin (Pins::No[Pins::f2]);
    }

    uint16_t delay_time      = stored.delay_time;
    uint16_t repetition_time = stored.repetition_time;

    pinMode(ConfigPin1, INPUT_PULLUP);
    pinMode(ConfigPin2, INPUT_PULLUP);
    pinMode(ConfigPin3, INPUT_PULLUP);

    int fMode = 0;
    if (digitalRead(ConfigPin2) == LOW) fMode |= 1;
    if (digitalRead(ConfigPin3) == LOW) fMode |= 2;

    switch (fMode)
    {
    case 1:
        isKeyboard = true;
        delay_time = 0;
        repetition_time = 0;
        break;
    case 2:
        isMouse    = true;
        delay_time = 100;
        break;
    case 3:
        isJoystick = true;
        delay_time  = 5;
        repetition_time = 5;
        break;
    case 0:
        isSerialConfig = true;
        Serial.begin(stored.baud);
        break;
    }

    // sent GND pin
    pinMode(Pins::No[Pins::gnd], OUTPUT);
    digitalWrite(Pins::No[Pins::gnd], 0);
    if (Pins::No[Pins::sel])
    {
        pinMode(Pins::No[Pins::sel], OUTPUT);
        digitalWrite(Pins::No[Pins::sel], 0);
    }
    if (Pins::No[Pins::vdd])
    {
        pinMode(Pins::No[Pins::vdd], OUTPUT);
        digitalWrite(Pins::No[Pins::vdd], 1);
    }

    ButtonRight.setDeBounce(stored.debounce_time);
    ButtonLeft.setDeBounce(stored.debounce_time);
    ButtonUp.setDeBounce(stored.debounce_time);
    ButtonDown.setDeBounce(stored.debounce_time);

    ButtonBtn1.setDeBounce(stored.debounce_time);
    ButtonBtn2.setDeBounce(stored.debounce_time);

    if (stored.fire_auto_delay && stored.fire_auto_repeat)
    {
        ButtonBtn1.setDelay(stored.fire_auto_delay);
        ButtonBtn1.setRepeat(stored.fire_auto_repeat);
        ButtonBtn2.setDelay(stored.fire_auto_delay);
        ButtonBtn2.setRepeat(stored.fire_auto_repeat);
    }

    if (isJoystick || isMouse)
    {
        ButtonRight.setDelay(delay_time);
        ButtonRight.setRepeat(repetition_time);
        ButtonLeft.setDelay(delay_time);
        ButtonLeft.setRepeat(repetition_time);
        ButtonUp.setDelay(delay_time);
        ButtonUp.setRepeat(repetition_time);
        ButtonDown.setDelay(delay_time);
        ButtonDown.setRepeat(repetition_time);
    }
#ifndef EMULATED
    if (isJoystick)
    {
        Joystick.begin(false);
        Joystick.setXAxis(XAxis);
        Joystick.setYAxis(YAxis);
        Joystick.setZAxis(0);
        Joystick.sendState();
    }
    if (isKeyboard)
    {
        Keyboard.begin();
    }
    if (isMouse)
    {
        Mouse.begin();
    }
#endif
}


void loop()
{
    if (isSerialConfig)
    {
        String fSentence = read_from_serial();
        check_sentence(fSentence);
    }
    else
    {
        unsigned long fNow = millis();
        ButtonRight.tick(fNow);
        ButtonLeft. tick(fNow);
        ButtonUp.   tick(fNow);
        ButtonDown. tick(fNow);
        ButtonBtn1. tick(fNow);
        ButtonBtn2. tick(fNow);
    }
}

void triggerButton(uint8_t aState, uint8_t aPin) 
{
#ifdef EMULATED
    (void) aState;
    (void) aPin;
#else
    bool button_pressed = aState == Button::pressed;
    int mouseX = 0;
    int mouseY = 0;
    switch (aState)
    {
    case Button::pressed:
    case Button::delayed:
    case Button::repeated:
        if (isJoystick)
        {
            if (aPin == Pins::No[Pins::right] && XAxis <  127) ++XAxis;
            if (aPin == Pins::No[Pins::left]  && XAxis > -127) --XAxis;
            if (aPin == Pins::No[Pins::up]    && YAxis <  127) ++YAxis;
            if (aPin == Pins::No[Pins::down]  && YAxis > -127) --YAxis;

            if (aPin == Pins::No[Pins::f1])
            {
                if (aState != Button::pressed) Joystick.setButton(0, 0);
                Joystick.setButton(0, 1);
            }
            if (aPin == Pins::No[Pins::f2])
            {
                if (aState != Button::pressed) Joystick.setButton(1, 0);
                Joystick.setButton(1, 1);
            }
        }
        else if (isKeyboard)
        {
            if (aPin == Pins::No[Pins::right]) Keyboard.press(stored.right_key);
            if (aPin == Pins::No[Pins::left])  Keyboard.press(stored.left_key);
            if (aPin == Pins::No[Pins::up])    Keyboard.press(stored.up_key);
            if (aPin == Pins::No[Pins::down])  Keyboard.press(stored.down_key);
            if (aPin == Pins::No[Pins::f1])
            {
                if (!button_pressed) Keyboard.release(stored.fire1_key);
                Keyboard.press(stored.fire1_key);
            }
            if (aPin == Pins::No[Pins::f2])
            {
                if (!button_pressed) Keyboard.release(stored.fire2_key);
                Keyboard.press(stored.fire2_key);
            }
        }
        else if (isMouse)
        {
            int depending_speed = button_pressed ? 1 : stored.mouse_speed;
            if (aPin == Pins::No[Pins::right]) mouseX =  depending_speed;
            if (aPin == Pins::No[Pins::left] ) mouseX = -depending_speed;
            if (aPin == Pins::No[Pins::up]   ) mouseY = -depending_speed;
            if (aPin == Pins::No[Pins::down] ) mouseY =  depending_speed;
            if (aPin == Pins::No[Pins::f1])
            {
                if (!button_pressed) Mouse.release(MOUSE_LEFT);
                Mouse.press(MOUSE_LEFT);
            }
            if (aPin == Pins::No[Pins::f2])
            {
                if (!button_pressed) Mouse.release(MOUSE_RIGHT);
                Mouse.press(MOUSE_RIGHT);
            }
        }
        break;
    case Button::released:
        if (isJoystick)
        {
            if (aPin == Pins::No[Pins::right]) XAxis = 0;
            if (aPin == Pins::No[Pins::left] ) XAxis = 0;
            if (aPin == Pins::No[Pins::up]   ) YAxis = 0;
            if (aPin == Pins::No[Pins::down] ) YAxis = 0;
            if (aPin == Pins::No[Pins::f1])    Joystick.setButton(0, 0);
            if (aPin == Pins::No[Pins::f2])    Joystick.setButton(1, 0);
        }
        else if (isKeyboard)
        {
            if (aPin == Pins::No[Pins::right]) Keyboard.release(stored.right_key);
            if (aPin == Pins::No[Pins::left])  Keyboard.release(stored.left_key);
            if (aPin == Pins::No[Pins::up])    Keyboard.release(stored.up_key);
            if (aPin == Pins::No[Pins::down])  Keyboard.release(stored.down_key);
            if (aPin == Pins::No[Pins::f1])    Keyboard.release(stored.fire1_key);
            if (aPin == Pins::No[Pins::f2])    Keyboard.release(stored.fire2_key);
        }
        else if (isMouse)
        {
            if (aPin == Pins::No[Pins::f1])  Mouse.release(MOUSE_LEFT);
            if (aPin == Pins::No[Pins::f2])  Mouse.release(MOUSE_RIGHT);
        }
        break;
    }

    if (isJoystick)
    {
        Joystick.setXAxis(XAxis);
        Joystick.setYAxis(YAxis);
        Joystick.sendState();
    }
    if (isMouse && (mouseX || mouseY))
    {
        Mouse.move(mouseX, mouseY);
    }
#endif
}

const char* read_from_serial()
{
    static const uint16_t gBufferLen = 128;
    static char gBuffer[gBufferLen];
    static int  gBufferPos = 0;
    static unsigned int  gLastTime = 0;
    static uint16_t  g_ms_per_byte = 2;

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

bool StoredInEEPROM::set(int key, int value)
{
    bool result = true;
    switch (key)
    {
    case Keys::joystick:
        if (value >= Joystick::First && value < Joystick::Last)
        {
            joystick = value;
        }
        else
        {
            result = false;
        }
        break;
    case Keys::up_key:            up_key = value; break;
    case Keys::down_key:          down_key = value; break;
    case Keys::left_key:          left_key = value; break;
    case Keys::right_key:         right_key = value; break;
    case Keys::fire1_key:         fire1_key = value; break;
    case Keys::fire2_key:         fire2_key = value; break;
    case Keys::fire_auto_delay:   fire_auto_delay = value; break;
    case Keys::fire_auto_repeat:  fire_auto_repeat = value; break;
    case Keys::debounce_time:     debounce_time = value; break;
    case Keys::delay_time:        delay_time = value; break;
    case Keys::repetition_time:   repetition_time = value; break;
    case Keys::mouse_speed:       mouse_speed = value; break;
    case Keys::baud:              baud  = value; break;
    default: result = false;
    }
    return result;
}

int StoredInEEPROM::get(int key)
{
    switch (key)
    {
    case Keys::joystick:          return joystick;
    case Keys::up_key:            return up_key;
    case Keys::down_key:          return down_key;
    case Keys::left_key:          return left_key;
    case Keys::right_key:         return right_key;
    case Keys::fire1_key:         return fire1_key;
    case Keys::fire2_key:         return fire2_key;
    case Keys::fire_auto_delay:   return fire_auto_delay;
    case Keys::fire_auto_repeat:  return fire_auto_repeat;
    case Keys::debounce_time:     return debounce_time;
    case Keys::delay_time:        return delay_time;
    case Keys::repetition_time:   return repetition_time;
    case Keys::mouse_speed:       return mouse_speed;
    case Keys::baud:              return baud;
    default:                      return None;
    }
}

int StoredInEEPROM::size()
{
    return sizeof(*this);
}

void check_sentence(String& fString)
{
    if (fString.length())
    {
        fString.toUpperCase();
        int fPos = fString.indexOf(':', 0);

        String fKeyWord;
        String fNumber;
        if (fPos > 0)
        {
            fKeyWord = fString.substring(0, fPos);
            fNumber  = fString.substring(fPos+1);
        }
        else
        {
            fKeyWord = fString;
        }

        int fNo = 0;
        int fKey = findKey(fKeyWord, gKeywords, fNo);
        if (fKey == None && !isDigit(fKeyWord[0]) && fNo != Request)
        {
            Serial.print(fKeyWord);
            Serial.println(F(": unknown key"));
        }
        if (fNo == Request)
        {
            int value = stored.get(fKey);
            if (value != None)
            {
                Serial.print(fKeyWord);
                Serial.print(F(": "));
                Serial.println(value);
                if (fKey == Keys::joystick)
                {
                    Serial.println(Joystick::Name[stored.joystick]);
                    String line_1 = "\\";
                    String line_2 = " \\  ";
                    int pin_array[Pins::last];
                    for (int i=0; i<Pins::last; ++i)
                    {
                        pin_array[i] = Pins::nc;
                    }
                    for (int i=0; i<Pins::last; ++i)
                    {
                        int pin = find_pin(Pins::No[i]);
                        if (pin != Pins::nc)
                        {
                            pin_array[pin] = i;
                        }
                    }
                    int i;
                    for (i=0; i<5; ++i)
                    {
                        line_1 += fillStringWithSpaces(Pins::Name[pin_array[i]], 6);
                    }
                    for (; i<Pins::last; ++i)
                    {
                        line_2 += fillStringWithSpaces(Pins::Name[pin_array[i]], 6);
                    }
                    line_1 += " /";
                    line_2 += "   /";
                    Serial.println(line_1);
                    Serial.println(line_2);
                }
            }
        }
        else
        {
            if (fKey == Keys::pins)
            {
                if (fNo > Pins::first && fNo <= Pins::last)
                {
                    int fDummy;
                    fKey = findKey(fNumber, Pins::Name, fDummy);
                    if (fKey != None && fKey != Pins::nc)
                    {
                        stored.pins[fKey] = Pins::PinCon[fNo-1];
                        Serial.print(fKeyWord);
                        Serial.print(F(" <- "));
                        Serial.println(Pins::Name[fKey]);
                    }
                    else
                    {
                        for (int i=0; i<Pins::last; ++i)
                        {
                            stored.pins[i] = Pins::nc;
                        }
                        Serial.println(F("Reset all user defined pins to NC"));
                    }
                }
            }
            else if (findKey(fNumber, gKeywords, fNo) == None && fNo != None )
            {
                if (stored.set(fKey, fNo))
                {
                    Serial.print(fKeyWord);
                    Serial.print(F(" <- "));
                    Serial.println(fNo);
                    if (fKey == Keys::joystick)
                    {
                        Serial.println(Joystick::Name[fNo]);
                    }
                }
                else
                {
                    Serial.println(F("Invalid value"));
                }
            }
            else
            {
                switch (fKey)
                {
                case Keys::store_eeprom:
                {
                    int i, len = strlen(gEEPROMid);
                    for (i=0; i<len; ++i)
                    {
                        EEPROM.write(i, gEEPROMid[i]);
                    }
                    if (i==len)
                    {
                        len = stored.size();
                        char *fBlock = (char*) &stored;
                        for (int j=0; j<len; ++j, ++i)
                        {
                            EEPROM.write(i, fBlock[j]);
                        }
                        Serial.println(F("Stored to EEPROM"));
                    }
                 }   break;
                case Keys::clear_eeprom:
                    EEPROM.write(0, ' ');
                    Serial.println(F("Cleared EEPROM"));
                    break;
                case Keys::help:
                    Serial.println(F("Usage:"));
                    Serial.println(F("- keyword:value -> set a value"));
                    Serial.println(F("- keyword? -> request value"));
                    Serial.println(F("Keywords:"));
                    for (int i=0; gKeywords[i]; ++i)
                    {
                        Serial.print(F("- "));
                        Serial.print(gKeywords[i]);
                        switch (i)
                        {
                        case Keys::pins:
                            Serial.println(F("[No]:key No -> pin number 1 - 9, keys:"));
                            for (int i=0; i<Pins::last; ++i)
                            {
                                Serial.println(String(F("  - ")) + Pins::Name[i]);
                            }
                            Serial.println(F("- PIN[any number]:NC resets all pins to NC"));
                            break;
                        case Keys::help:         Serial.println(F(": prints this help")); break;
                        case Keys::store_eeprom: Serial.println(F(": stores settings to EEPROM")); break;
                        case Keys::clear_eeprom: Serial.println(F(": clears settings to default")); break;
                        default:  Serial.println(F(": value")); break;
                        }
                    }
                    break;
                default:
                     Serial.println(F("Invalid value"));
                    break;
                }
            }
        }
    }
}

String fillStringWithSpaces(const char* aStr, unsigned int aLen)
{
    String fStr(aStr);
    bool left = true;
    while (fStr.length() < aLen)
    {
        if (left) fStr = " " + fStr;
        else      fStr += " ";
        left = !left;
    }
    return fStr;
}

int find_pin(int pin)
{
    for (int i=0; i<Pins::last; ++i)
    {
        if (pin == Pins::PinCon[i]) return i;
    }
    return Pins::nc;
}
