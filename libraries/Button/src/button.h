#ifndef Button_h
#define Button_h


/*
 * button.h - library for a button triggering events controled by
 * repeatedly calling the tick(..) function within the function loop().
 */

#include <inttypes.h>


// @brief signature of the trigger function
// @param aState state of the pressed button, see enum eState 
// @param aPin number of the pin or bit mask with set bits acording to the array position
typedef void (*fTriggerFunc)(uint8_t aState, uint8_t aPin);

class Button
{
public:
  enum eState : uint8_t {none, released=1, pressed=2, delayed=4, repeated=8, fired=16 };
  
  Button(uint8_t aPin, fTriggerFunc anEventFnc, uint8_t aHighlevel=0);
  Button(uint8_t* aPins, uint8_t aNoOfPins, fTriggerFunc anEventFnc, uint8_t aHighlevel=0);
	
  void setRepeat(int aRepeat_ms);
  void setDelay(int aDelay_ms);
  void setDeBounce(int aDeBounce_ms);

  void setPinsArray(uint8_t* aPins, uint8_t aNoOfPins);
  
  void tick(unsigned long fNow);
  static const char* nameOf(eState);

  
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
