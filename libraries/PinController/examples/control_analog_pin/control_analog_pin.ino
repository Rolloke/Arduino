
#include "PinController.h"

Function triangle[] =
{
    { Function::linear_ramp, 255, 1500 },
    { Function::linear_ramp,   0, 1500 },
    { Function::none,          0,    0 }
};

Function trapez[] =
{
    { Function::linear_ramp, 255, 1000 },
    { Function::constant,    255,  500 },
    { Function::linear_ramp,   0, 1000 },
    { Function::constant,      0,  500 },
    { Function::none,          0,    0 }
};

Function sine_half[] =
{
    { Function::sine_half, 255, 3000 },
    { Function::none, 0, 0}
};

AnalogPinController gAnalogPin(5, sine_half);


void setup() 
{
    gAnalogPin.start();
}

void loop() 
{
  // the button uses ticks in ms for working
  unsigned long fNow = millis();
  gAnalogPin.tick(fNow);
}
