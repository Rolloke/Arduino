#include "LiquidCrystal.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include <mainwindow.h>

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

LiquidCrystal::LiquidCrystal(uint8_t rs,  uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  init(1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

void LiquidCrystal::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
			 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    pinMode(rs, OUTPUT);
    // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
    if (rw != 255)
    {
      pinMode(rw, OUTPUT);
    }
    pinMode(enable, OUTPUT);

    uint8_t _data_pins[8];
    _data_pins[0] = d0;
    _data_pins[1] = d1;
    _data_pins[2] = d2;
    _data_pins[3] = d3;
    _data_pins[4] = d4;
    _data_pins[5] = d5;
    _data_pins[6] = d6;
    _data_pins[7] = d7;

    // Do these once, instead of every time a character is drawn for speed reasons.
    for (int i=0; i<((fourbitmode) ? 4 : 8); ++i)
    {
      pinMode(_data_pins[i], OUTPUT);
    }


    begin(16, 1);
    mDisplay = true;
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t)
{
    mText.clear();
    mText.resize(lines);
    for (auto& fLine : mText)
    {
        fLine.resize(cols, ' ');
    }

    setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

    mChanged    = true;
    mBlink      = false;
    mBlinkState = true;
    mCursor     = false;
    mCursorChar = 0;
    mAutoScroll = false;

    if (gMainWindowPointer)
    {
        gMainWindowPointer->setLiquidCrystal(this);
    }
}

void LiquidCrystal::setRowOffsets(int row0, int row1, int row2, int row3)
{
  _row_offsets[0] = row0;
  _row_offsets[1] = row1;
  _row_offsets[2] = row2;
  _row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void LiquidCrystal::clear()
{
    for (auto fLine : mText)
    {
        std::fill(fLine.begin(), fLine.end(), ' ');
    }
    mChanged = true;
}

void LiquidCrystal::home()
{
    mColumnCursor = mRowCursor = 0;
}

void LiquidCrystal::setCursor(uint8_t aCol, uint8_t aRow)
{
    mColumnCursor = aCol;
    mRowCursor    = aRow;
}

// Turn the display on/off (quickly)
void LiquidCrystal::noDisplay()
{
    mDisplay = false;
}

void LiquidCrystal::display()
{
    mDisplay = true;
}

// Turns the underline cursor on/off
void LiquidCrystal::noCursor()
{
    mCursor = false;
}

void LiquidCrystal::cursor()
{
    mCursor = true;
}

// Turn on and off the blinking cursor
void LiquidCrystal::noBlink()
{
    mBlink = false;
    mBlinkState = true;
}

void LiquidCrystal::blink()
{
    mBlink = true;
    mCursorChar = 0;
}

// These commands scroll the display without changing the RAM
void LiquidCrystal::scrollDisplayLeft(void)
{
}

void LiquidCrystal::scrollDisplayRight(void)
{
}

// This is for text that flows Left to Right
void LiquidCrystal::leftToRight(void)
{
}

// This is for text that flows Right to Left
void LiquidCrystal::rightToLeft(void)
{
}

// This will 'right justify' text from the cursor
void LiquidCrystal::autoscroll(void)
{
}

// This will 'left justify' text from the cursor
void LiquidCrystal::noAutoscroll(void)
{
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal::createChar(uint8_t /* location */, uint8_t [] /*charmap[]*/)
{

}

/*********** mid level commands, for sending data/cmds */

inline size_t LiquidCrystal::write(uint8_t value)
{
    if (mRowCursor < mText.size())
    {
        if (mColumnCursor < mText[mRowCursor].size())
        {
            mText[mRowCursor][mColumnCursor++] = value;
            mChanged = true;
        }
    }
    return 1; // assume sucess
}

bool LiquidCrystal::isChanged() const
{
    return mChanged;
}

void LiquidCrystal::setUnchanged()
{
    mChanged = false;
}

int LiquidCrystal::getLines() const
{
    return mText.size();
}

const std::string& LiquidCrystal::getDisplayTextLine(int aLine) const
{
    static const std::string fDummy;
    bool fDisplay = true;
    if (mBlink)
    {
        fDisplay = mBlinkState;
    }
    if (mDisplay && fDisplay)
    {
        if (aLine < static_cast<int>(mText.size()))
        {
            return mText[aLine];
        }
    }
    return fDummy;
}

bool LiquidCrystal::getCursor(uint8_t &aRow, uint8_t& aCol)
{
    aRow = mRowCursor;
    aCol = mColumnCursor;
    return mCursor;
}

void LiquidCrystal::updateBlinkState()
{
    if (mBlink)
    {
        mBlinkState = not mBlinkState;
        mChanged = true;
    }
    if (mCursor)
    {
#if 1
        mCursorChar = mCursorChar != 0 ? 0 : 1;
#else
        if (mCursorChar)
        {
            mText[mRowCursor][mColumnCursor] = mCursorChar;
            mCursorChar = 0;
        }
        else
        {
            mCursorChar = mText[mRowCursor][mColumnCursor];
            mText[mRowCursor][mColumnCursor] = ' ';
        }
        mChanged = true;
#endif
    }
}
