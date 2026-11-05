//
//  Test 1: alleen de MOTOR (zonder encoder, zonder PID)
//
//  Seriële commando's (115200 baud, newline als einde):
//    M 100    -> motor draait met PWM +100 (richting 1)
//    M -100   -> motor draait met PWM -100 (richting 2)
//    STOP     -> motor stopt
//

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

const int ENA = 9;
const int IN1 = 7;
const int IN2 = 8;

const int MAX_PWM = 220;
const unsigned long RUN_TIME_MS = 1000;   // automatische stop na 1 s

unsigned long motorStartTime = 0;
bool motorRunning = false;

// ==== Serial Buffer ====
const byte BUFFER_SIZE = 24;
char buffer[BUFFER_SIZE];
byte bufferIndex = 0;

// ==== Motor ====
// Let op: hier géén MIN_PWM-correctie, zodat ik zelf kan zien
// vanaf welke PWM-waarde de motor begint te draaien.
void motor(int pwm){
  pwm = constrain(pwm, -MAX_PWM, MAX_PWM);

  if(pwm == 0){
    analogWrite(ENA, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    motorRunning = false;
    return;
  }

  digitalWrite(IN1, pwm > 0 ? HIGH : LOW);
  digitalWrite(IN2, pwm > 0 ? LOW : HIGH);
  analogWrite(ENA, abs(pwm));

  motorStartTime = millis();
  motorRunning = true;
}

// ==== Commands ====
void processCommands(char *cmd){

  for(char *p = cmd; *p; p++){
    *p = toupper((unsigned char)*p);
  }

  if (cmd[0] == 'M' && cmd[1] == ' '){
    char *end;
    long pwm = strtol(cmd + 1, &end, 10);

    while (*end == ' ') end++;

    if (end != cmd + 2 && *end == '\0'){
      motor((int)pwm);
      Serial.print(F("Motor PWM: "));
      Serial.println(constrain(pwm, -MAX_PWM, MAX_PWM));
    } else {
      Serial.println(F("Use: M 100  or  M -100"));
    }
  }
  else if (strcmp(cmd, "STOP") == 0){
    motor(0);
    Serial.println(F("Motor stopped"));
  }
  else if (cmd[0] != '\0') {
    Serial.println(F("Use: M 100, M -100, STOP"));
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
  motor(0);

  Serial.println(F("MOTOR TEST"));
  Serial.println(F("Commands: M 100, M -100, STOP"));
}

// ==== Main Loop ====
void loop(){
  readSerial();

  if (motorRunning && millis() - motorStartTime >= RUN_TIME_MS){
    motor(0);
    Serial.println(F("Auto-stop"));
  }
}
