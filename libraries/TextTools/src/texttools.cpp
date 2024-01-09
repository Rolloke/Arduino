
#include "Arduino.h"
#include "texttools.h"


int findKey(const String& aKeyWord, const char* const aKeywords[], int& aNo)
{
  int fPos = aKeyWord.indexOf('[');
  if (fPos != -1)
  {
    ++fPos;
    bool fAnalogPin = aKeyWord.charAt(fPos) == 'A';
    if (fAnalogPin) ++fPos;
    aNo = aKeyWord.substring(fPos).toInt();
    if (fAnalogPin)
    {
      switch (aNo)
      {
        case  0: aNo = A0; break;
        case  1: aNo = A1; break;
        case  2: aNo = A2; break;
        case  3: aNo = A3; break;
        case  4: aNo = A4; break;
        case  5: aNo = A5; break;
        case  6: aNo = A6; break;
        case  7: aNo = A7; break;
      }
    }
  }
  else if (aKeyWord.indexOf('?') != -1)
  {
    aNo = Request;
  }
  else
  {
    aNo = None;
  }
  for (int i=0; aKeywords[i] != 0; ++i)
  {
    if (aKeyWord.indexOf(aKeywords[i]) == 0)
    {
      return i;
    }
  }
  if (aNo == None)
  {
    // feature! this may contain a unit like m to convert length to ms
    if (isDigit(aKeyWord[0]))
    {
      aNo = aKeyWord.toInt();
    }
  }
  return None;
}


String printFloat(float aValue, int aPre, int aPost, bool aSign)
{
  String fFloat;
  if (aSign)
  {
    if (aValue >= 0) 
    {
      fFloat.concat('+');
    }
    else
    {
      fFloat.concat('-');
      aValue = -aValue;
    }
  }
  int fIntVal = (int) fabs(aValue);
  if (aPre)
  {
    int fDecimals = pow(10, aPre-1) -1;
    while (fIntVal < fDecimals)
    {
      fFloat.concat('0');
      fDecimals = fDecimals / 10;
    }
  }
  fFloat.concat(fIntVal);
  if (aPost)
  {
    int fDecimals = pow(10, aPost);
    aValue = (aValue - (float)fIntVal) * (float)fDecimals;
    fIntVal = fabs(aValue)+0.5;
    fFloat.concat('.');
    fDecimals = (fDecimals - 1) / 10;
    while (fIntVal < fDecimals)
    {
      fFloat.concat('0');
      fDecimals = fDecimals / 10;
    }
    fFloat.concat(fIntVal);
  }
  return fFloat;
}

