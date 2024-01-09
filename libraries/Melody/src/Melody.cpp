#include "Melody.h"
#include <Arduino.h>


Melody::Melody(uint8_t outputPin, Tone* pTones, uint8_t repeats, unsigned long tone_ms):
  mOutputPin(outputPin)
, mRepeats(repeats)
, mCurrentRepeat(repeats)
, mpTones(pTones)
, mCurrentTone(0)
, mToneOn(0)
, mTone_ms(tone_ms)
, mNextTime_ms(0)
{
}

void Melody::tick(unsigned long current_ms)
{
  if (current_ms > mNextTime_ms && mCurrentRepeat < mRepeats)
  {
    if (mpTones != 0 && (mpTones[mCurrentTone].mTone != 0 || mpTones[mCurrentTone].mCnt != 0))
    {
      if (!mToneOn)
      {
        unsigned long fLength_ms = mTone_ms * mpTones[mCurrentTone].mCnt / mpTones[mCurrentTone].mDiv;
        if (mpTones[mCurrentTone].mTone)
        {
          tone(mOutputPin, mpTones[mCurrentTone].mTone, fLength_ms);
        }
        mToneOn = 1;
        mNextTime_ms = current_ms + fLength_ms;
      }
      else
      {
        noTone(mOutputPin);
        mToneOn = 0;
        ++mCurrentTone;
      }
    }
    else
    {
      ++mCurrentRepeat;
      mCurrentTone = 0;
    }
  }
}

void Melody::setTones(Tone* aTones)
{
    mpTones = aTones;
}

void Melody::startMelody()
{
  mCurrentRepeat = 0;
  mCurrentTone = 0;
  mToneOn = 0;
}

void Melody::stopMelody()
{
  mCurrentRepeat = mRepeats;
}

bool Melody::isPlaying()
{
  return mCurrentRepeat != mRepeats;
}
