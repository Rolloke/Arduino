
#include "Melody.h"

// big ben
Tone gTones[] =
{
	{ NOTE_B3, 1, 2}, // 1/2 tone length
	{ NOTE_G3, 1, 2},
	{ NOTE_A3, 1, 2},
	{ NOTE_D3, 1, 1}, // full tone length
	{ SILENCE, 1, 2}, 
	{ NOTE_D3, 1, 2},
	{ NOTE_A3, 1, 2},
	{ NOTE_B3, 1, 2},
	{ NOTE_G3, 1, 1}, 
	{ SILENCE, 2, 1},
	{ 0, 0, 0}        // end of array
};

const int toneOutput        =  4;
const int Repeats           = 10;
const int FullToneLenght_ms = 1000;

Melody gMelody(toneOutput, gTones, Repeats, FullToneLenght_ms);

void setup() 
{
    gMelody.startMelody();
}

void loop() 
{
  // the melody module uses ticks in ms for working
  unsigned long fNow = millis();
  gMelody.tick(fNow);  
}
