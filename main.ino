#define MOZZI_CONTROL_RATE 128
#include "MozziConfigValues.h"
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM

#include <Mozzi.h>
#include <Oscil.h> // oscillator
#include <tables/sin2048_int8.h> // table for Oscils to play
#include <Smooth.h>

Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <2048, MOZZI_CONTROL_RATE> kVib(SIN2048_DATA);

const int soft_pot_pin = A0;
const int fsr_pin = A4;

//this int depends on how comfortable your finger is while applying force to the fsr.
//get your limit by printing out the fsr and seeing the limit that you prefer.
const int fsr_velocity_limit = 930;

const int scale = 3; //scale of octaves playable
const int starting_octave = 3; //cant be less than 0

int soft_pot;
int fsr;

int pitch; //27.5hz-4186hz
int velocity; //0-127

float hz_map_low = 32.703; //dont change
float hz_map_high;

float depth = 0.05;

void setup() {
  Serial.begin(115200);

  Serial.println("udu3324 was here <3");

  //calculate the starting hz
  for (int i = 1; i < starting_octave; i++) {
    hz_map_low += hz_map_low;
  }

  hz_map_high = hz_map_low;

  //calculate the ending hz
  for (int i = 0; i < scale; i++) {
    hz_map_high += hz_map_low;
  }

  //vibrato setup
  kVib.setFreq(6.5f);

  startMozzi();
}

void updateControl() {
  // read values from both sensors
  soft_pot = mozziAnalogRead(soft_pot_pin);
  fsr = mozziAnalogRead(fsr_pin);

  //map these values
  pitch = map(soft_pot, 0, 1019, hz_map_low, hz_map_high);
  velocity = map(fsr, 0, fsr_velocity_limit, 0, 127);

  //hard limits
  if ((soft_pot == 0)) pitch = 0; //stop mapping of zero
  if (velocity > 127) velocity = 127; //stop higher mappings

  Serial.print("pitch \t");
  Serial.print(pitch);
  Serial.print("\t velocity   ");
  Serial.println(velocity);

  aSin.setFreq(pitch);
}

AudioOutput updateAudio() {
  return MonoOutput::from8Bit(aSin.next());
}

void loop() {
  audioHook();
}
