#include <SoftwareSerial.h>
SoftwareSerial pmsSerial(2, 3);

void setup() {
  pinMode(0, OUTPUT);

  // our debugging output
  Serial.begin(115200);
}

void loop() {
  digitalWrite(0, HIGH);
  delay(500);
  digitalWrite(0, LOW);
  delay(500);

  Serial.print("Particles > 0.3um / 0.1L air:");
}
