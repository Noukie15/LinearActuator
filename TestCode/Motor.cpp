#include "Motor.h"
#include "Config.h"
#include <Arduino.h>

void setupMotor() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void motor(int pwm){
  pwm = constrain(pwm, -MAX_PWM, MAX_PWM);

  if(pwm == 0){
    analogWrite(ENA, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    return;
  }

  digitalWrite(IN1, pwm > 0 ? HIGH : LOW);
  digitalWrite(IN2, pwm > 0 ? LOW : HIGH);

  int power = abs(pwm);
  if(power < MIN_PWM) power = MIN_PWM;
  analogWrite(ENA, power);
}
