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

Copyright (c) 2018 RkSoft LLC. All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
