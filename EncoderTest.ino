//
//  Test 2: alleen de ENCODER
//
//  Draai de actuator met de hand
//  (115200 baud). Er wordt enkel iets geprint als de waarde verandert.
//
//  Seriële commando's:
//    ZERO     -> teller en positie op 0 zetten
//
//  Verwachting: 80 counts = 1 mm (20 gaten x 4 flanken, spoed 1 mm).
//  Vooruit/achteruit moet een tegengesteld teken geven. Is dat omgekeerd,
//  wissel dan ENCODER_A en ENCODER_B.
//

#include <string.h>
#include <ctype.h>
#include <Encoder.h>

const byte ENCODER_A = 2;
const byte ENCODER_B = 3;

Encoder encoder(ENCODER_A, ENCODER_B);

// M6, spoed 1mm, 20 gaten, x4-telling
const double MM_PER_COUNT = 1.0 / 80.0;

long lastCount = 0;

// ==== Serial Buffer ====
const byte BUFFER_SIZE = 24;
char buffer[BUFFER_SIZE];
byte bufferIndex = 0;

void processCommands(char *cmd){

  for(char *p = cmd; *p; p++){
    *p = toupper((unsigned char)*p);
  }

  if (strcmp(cmd, "ZERO") == 0){
    encoder.write(0);
    lastCount = 0;
    Serial.println(F("Encoder zeroed"));
  }
  else if (cmd[0] != '\0') {
    Serial.println(F("Use: ZERO"));
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
  Serial.begin(115200);

  Serial.println(F("ENCODER TEST"));
  Serial.println(F("Move the actuator by hand. Commands: ZERO"));
}

// ==== Main Loop ====
void loop(){
  readSerial();

  long count = encoder.read();

  if (count != lastCount){
    Serial.print(F("counts: "));
    Serial.print(count);
    Serial.print(F("   position: "));
    Serial.print(count * MM_PER_COUNT, 3);
    Serial.print(F(" mm   direction: "));
    Serial.println(count > lastCount ? F("+") : F("-"));

    lastCount = count;
  }

  delay(50);   // niet serial spammen
}
