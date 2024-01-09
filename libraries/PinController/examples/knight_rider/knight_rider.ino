
#include "PinController.h"

const uint16_t dwell_time = 50;

Command night_rider[] =
{
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1), dwell_time},
    { PINS16(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1), dwell_time},
    { 0, 0}        // end of array
};

uint8_t pin_numbers[]  =
{
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
   10,
   11,
   12,
   13,
   14,
   15,
   16,
    0
};

DigitalPinController gPinControl(pin_numbers, night_rider);

void setup() 
{
    gPinControl.start();
}

void loop() 
{
  // the button uses ticks in ms for working
  unsigned long fNow = millis();
  gPinControl.tick(fNow);
}
