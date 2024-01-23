#ifndef Button_h
#define Button_h


/*
 * button.h - library for a button triggering events controled by
 * repeatedly calling the tick(..) function within the function loop().
 */

#include <inttypes.h>


//! @brief signature of the trigger function
//! @param aState state of the pressed button, see enum eState 
//! @param aPin number of the pin or bit mask with set bits according to the array position
//! @note for mutliple buttons the release signal has no information about released pin
typedef void (*fTriggerFunc)(uint8_t aState, uint8_t aPin);

class Button
{
public:
  enum eState : uint8_t {none, released=1, pressed=2, delayed=4, repeated=8, fired=16 };
  
  //! @brief constructor with arguments
  //! @param aPin Arduino pin number
  //! @param anEventFnc function triggered by button events
  //! @param aHighlevel high level for pin (LOW, HIGH)
  //! @note  LOW is default and initializes an input with internal pullup
  Button(uint8_t aPin, fTriggerFunc anEventFnc, uint8_t aHighlevel=0);

  //! @brief constructor with arguments
  //! @param aPin array with Arduino pin numbers
  //! @param aNoOfPins size of the array with pin numbers
  //! @param anEventFnc function triggered by button events
  //! @param aHighlevel high level for pin (LOW, HIGH)
  //! @hint  LOW is default and initializes an input with internal pullup
  Button(uint8_t* aPins, uint8_t aNoOfPins, fTriggerFunc anEventFnc, uint8_t aHighlevel=0);
	
  //! @brief sets a delay time to debounce a connected switch
  //! @param aDeBounce_ms debounce delay time
  void setDeBounce(int aDeBounce_ms);

  //! @brief sets delay time for repetion trigger for a pressed button
  //! @param aDelay_ms delay time before repeated trigger
  void setDelay(int aDelay_ms);

  //! @brief sets repetition trigger time for a pressed button
  //! @param aRepeat_ms repetition time
  void setRepeat(int aRepeat_ms);

  //! @brief controles trigger events of a button
  //! @param fNow current time determined by millis()
  //! @note must be called repeatedly within loop()
  void tick(unsigned long fNow);

  //! @brief sets new pin number
  //! @param aPin Arduino pin number
  //! @return success of altering pin
  bool setPin(uint8_t aPin);

  //! @brief sets new pin numbers
  //! @param aPin array with Arduino pin numbers
  //! @param aNoOfPins size of the array with pin numbers
  //! @return success of altering pins
  bool setPins(uint8_t* aPins, uint8_t aNoOfPins);

  //! @brief delivers the name of the button state
  //! @param aState button state
  //! @return name of state
  static const char* nameOf(eState aState);

  
private:
  Button();
  bool isPressed();
  
  fTriggerFunc triggerEvent;
  uint16_t mRepeat_ms;
  uint16_t mDelay_ms;
  uint16_t mDeBounce_ms;
  unsigned long mTime;
  eState mState; 
  uint8_t mPin;
  uint8_t *mPins;
  uint8_t mNoOfPins;
  uint8_t mHighLevel;
};

#endif
