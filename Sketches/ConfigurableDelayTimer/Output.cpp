
#include "Output.h"

unsigned long OutputPin::mTick = 0;

OutputPin::OutputPin() : 
    mPin(0),
    mType(Binary),
    mDelay_ms(0),
    mPulse_ms(0),
    mValue(0),
    mRepeat(0),
    mNextTask(none),
    mNextTaskTime(0),
    mConnection(0)
{

}

void OutputPin::tick(unsigned long aNow)
{
    mTick = aNow;
    if (mPin && mNextTask != none && aNow >= mNextTaskTime)
    {
        switch(mNextTask)
        {
        case set_on:
            set_value(mValue);
            setOn(true);
            if (mPulse_ms)
            {
                mNextTask = set_off;
                mNextTaskTime = aNow + mPulse_ms;
            }
            break;
        case set_off:
            set_value(0);
            setOn(false);
            if (mRepeat != 0)
            {
                mNextTask = set_on;
                mNextTaskTime = aNow + mDelay_ms;
                if (mRepeat > 0) --mRepeat;
            }
            break;
        case none: break;
        case Stop: break;
        }
    }
}

void OutputPin::setPin(uint8_t aPin, OutputPin::type aType)
{
    mPin    = aPin;
    pinMode(mPin, OUTPUT);
    mType = aType;
}

void OutputPin::init()
{
    if (mPin)
    {
        pinMode(mPin, OUTPUT);
        setValue(mValue, true);
    }
}

void OutputPin::setConnection(OutputPin* aPin)
{
    mConnection = aPin;
}

void OutputPin::setOn(bool aOn)
{
    if (aOn)
    {
        mType |= On;
    }
    else
    {
        mType &= ~On;
    }
}

bool OutputPin::isOn() const
{
    return (mType & On) != 0;
}

void OutputPin::setInvert(bool aInvert)
{
    if (aInvert)
    {
        mType |= Invert;
    }
    else
    {
        mType &= ~Invert;
    }
}

bool OutputPin::doInvert() const
{
    return (mType & Invert) != 0;
}

void OutputPin::setToggle(bool aToggle)
{
    if (aToggle)
    {
        mType |= Toggle;
    }
    else
    {
        mType &= ~Toggle;
    }
}

void OutputPin::setEdge(uint8_t aEdge)
{
    mType &= ~Change;
    mType |= aEdge;
}

bool OutputPin::doToggle() const 
{
    return (mType & Toggle) != 0;
}

bool OutputPin::isBinary()
{
    return (mType & Binary) != 0;
}

bool OutputPin::isAnalog()
{
    return (mType & Analog) != 0;
}

uint8_t OutputPin::getEgde() const
{
    return mType & Change;
}

void OutputPin::setValue(uint16_t aValue, int aTrigger)
{
    if (aTrigger)
    {
        if ((mValue == 0 && aValue != 0) || aTrigger == Analog)
        {
            mValue = aValue;
        }
        mNextTask = none;
        if (doToggle() && aValue != 0)
        {
            setOn(!isOn());
            set_value(isOn() ? mValue : 0);
        }
        else if (mDelay_ms > 0 && mPulse_ms > 0)
        {
            mNextTask = aValue != 0 ? set_on : set_off;
            mNextTaskTime = mTick + mDelay_ms;
            if      (mRepeatStored == 0) mRepeat = 0;
            else if (mRepeatStored <  0) mRepeat = -1;
            else                         mRepeat = mRepeatStored-1;
        }
        else if (mDelay_ms)
        {
            mNextTask = aValue != 0 ? set_on : set_off;
            mNextTaskTime = mTick + mDelay_ms;
        }
        else
        {
            set_value(aValue != 0 ? mValue : 0);
        }
    }
    else
    {
        mValue = aValue;
    }
}

uint16_t OutputPin::getValue() const
{
    switch (mType & TypeMask)
    {
    case Binary: return digitalRead(mPin); // 0, 1
    case Analog: return mValue;  // 0,...,255
    case Tone:   return mValue;
    }
    return 0;
}

void OutputPin::setDelay(uint16_t aDelay_ms)
{
    mDelay_ms = aDelay_ms;
}

void OutputPin::setPulse(uint16_t aPulse_ms)
{
    mPulse_ms = aPulse_ms;
}

void OutputPin::setRepeat(int16_t aRepeat)
{
    if (aRepeat == 0)
    {
        mRepeatStored = mRepeat = aRepeat;
    }
    else
    {
        mRepeatStored = aRepeat;
        setValue(mValue != 0 ? mValue : 0);
    }
}

void OutputPin::set_value(uint16_t aValue)
{
    if (mConnection)
    {
        switch (mType&Change)
        {
        case Rising:
            if (aValue != 0) mConnection->setValue(aValue, true);
            break;
        case Falling:
            if (aValue == 0) mConnection->setValue(1, true);
            break;
        default: case Change:
            mConnection->setValue(aValue, true);
            break;
        }
    }

    switch (mType & TypeMask)
    {
    case Binary:
        if (doInvert()) aValue = !aValue;
        digitalWrite(mPin, aValue != 0 ? HIGH : LOW);
        break;
    case Analog:
        if (doInvert()) aValue = max_analog_out - aValue;
        analogWrite(mPin, aValue);
        break;
    case Tone:
        if (aValue)
        {
            if (mPulse_ms) tone(mPin, aValue, mPulse_ms);
            else           tone(mPin, aValue);
        }
        else        noTone(mPin);
        break;
    }
}


String OutputPin::info(bool aShort) const
{
    String fInfo = F("Output Pin: ");
    fInfo += String((int)mPin);
    fInfo += ((mType&TypeMask) == Binary ? ": binary" : ((mType&TypeMask) == Analog) ? ": analog" : ": tone" );
    if (!aShort)
    {
        if (mDelay_ms)
        {
            fInfo += F(", Delay: ");
            fInfo += String(mDelay_ms);
            fInfo += F(" ms");
        }
        if (mPulse_ms)
        {
            fInfo += F(", Pulse: ");
            fInfo += String(mPulse_ms);
            fInfo += F(" ms");
        }
        if (doInvert())    fInfo += F(", Inverted");
        if (doToggle())    fInfo += F(", Toggle");
        switch (getEgde())
        {
        case Change:  fInfo += F(", Egde: change"); break;
        case Rising:  fInfo += F(", Egde: rising"); break;
        case Falling: fInfo += F(", Egde: falling"); break;
        }
        if (mRepeatStored)
        {
            fInfo += F(", Repeat: ");
            fInfo += String(mRepeatStored);
        }
        fInfo += F(", Value: ");
        fInfo += String(getValue());
        if (mConnection)
        {
            fInfo += F(", Connected to: ");
            fInfo += mConnection->info(true);
        }
    }
    return fInfo;
}

uint8_t  OutputPin::pin() const
{
    return mPin;
}
