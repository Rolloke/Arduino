#include "button.h"
#include <Arduino.h>

// @brief constructor with arguments
// @param aPin Arduino pin number
// @param anEventFnc function triggered by button events
// @param aHighlevel high level for pin (LOW, HIGH)
// @hint  LOW is default and initializes an input with internal pullup
Button::Button(uint8_t aPin, fTriggerFunc anEventFnc, uint8_t aHighlevel) :
    triggerEvent(anEventFnc)
  , mRepeat_ms(0)
  , mDelay_ms(0)
  , mDeBounce_ms(50)
  , mTime(0)
  , mState(released)
  , mPin(aPin)
  , mPins(0)
  , mNoOfPins(0)
  , mHighLevel(aHighlevel)
{
    if (mHighLevel == HIGH)
    {
        pinMode(mPin, INPUT);
    }
    else
    {
        pinMode(mPin, INPUT_PULLUP);
        digitalWrite(mPin, HIGH);
    }
}

// @brief constructor with arguments
// @param aPin array with Arduino pin numbers
// @param aNoOfPins size of the array with pin numbers
// @param anEventFnc function triggered by button events
// @param aHighlevel high level for pin (LOW, HIGH)
// @hint  LOW is default and initializes an input with internal pullup
Button::Button(uint8_t* aPins, uint8_t aNoOfPins, fTriggerFunc anEventFnc, uint8_t aHighlevel) :
    triggerEvent(anEventFnc)
  , mRepeat_ms(0)
  , mDelay_ms(0)
  , mDeBounce_ms(50)
  , mTime(0)
  , mState(released)
  , mPin(0)
  , mPins(aPins)
  , mNoOfPins(aNoOfPins)
  , mHighLevel(aHighlevel)
{
    for (uint8_t fPin=0; fPin<aNoOfPins; ++fPin)
    {
        if (mHighLevel == HIGH)
        {
            pinMode(mPins[fPin], INPUT);
        }
        else
        {
            pinMode(mPins[fPin], INPUT_PULLUP);
            digitalWrite(mPins[fPin], HIGH);
        }
    }
}

// @brief sets repetition trigger time for a pressed button
// @param aRepeat_ms repetition time
void Button::setRepeat(int aRepeat_ms)
{
    mRepeat_ms = aRepeat_ms;
}

// @brief sets delay time for repetion trigger for a pressed button
// @param aDelay_ms delay time before repeated trigger
void Button::setDelay(int aDelay_ms)
{
    mDelay_ms = aDelay_ms;
}

// @brief sets a delay time to debounce a connected switch
// @param aDeBounce_ms debounce delay time
void Button::setDeBounce(int aDeBounce_ms)
{
    mDeBounce_ms = aDeBounce_ms;
}

// \brief controlles trigger events of a button
// @param fNow current time determined by millis()
void Button::tick(unsigned long fNow)
{
    if (isPressed())
    {
        switch (mState)
        {
        case released:
            mTime = fNow + mDeBounce_ms;
            mState = pressed;
            break;
        case pressed:
            if (fNow >= mTime)
            {
                (*triggerEvent)(mState, mPin);
                mState = fired;
                if (mDelay_ms > 0)
                {
                    mTime = fNow + mDelay_ms;
                    mState = delayed;
                }
            }
            break;
        case delayed:
            if (fNow >= mTime)
            {
                (*triggerEvent)(mState, mPin);
                mState = fired;
                if (mRepeat_ms > 0)
                {
                    mTime = fNow + mRepeat_ms;
                    mState = repeated;
                }
            }
            break;
        case repeated:
            if (fNow >= mTime)
            {
                (*triggerEvent)(mState, mPin);
                mTime = fNow + mRepeat_ms;
            }
            break;
        default: break;
        }
    }
    else
    {
        if ((   (mDelay_ms  > 0 && mState == delayed)
                || (mDelay_ms == 0 && mState == fired )
                ) && mPin != 0)
        {
            (*triggerEvent)(released, mPin);
        }
        mState = released;
    }
}

bool Button::isPressed()
{
    if (mPins)
    {
        mPin = 0;
        for (int i=0; i< mNoOfPins; ++i)
        {
            if (digitalRead(mPins[i]) == mHighLevel)
            {
                mPin |= (1 << i);
            }
        }
        return mPin != 0;
    }
    else
    {
        return digitalRead(mPin) == mHighLevel;
    }
}

const char* Button::nameOf(Button::eState aState)
{
    switch (aState)
    {
    case none: return "none";
    case released: return "released";
    case pressed: return "pressed";
    case fired: return "fired";
    case delayed: return "delayed";
    case repeated: return "repeated";
    }
    return "unknown";
}

