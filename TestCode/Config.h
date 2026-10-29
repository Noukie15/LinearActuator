#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Encoder.h>
#include <PID_v1.h>

// Pinnen
const byte ENCODER_A = 2;
const byte ENCODER_B = 3;
const int ENA = 9;
const int IN1 = 7;
const int IN2 = 8;

// Mechanica (Aanname: M6 spindel, 1mm spoed, 20 gaten, x4 telling = 80 counts/mm -> 1.0 / 80.0 mm per count)
const double MM_PER_COUNT = 1.0 / 80.0;

const double P1 = 10.0;
const double P2 = 50.0;
const double P3 = 90.0;

// PID Variabelen
extern double position;
extern double targetposition;
extern double output;

// Motor PWM limieten
const int MAX_PWM = 220;
const int MIN_PWM = 45;

// Externe objecten
extern Encoder encoder;
extern PID pid;

#endif
