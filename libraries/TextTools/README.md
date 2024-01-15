## TextTools Library for Arduino 

The TextTools library provides some text tools for a line parser to parse commands
with the following syntax e.g.: KEYWORD1[1]:KEYWORD2:KEYWORD3:NUMBER

- int    findKey(const String& aKeyWord, const char* aKeywords[], int& aNo);
  Searches for a keyword in a given list of keywords and returns the index or -1,
  if the keyword is not existing. It also returnes a number within square brackets
  behind the keyword: KEYWORD[aNo].
  
- String printFloat(float aValue, int aPre, int aPost, bool aSign=false);
  Prints a floating point value into a String object with a given number of 
  pre and post comma leading zeros.

### License 

see File: LICENSE

