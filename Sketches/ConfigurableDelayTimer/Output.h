#ifndef OutputPin_h
#define OutputPin_h

#include <inttypes.h>

#include <Arduino.h>

enum constants
{
  max_analog_out = 256,
  max_analog_in  = 1024,
  factor_in_out  = max_analog_in / max_analog_out,
  default_switch_debounce_ms = 50
};

class OutputPin
{
  enum task { Stop=-1, none, set_on, set_off };
  public:
  
  enum type { Binary=1, Analog=2, Tone=4, Invert=8, Toggle=0x10, On=0x20, Rising=0x40, Falling=0x80, Change=Rising|Falling, TypeMask=Binary|Analog|Tone };

  OutputPin();

  void tick(unsigned long aNow);
  
  void setPin(uint8_t aPin, type aType);

  void setToggle(bool aToggle);
  void setInvert(bool aInvert);
  void setValue(uint16_t aValue, int aTrigger=false);
  void setDelay(uint16_t aDelay_ms);
  void setPulse(uint16_t aPulse_ms);
  void setRepeat(int16_t aValue = Stop);
  void setConnection(OutputPin* aPin);
  void setEdge(uint8_t aEdge);

  bool isBinary();
  bool isAnalog();
  uint8_t  pin() const;
  uint16_t getValue() const;
  String   info(bool aShort=false) const;
  OutputPin* getConnection()const { return mConnection; }

  void     init();

  private:
  
  void set_value(uint16_t aValue);
  void setOn(bool aOn);
  bool doInvert() const;
  bool doToggle() const;
  bool isOn() const;
  uint8_t getEgde() const;
  
  uint8_t  mPin;
  uint8_t  mType;
  
  uint16_t mDelay_ms;
  uint16_t mPulse_ms;
  uint16_t mValue;
  int16_t  mRepeat;
  int16_t  mRepeatStored;

  task     mNextTask;
  unsigned long mNextTaskTime;
  static unsigned long mTick;

  OutputPin* mConnection;
};

#endif // OutputPin_h
