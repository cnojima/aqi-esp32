#include <ESP8266WiFi.h>

void wifi() {
  WiFi.begin("osgiliath", "th0rsh4mm3r1smj0ln1r");
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}
