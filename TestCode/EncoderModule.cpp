#include "EncoderModule.h"
#include "Config.h"

Encoder encoder(ENCODER_A, ENCODER_B);
double position = 0;

void setupEncoder() {
  // Encoder bibliotheek initialiseert interrupts automatisch via de constructor
}
