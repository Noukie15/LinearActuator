//
//  @author: Navez Thomas
//  @date: Thu Oct 29
//  @version: 1.0
//

#include <math.h>
#include <string.h>
#include <PID_v1.h>
#include <Encoder.h>
#include <ctype.h>
#include <stdlib.h>

const byte ENCODER_A = 2;
const byte ENCODER_B = 3;

Encoder encoder(ENCODER_A, ENCODER_B);

const int ENA = 9;
const int IN1 = 7;
const int IN2 = 8;

// ==== Mechanics ====
//M6, spoed 1mm, 20 gaten, x4-telling
//MM_PER_COUNT = p / (N * 4)  (p : spoed, N : gaten in encoder, 4 : factor voor kwadratuur alle vier de flankovergangen worden geteld)
const double MM_PER_COUNT = 1.0 / 80.0;

const double P1 = 10.0;
const double P2 = 50.0;
const double P3 = 90.0;

// ==== PID ====
double position = 0;
double targetposition = 0;
double output = 0;

double Kp = 80.0;
double Ki = 0.0;
double Kd = 3.0;

PID pid(&position, &output, &targetposition, Kp, Ki, Kd, DIRECT);

// ==== Serial Buffer ====
const byte BUFFER_SIZE = 24;
char buffer[BUFFER_SIZE];
byte bufferIndex = 0;

// ==== Motor ====
const int MAX_PWM = 220;
const int MIN_PWM = 45;

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

// ==== Commands ====
void processCommands(char *cmd){

  for( char *p = cmd; *p; p++){
    *p = toupper((unsigned char)*p);
  }

  if (strcmp(cmd, "P1") == 0){
    targetposition = P1;
  }
  else if (strcmp(cmd, "P2") == 0){
    targetposition = P2;
  }
  else if (strcmp(cmd, "P3") == 0){
    targetposition = P3;
  }
  else if (cmd[0] == 'G' && cmd[1] == ' '){
    char *end;
    double newPosition = strtod(cmd + 1, &end);

    while (*end == ' ') end++;

    if (end != cmd + 2 && *end == '\0' && newPosition >= 0 && newPosition <= 100){
      targetposition = newPosition;
    } else {
      Serial.println(F("Use: G 25"));
    }
  }
  else if (strcmp(cmd, "ZERO") == 0){
    motor(0);
    encoder.write(0);
    position = 0;
    targetposition = 0;
  }
  else if (strcmp(cmd, "STOP") == 0){
    motor(0);
    targetposition = position;
  }
  else if (cmd[0] != '\0') {
    Serial.println(F("Use: P1, P2, P3, G 25, ZERO, STOP"));
  }
}

void readSerial(){
  while (Serial.available() > 0) {
    char symbol = Serial.read();

    if (symbol == '\r' || symbol == '\n') {
      if (bufferIndex > 0) {
        buffer[bufferIndex] = '\0';
        processCommands(buffer);
        bufferIndex = 0;
      }
    }
    else if (bufferIndex < BUFFER_SIZE - 1) {
      buffer[bufferIndex++] = symbol;
    }
    else {
      //to long command: empty buffer (das pech buffer weg)
      bufferIndex = 0;
    }
  }
}

// ==== Setup ====
void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  Serial.begin(115200);

  pid.SetOutputLimits(-MAX_PWM, MAX_PWM);
  pid.SetSampleTime(10);
  pid.SetMode(AUTOMATIC);

  motor(0);

  Serial.println(F("Commands: P1, P2, P3, G 25, ZERO, STOP"));
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
