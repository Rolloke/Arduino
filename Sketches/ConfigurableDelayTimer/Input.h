#ifndef InputPin_h
#define InputPin_h

#include <inttypes.h>

#include <Arduino.h>
class OutputPin;

class InputPin
{
  public:
  enum edge  { no_edge, rising=1, falling=2, low=4, high=8, change=0x10, interrupt=0x20, interrupt_mask=0x1f, Switch=0x40 };
  enum state { none, released=1, pressed=2, fired=3, delayed=4, repeated=8 };
  enum flags { binary=1, analog=2, report=4, below_treshold=8 };
  
  InputPin();

  void tick(unsigned long aNow);
  
  void setPin(uint8_t aPin, bool aBinary=true);
  void setConnection(OutputPin* aPin);
  void setEdge(uint8_t aEdge);
  void setThreshold(uint16_t aThreshold);
  void setRepeat(uint16_t aRepeat_ms);
  void setDelay(uint16_t aDelay_ms);
  void setDebounce(uint16_t aDebounce_ms);
  void setValue(uint16_t aValue);
  void setReport(bool aR);
  OutputPin* getConnection()const { return mConnection; }

  uint32_t getPulseLength(bool aHigh, uint32_t aTimeout);
  uint8_t  pin() const;
  uint16_t getValue() const;
  String   info(bool aShort=false) const;
  bool     isAnalog() const;

  void     init();

  private:
  
  void get_value(unsigned long aNow);
  void triggerConnection(uint16_t fValue=1, int aTrigger=false);
  void InterruptSR();
  bool isBinary() const;
  bool doReport() const;
  bool isBelowThresholdValue() const;
  void setBelowThresholdValue(bool aB);
  static void ISRstatic();
  
  uint8_t  mPin;
  uint8_t  mEdge;
  uint8_t  mFlags;
  
  uint16_t mValue;
  int16_t  mThreshold;
  uint16_t mDeBounce;
  uint16_t mRepeat_ms;
  uint16_t mDelay_ms;
  unsigned long mTime;
  
  OutputPin* mConnection;
  static uint8_t   mPinForISR;
  static InputPin* mThisForISR;
};

#endif // InputPin_h
