#include <button.h>
 
#include <SoftwareSerial.h>

// forward declaration of trigger function for Button constructor
void triggerButton(uint8_t, uint8_t);
// array with buttons: Bit 1, 2, 3
uint8_t gButtonPins[3] = { 8, 9, 10 };
// constructor with arguments
Button gButton(gButtonPins, sizeof(gButtonPins), triggerButton);

void setup() 
{
  // optionally additional trigger events may be activated
  // normally the button is triggered once and released once
  gButton.setDelay(2000); // set delayed trigger
  gButton.setRepeat(500); // set repeat interval

  Serial.begin(115200);
}

void loop() 
{
  // the button uses ticks in ms for working
  unsigned long fNow = millis();
  gButton.tick(fNow);
  
}


void triggerButton(uint8_t aState, uint8_t aButtonNo)
{
  // for debugging purpose use the serial monitor of the arduino IDE
  Serial.print("trigger: ");
  Serial.print(Button::nameOf(static_cast<Button::eState>(aState)));
  // the button number is identified by the bit that is set
  // bit 1 for pin  8 -> aButtonNo: 1
  // bit 2 for pin  9 -> aButtonNo: 2
  // bit 3 for pin 10 -> aButtonNo: 4
  Serial.print(", Button No: ");
  Serial.println(aButtonNo);
}
