#ifndef Keywords_h
#define Keywords_h

enum eKeyId
{
  Request  = -2,
  None     = -1, 
};

int    findKey(const String& aKeyWord, const char* const aKeywords[], int& aNo);

String printFloat(float aValue, int aPre, int aPost, bool aSign=false);


#endif // Keywords_h
