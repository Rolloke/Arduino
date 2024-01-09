
#include "Input.h"
#include "Output.h"

uint8_t   InputPin::mPinForISR  = 0;
InputPin* InputPin::mThisForISR = 0;

InputPin::InputPin() : 
mPin(0),
mEdge(no_edge),
mFlags(binary),
mValue(0),
mThreshold(0),
mDeBounce(0),
mRepeat_ms(0),
mDelay_ms(0),
mTime(0),
mConnection(0)
{

}

void InputPin::tick(unsigned long aNow)
{
  if (mPin)
  {
	  get_value(aNow);
  }
}

void InputPin::setPin(uint8_t aPin, bool aBinary)
{
  mPin    = aPin;
  pinMode(mPin, INPUT);
  if (aBinary)
  {
    mFlags = binary;
    mEdge  = change;
  }
  else
  {
    mFlags = analog;
  }
}

void InputPin::init()
{
    if (mPin)
    {
        pinMode(mPin, INPUT);
        if (mEdge & Switch)
        {
            pinMode(mPin, INPUT_PULLUP);
        }
        else if (mEdge & interrupt)
        {
            setEdge(mEdge);
        }
    }
}

bool InputPin::isBinary() const
{
  return (mFlags & binary) != 0;
}

bool InputPin::isAnalog() const
{
  return (mFlags & analog) != 0;
}

bool InputPin::doReport() const
{
  return (mFlags & report) != 0;
}

bool InputPin::isBelowThresholdValue() const
{
  return (mFlags & below_treshold) != 0;
}

void InputPin::setBelowThresholdValue(bool aB)
{
  if (aB)
  {
    mFlags |= below_treshold;
  }
  else
  {
    mFlags &= ~below_treshold;
  }
}

void InputPin::ISRstatic()
{
  mThisForISR->InterruptSR();  
}

void InputPin::InterruptSR()
{
  if      (mEdge&change)        triggerConnection(digitalRead(mPin), true);
  else if (mEdge&(rising|high)) triggerConnection(1, true);
  else if (mEdge&(falling|low)) triggerConnection(1, true);
}

void InputPin::setEdge(uint8_t aEdge)
{
  if (aEdge & Switch)
  {
      mThreshold = released;
      mDeBounce  = default_switch_debounce_ms;
      pinMode(mPin, INPUT_PULLUP);
      mEdge = aEdge|change;
  }
  else if (mEdge & interrupt)
  {
    int mode = CHANGE;
    switch(aEdge & interrupt_mask)
    {
      case rising:  mode = RISING;  break;
      case falling: mode = FALLING; break;
      case low:     mode = LOW;     break;
      case high:    mode = HIGH;    break;
    }
    if (mThisForISR != 0)
    {
      detachInterrupt(digitalPinToInterrupt(mPinForISR));
    }
    mEdge |= aEdge;
    attachInterrupt(digitalPinToInterrupt(mPin), ISRstatic, mode);
    mThisForISR = this;
    mPinForISR  = mPin;
  }
  else
  {
      mEdge &= ~interrupt_mask;
      mEdge |= aEdge;
  }
}

void InputPin::setRepeat(uint16_t aRepeat_ms)
{
  mRepeat_ms = aRepeat_ms;
}

void InputPin::setDelay(uint16_t aDelay_ms)
{
  mDelay_ms = aDelay_ms;
}

void InputPin::setDebounce(uint16_t aDebounce_ms)
{
  mDeBounce = aDebounce_ms;
}

void InputPin::setConnection(OutputPin* aPin)
{
  mConnection = aPin;
}

void InputPin::setThreshold(uint16_t aThreshold)
{
  mThreshold = aThreshold;
  mEdge      = change;
  setBelowThresholdValue(mValue < aThreshold);
}

uint16_t InputPin::getValue() const
{
  return mValue;
}

void InputPin::setValue(uint16_t aValue)
{
  if (mPin)
  {
    digitalWrite(mPin, aValue);
  }
}

void InputPin::setReport(bool aR)
{
  if (aR)
  {
    mFlags |= report;
  }
  else
  {
    mFlags &= ~report;
  }
}

void InputPin::get_value(unsigned long aNow)
{
  if (isBinary())
  {
    // binary --------------------------------------------------------------------
    uint16_t fValue = digitalRead(mPin); // 0, 1
    switch (mEdge)
    {
      // --------------------------------------------------------------------       
      case rising:
        if (fValue != 0 && mValue == 0)
        {
          mValue = fValue;
          triggerConnection(1, true);
        }
        break;
      case falling:
        if (fValue == 0 && mValue != 0)
        {
          mValue = fValue;
          triggerConnection(1, true);
        }
        break;
      case change:
        if (fValue != mValue)
        {
          mValue = fValue;
          triggerConnection(mValue, true);
        }
        break;
      // --------------------------------------------------------------------------       
      // handle as low active switch with internal pullup resistor
      case (Switch|change):
      case (Switch|rising):
      case (Switch|falling):
        if (fValue == LOW) // low means button is pressed
        {
          switch (mThreshold)
          {
            case released:
              mTime = aNow + mDeBounce;
              mThreshold = pressed;
              break;
            // --------------------------------------------------------------------
            case pressed: // button pressed
              if (aNow >= mTime)
              {
                if (mEdge&(rising|change)) triggerConnection(1, true);
                mThreshold = fired;
                if (mDelay_ms > 0)
                {
                  mTime = aNow + mDelay_ms;
                  mThreshold = delayed;
                }
                else if (mRepeat_ms > 0)
                {
                  mTime = aNow + mRepeat_ms;
                  mThreshold = repeated;
                }
              }
              break;
            // --------------------------------------------------------------------
            case delayed: // second trigger
              if (aNow >= mTime)
              {
                triggerConnection(1, true);
                mThreshold = fired;
                if (mRepeat_ms > 0)
                {
                  mTime = aNow + mRepeat_ms;
                  mThreshold = repeated;
                }
              }
              break;
            // --------------------------------------------------------------------
            case repeated: // further triggers
              if (aNow >= mTime)
              {
                triggerConnection(1, true);
                mTime = aNow + mRepeat_ms;
              }
              break;
          }
        }
        else  // fValue == HIGH
        {
          // --------------------------------------------------------------------
          if (mThreshold >= fired)  // release button, if it was fired and change flag is set
          {
              if      (mEdge&change)  triggerConnection(0, true);
              else if (mEdge&falling) triggerConnection(1, true);
          }
          mThreshold = released;
        }
        break;
      default:
        if (mEdge & interrupt) break;
        triggerConnection(1, true);
        break;      
    }
    mValue = fValue;
  }
  else
  {
    // analog --------------------------------------------------------------------
    uint16_t fValue = analogRead(mPin); // 0,...,1023
    if (mThreshold > 0)
    {
      // analog treshold handling --------------------------------------------------
      if (isBelowThresholdValue())
      {
        if (fValue > mThreshold+mDeBounce)
        {
          if (mEdge&(change|rising))
          {
            triggerConnection(1, true);
          }
          setBelowThresholdValue(false);
        }
      }
      else
      {
        if (fValue < mThreshold-mDeBounce)
        {
          if      (mEdge&change)  triggerConnection(0, true);
          else if (mEdge&falling) triggerConnection(1, true);
          setBelowThresholdValue(true);
        }
      }
    }
    else if (doReport() && mRepeat_ms)
    {
      // repeated report of values -------------------------------------------------
      if (aNow >= mTime)
      {
        triggerConnection(fValue);
        mTime = aNow + mRepeat_ms;
      }
    }
    else if (abs(fValue - mValue) > mDeBounce)
    {
      // analog change of value   --------------------------------------------------
      triggerConnection(fValue, analog);
    }
    mValue = fValue;
  }
}

uint32_t InputPin::getPulseLength(bool aHigh, uint32_t aTimeout)
{
  return pulseIn(mPin, aHigh ? HIGH : LOW, aTimeout);
}

void InputPin::triggerConnection(uint16_t fValue, int aTrigger)
{
  if (mConnection)
  {
    mConnection->setValue((mConnection->isAnalog() && isAnalog() && aTrigger == analog) ? fValue / factor_in_out : fValue, aTrigger);
  }
  if (doReport())
  {
    Serial.println(fValue);
  }
}

String InputPin::info(bool aShort) const
{
  String fInfo = F("Input Pin: ");
  fInfo += String((int)mPin);
  fInfo += isBinary() ? F(": binary") : F(": analog");
  if (!aShort)
  {
    if (mEdge & interrupt)
    {
      fInfo += F(", Interrupt: ");
    }
    else if (mEdge&interrupt_mask)
    {
      fInfo += F(", Edge:");
      fInfo += String(mEdge, 16);
    }

    switch ((mEdge&interrupt_mask))
    {
      //case no_edge:fInfo += ": none"; break;
      case rising: fInfo += F(": rising"); break;
      case falling:fInfo += F(": falling"); break;
      case change: fInfo += F(": change"); break;
    }

    if (mEdge&Switch)
    {
      fInfo += F(", Low active switch with internal pullup resistor");
      if (mDeBounce)
      {
        fInfo += F(", Pulse trigger after: ");
        fInfo += String(mDeBounce) + " ms";
      }
      if (mDelay_ms)
      {
        fInfo += F(", Delayed 2nd trigger: ");
        fInfo += String(mDelay_ms) + " ms";
      }
      if (mRepeat_ms)
      {
        fInfo += F(", Repeat trigger: ");
        fInfo += String(mRepeat_ms) + " ms";
      }
    }

    if (mThreshold > 0 && mEdge != Switch)
    {
      fInfo += F(", Threshold Value: ");
      fInfo += String(mThreshold);
      if (mDeBounce)
      {
        fInfo += F(", switch threshold (+-): ");
        fInfo += String(mDeBounce);
      }
      fInfo += F(", value is ");
      if(isBelowThresholdValue()) fInfo += F("below");
      else                        fInfo += F("above");
    }
    if (doReport()) fInfo += F(", reporting to serial com");
    if (mRepeat_ms > 0)
    {
      fInfo += F(" request ");
      fInfo += String(mRepeat_ms);
      fInfo += F(" ms interval");
    }
    fInfo += F(", Value: ");
    fInfo += String(mValue);
    if (mConnection != 0)
    {  
      fInfo += F(", Connected to: ");
      fInfo += mConnection->info(true);
    }
  }
  return fInfo;
}

uint8_t  InputPin::pin() const
{
  return mPin;
}
