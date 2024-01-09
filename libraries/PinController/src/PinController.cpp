#include "PinController.h"
#include <Arduino.h>

TimerController::TimerController(uint8_t repeats) :
    mRepeats(repeats)
  , mCurrentRepeat(repeats)
  , mCurrentCommand(0)
  , mNextTime_ms(0)
{

}

void TimerController::start()
{
    mCurrentRepeat = 0;
    mCurrentCommand = 0;
}

void TimerController::stop()
{
    mCurrentRepeat = mRepeats;
}

bool TimerController::isPlaying()
{
    return mCurrentRepeat != mRepeats;
}

bool TimerController::do_repeat()
{
    return mCurrentRepeat < mRepeats || (mRepeats == for_ever && mCurrentRepeat != for_ever);
}


DigitalPinController::DigitalPinController(uint8_t* pOutputPins, Command* pCommands, uint16_t commands, uint8_t repeats):
    TimerController(repeats)
  , mpOutputPin(pOutputPins)
  , mpCommands(pCommands)
  , mCommands(commands)
  , mFlags(none)
{
    for (int i=0; mpOutputPin[i] != 0; ++i)
    {
        pinMode(mpOutputPin[i], OUTPUT);
    }
}

void DigitalPinController::tick(unsigned long current_ms)
{
    if (do_repeat())
    {
        if (current_ms >= mNextTime_ms)
        {
            if (mpCommands != 0 && mCurrentCommand >= 0 && mCurrentCommand < mCommands)
            {
                unsigned long fLength_ms = mpCommands[mCurrentCommand].mDelay;
                int i;
                for (i=0; i < 16 && mpOutputPin[i] != 0; ++i)
                {
                    digitalWrite(mpOutputPin[i], bitRead(mpCommands[mCurrentCommand].mBits,i) ? HIGH : LOW);
                }
                if (is_set(common_delay))
                {
                    for ( ; i < 32 && mpOutputPin[i] != 0; ++i)
                    {
                        digitalWrite(mpOutputPin[i], bitRead(mpCommands[mCurrentCommand].mBits,i) ? HIGH : LOW);
                    }
                    mNextTime_ms = current_ms + mDelay;
                }
                else
                {
                    mNextTime_ms = current_ms + fLength_ms;
                }
                if (is_set(reverse))
                {
                    --mCurrentCommand;
                }
                else
                {
                    ++mCurrentCommand;
                }
            }
            else
            {
                if (mRepeats != for_ever)
                {
                    ++mCurrentRepeat;
                }
                if (is_set(reverse))
                {
                    mCurrentCommand = mCommands - 1;
                }
                else
                {
                    mCurrentCommand = 0;
                }
            }
        }
    }
}

void DigitalPinController::setCommands(Command* commands, uint16_t no_of_commands, uint8_t repeats)
{
    mpCommands = commands;
    mCommands  = no_of_commands;
    mRepeats   = repeats;
}

void DigitalPinController::setFlags(uint8_t flags, bool set)
{
    if (set)
    {
        mFlags |= flags;
    }
    else
    {
        mFlags &= ~flags;
    }
}

bool DigitalPinController::is_set(uint8_t flags)
{
    return mFlags & flags ? true : false;
}

void DigitalPinController::setCommonDelay(uint16_t delay)
{
    mDelay = delay;
    setFlags(common_delay, mDelay != 0);
}

void DigitalPinController::set_all_pins(uint8_t value)
{
    for (int i=0; mpOutputPin[i] != 0; ++i)
    {
        digitalWrite(mpOutputPin[i], value);
    }
}


AnalogPinController::AnalogPinController(uint8_t* OutputPins, Function *functions, uint8_t repeats):
    TimerController(repeats)
  , mOutputPins(OutputPins)
  , mpFunctions(functions)
  , mPreviousValue(0)
  , mPreviousTime_ms(0)

{
    for (int i=0; mOutputPins[i] != 0; ++i)
    {
        pinMode(mOutputPins[i], OUTPUT);
    }
}

void AnalogPinController::tick(unsigned long current_ms)
{
    const bool repeat = do_repeat();
    if (repeat)
    {
        if (current_ms > mNextTime_ms)
        {
            if (mpFunctions)
            {
                mPreviousValue   = mpFunctions[mCurrentCommand].mValue;

                switch (mpFunctions[mCurrentCommand].mFunction)
                {
                case Function::set_start_value:
                case Function::constant:
                case Function::linear_ramp:
                    set_value_of_all_pins(mpFunctions[mCurrentCommand].mValue);
                    break;
                 default: break;
                }

                ++mCurrentCommand;
                mPreviousTime_ms = current_ms;
                mNextTime_ms     = current_ms + mpFunctions[mCurrentCommand].mPeriod;
                mPeriodStep      = 1;

                switch (mpFunctions[mCurrentCommand].mFunction)
                {
                case Function::none:
                    if (mRepeats != for_ever)
                    {
                        ++mCurrentRepeat;
                    }
                    mCurrentCommand = 0;
                    break;
                case Function::constant:
                    set_value_of_all_pins(mpFunctions[mCurrentCommand].mValue);
                    break;
                default: break;
                }
            }
        }
        else if (current_ms >= mPreviousTime_ms && current_ms < mNextTime_ms)
        {
            const uint16_t current_time    = current_ms  - mPreviousTime_ms;
            const uint16_t length_ms       = mNextTime_ms - mPreviousTime_ms;
            unsigned long next_change_time = mPreviousTime_ms + 10 * mPeriodStep;
            if (current_ms >= next_change_time)
            {
                ++mPeriodStep;
                int32_t value = 0;
                const float half_pi = 0.5 * M_PI;
                const float one_and_half_pi = half_pi + M_PI;
                const float two_pi  = 2 * M_PI;
                switch (mpFunctions[mCurrentCommand].mFunction)
                {
                case Function::linear_ramp:
                    value = calculate_linear_ramp(current_time, length_ms);
                    break;
                case Function::constant:
                case Function::set_start_value:
                    return;
                case Function::sine:
                    value = 0.5 * (1 + sin(two_pi * current_time / length_ms)) * mpFunctions[mCurrentCommand].mValue;
                    break;
                case Function::sine_half_1:
                    value = sin(M_PI * current_time / length_ms) * mpFunctions[mCurrentCommand].mValue;
                    break;
                case Function::sine_half_2:
                    value = (1 + sin(M_PI + M_PI * current_time / length_ms)) * mpFunctions[mCurrentCommand].mValue;
                    break;
                case Function::sine_quater_1:
                    value = sin(half_pi * current_time / length_ms) * mpFunctions[mCurrentCommand].mValue;
                    break;
                case Function::sine_quater_2:
                    value = sin(half_pi + half_pi * current_time / length_ms) * mpFunctions[mCurrentCommand].mValue;
                    break;
                case Function::sine_quater_3:
                    value = (1 + sin(M_PI + half_pi * current_time / length_ms)) * mpFunctions[mCurrentCommand].mValue;
                    break;
                case Function::sine_quater_4:
                    value = (1 + sin(one_and_half_pi + half_pi * current_time / length_ms)) * mpFunctions[mCurrentCommand].mValue;
                    break;
                case Function::none: break;
                }
                set_value_of_all_pins(value);
            }
        }
    }
}

void AnalogPinController::set_value_of_all_pins(uint16_t value)
{
    for (int i=0; mOutputPins[i] != 0; ++i)
    {
        analogWrite(mOutputPins[i], value);
    }
}

int32_t AnalogPinController::calculate_linear_ramp(uint16_t current_time, uint16_t length_ms)
{
    return map(current_time, 0, length_ms, mPreviousValue, mpFunctions[mCurrentCommand].mValue);
}

void AnalogPinController::setFunctions(Function* functions, uint8_t repeats)
{
    mpFunctions = functions;
    mRepeats = repeats;
}


TimerFunction::TimerFunction(TriggerFunc function, uint16_t timeout, uint8_t repeats) : TimerController(repeats)
{
    setTimerFunction(function, timeout, repeats);
}

void TimerFunction::tick(unsigned long current_ms)
{
    if (do_repeat() && mTriggerFunction)
    {
        if (current_ms >= mNextTime_ms)
        {
            if (!mTriggerFunction())
            {
                stop();
            }
            else
            {
                mNextTime_ms = current_ms + mTimeout;
            }
            if (mRepeats != for_ever)
            {
                ++mCurrentRepeat;
            }
        }
    }
}

void TimerFunction::setTimerFunction(TriggerFunc function, uint16_t timeout, uint8_t repeats)
{
    mNextTime_ms = millis() + timeout;
    mRepeats = repeats;
    mTriggerFunction = function;
}
