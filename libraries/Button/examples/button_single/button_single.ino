#include <button.h>
 
#include <SoftwareSerial.h>

// forward declaration of trigger function for Button constructor
void triggerButton(uint8_t, uint8_t);

// constructor with arguments
Button gButton1( 8, triggerButton); // button for pin  8 only
Button gButton2( 9, triggerButton); // button for pin  9 only
Button gButton3(10, triggerButton); // button for pin 10 only

void setup() 
{
  // optionally additional trigger events may be activated
  // normally the button is triggered once and released once
  gButton1.setDelay(2000); // set delayed trigger
  gButton1.setRepeat(500); // set repeat interval

  Serial.begin(115200);
}

void loop() 
{
  // the button uses ticks in ms for working
  unsigned long fNow = millis();
  gButton1.tick(fNow);
  gButton2.tick(fNow);
  gButton3.tick(fNow);
  
}


void triggerButton(uint8_t aState, uint8_t aButtonNo)
{
  // for debugging purpose use the serial monitor of the arduino IDE
  Serial.print("trigger: ");
  Serial.print(Button::nameOf(static_cast<Button::eState>(aState)));
  // the button number is equal to the pin number
  Serial.print(", Button No: ");
  Serial.println(aButtonNo);
}
