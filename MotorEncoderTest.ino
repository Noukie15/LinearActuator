//
//  Test 3: MOTOR + ENCODER samen (open lus, nog zonder PID)
//  Doel: controleren of motor en encoder bij elkaar passen:
//        - telt de encoder in de juiste richting als de motor draait?
//        - vanaf welke PWM begint de actuator te bewegen (MIN_PWM)?
//
//  Seriële commando's (115200 baud, newline als einde):
//    M 100    -> motor draait met PWM +100, positie wordt live geprint
//    M -100   -> motor draait met PWM -100
//    STOP     -> motor stopt
//    ZERO     -> encoder en positie op 0 zetten
//    TEST     -> korte heen-en-weer test, controleert het teken van de encoder
//    RAMP     -> verhoogt de PWM stapsgewijs tot de actuator beweegt
//                (zo vind je een goede waarde voor MIN_PWM)
//
//  LET OP: er zijn geen eindschakelaars. De motor stopt automatisch na
//  RUN_TIME_MS. Zet de actuator vooraf ergens in het midden
//

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <Encoder.h>

const byte ENCODER_A = 2;
const byte ENCODER_B = 3;

Encoder encoder(ENCODER_A, ENCODER_B);

const int ENA = 9;
const int IN1 = 7;
const int IN2 = 8;

// M6, spoed 1mm, 20 gaten, x4-telling
const double MM_PER_COUNT = 1.0 / 80.0;

const int MAX_PWM = 220;
const unsigned long RUN_TIME_MS = 1000;   // automatische stop

// Instellingen voor TEST en RAMP
const int TEST_PWM = 120;
const unsigned long TEST_TIME_MS = 500;
const int RAMP_STEP = 5;
const unsigned long RAMP_STEP_MS = 200;
const long MOVE_THRESHOLD = 4;            // counts (= 0.05 mm) om "beweging" te detecteren

unsigned long motorStartTime = 0;
unsigned long lastPrint = 0;
bool motorRunning = false;

// ==== Serial Buffer ====
const byte BUFFER_SIZE = 24;
char buffer[BUFFER_SIZE];
byte bufferIndex = 0;

// ==== Motor ====
// Geen MIN_PWM-correctie, zodat RAMP de echte drempel kan vinden.
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

// ==== Tests ====
void runDirectionTest(){
  Serial.println(F("--- TEST: direction check ---"));

  long start = encoder.read();
  motor(TEST_PWM);
  delay(TEST_TIME_MS);
  motor(0);
  delay(300);
  long forward = encoder.read() - start;

  start = encoder.read();
  motor(-TEST_PWM);
  delay(TEST_TIME_MS);
  motor(0);
  delay(300);
  long backward = encoder.read() - start;

  Serial.print(F("PWM +"));
  Serial.print(TEST_PWM);
  Serial.print(F(" -> counts: "));
  Serial.print(forward);
  Serial.print(F(" ("));
  Serial.print(forward * MM_PER_COUNT, 3);
  Serial.println(F(" mm)"));

  Serial.print(F("PWM -"));
  Serial.print(TEST_PWM);
  Serial.print(F(" -> counts: "));
  Serial.print(backward);
  Serial.print(F(" ("));
  Serial.print(backward * MM_PER_COUNT, 3);
  Serial.println(F(" mm)"));

  if (forward > 0 && backward < 0){
    Serial.println(F("OK: positive PWM = positive counts"));
  }
  else if (forward < 0 && backward > 0){
    Serial.println(F("INVERTED: swap ENCODER_A/ENCODER_B or IN1/IN2"));
  }
  else {
    Serial.println(F("FAIL: no (or unclear) movement. Check wiring/power/PWM."));
  }
}

void runRamp(){
  Serial.println(F("--- RAMP: searching minimum PWM ---"));

  long start = encoder.read();

  for (int pwm = RAMP_STEP; pwm <= MAX_PWM; pwm += RAMP_STEP){
    motor(pwm);
    delay(RAMP_STEP_MS);

    long moved = labs(encoder.read() - start);

    Serial.print(F("PWM "));
    Serial.print(pwm);
    Serial.print(F("  moved: "));
    Serial.print(moved);
    Serial.println(F(" counts"));

    if (moved >= MOVE_THRESHOLD){
      motor(0);
      Serial.print(F("Movement starts around PWM "));
      Serial.println(pwm);
      Serial.println(F("Use a value slightly above this as MIN_PWM."));
      return;
    }
  }

  motor(0);
  Serial.println(F("No movement detected up to MAX_PWM. Check wiring/power."));
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
  else if (strcmp(cmd, "ZERO") == 0){
    motor(0);
    encoder.write(0);
    Serial.println(F("Encoder zeroed"));
  }
  else if (strcmp(cmd, "TEST") == 0){
    runDirectionTest();
  }
  else if (strcmp(cmd, "RAMP") == 0){
    runRamp();
  }
  else if (cmd[0] != '\0') {
    Serial.println(F("Use: M 100, M -100, STOP, ZERO, TEST, RAMP"));
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

  Serial.println(F("MOTOR + ENCODER TEST"));
  Serial.println(F("Commands: M 100, M -100, STOP, ZERO, TEST, RAMP"));
}

// ==== Main Loop ====
void loop(){
  readSerial();

  // Automatische stop
  if (motorRunning && millis() - motorStartTime >= RUN_TIME_MS){
    motor(0);
    Serial.println(F("Auto-stop (time out)"));
  }

  // Live positie tijdens het draaien
  if (motorRunning && millis() - lastPrint >= 100){
    lastPrint = millis();
    long count = encoder.read();
    Serial.print(F("counts: "));
    Serial.print(count);
    Serial.print(F("   position: "));
    Serial.print(count * MM_PER_COUNT, 3);
    Serial.println(F(" mm"));
  }
}
