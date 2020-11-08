#include "Melody.h"
#include <Arduino.h>


// \brief constructor with parameters
// outputPin No
// pTones array with tones
// repeats of melody
// tone_ms length of a full tone
Melody::Melody(uint8_t outputPin, Tone* pTones, uint8_t repeats, unsigned long tone_ms):
  mOutputPin(outputPin)
, mRepeats(repeats)
, mCurrentRepeat(repeats)
, mpTones(pTones)
, mCurrentTone(0)
, mToneOn(0)
, mTone_ms(tone_ms)
, mLast_ms(0)
, mCurrent_ms(0)
, mNextTime_ms(0)
{
}

// \brief controlles playing the melody using current time ticks
// fNow_ms current time determined by millis()
void Melody::tick(unsigned long fNow_ms)
{
  unsigned long fStep_ms = fNow_ms - mLast_ms;
  mLast_ms = fNow_ms;
  mCurrent_ms += fStep_ms;
  if (mCurrent_ms > mNextTime_ms && mCurrentRepeat < mRepeats)
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
        mNextTime_ms = mCurrent_ms + fLength_ms;
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

// \brief sets a new array of tones
// aTones Array with tones and time description
// \hint the last entry must contain zeros
void Melody::setTones(Tone* aTones)
{
    mpTones = aTones;
}

// \brief starts playing the melody
void Melody::startMelody()
{
  mCurrent_ms = 0;
  mNextTime_ms = 0;
  mCurrentRepeat = 0;
  mCurrentTone = 0;
  mToneOn = 0;
}

// \brief stops playing the melody
void Melody::stopMelody()
{
  mCurrentRepeat = mRepeats;
}

// \brief determines status of playing
bool Melody::isPlaying()
{
  return mCurrentRepeat != mRepeats;
}
