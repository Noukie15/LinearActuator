//
//  @author: Navez Thomas
//  @date: Thu Oct 29
//  @version: 1.1
//

#include <math.h>
#include <string.h>
#include <PID_v1.h>
#include <Encoder.h>
#include <ctype.h>
#include <stdlib.h>

#include "Config.h"
#include "Motor.h"
#include "EncoderModule.h"
#include "PidControl.h"
#include "Communication.h"

// ==== Setup ====
void setup() {
  setupMotor();
  setupEncoder();
  setupCommunication();
  setupPid();

  motor(0);
}

// ==== Main Loop ====
void loop(){
  readSerial();

  position = encoder.read() * MM_PER_COUNT;

  if(fabs(targetposition - position) <= 0.01){
    motor(0);
  } else {
    pid.Compute();
    motor((int)output);
  }
}
