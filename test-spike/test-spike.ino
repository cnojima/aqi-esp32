#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#ifdef ESP8266
   #define STMPE_CS 16
   #define TFT_CS   0
   #define TFT_DC   15
   #define SD_CS    2
#endif
#ifdef ESP32
   #define STMPE_CS 32
   #define TFT_CS   15
   #define TFT_DC   33
   #define SD_CS    14
#endif

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(10);
  Serial.println("AQI TFT Test!");

  tft.begin();  
  tft.fillScreen(ILI9341_BLACK);
}

unsigned long writeText(char s[]) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(6);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.println(s);

  return micros() - start;
}


void loop() {
  // put your main code here, to run repeatedly:
  for(uint8_t rotation=0; rotation<4; rotation++) {
    tft.setRotation(rotation);
    writeText("Hello there");
    delay(1000);
  }
}
