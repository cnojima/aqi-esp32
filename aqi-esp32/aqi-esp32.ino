#include <SPI.h>
#include <Adafruit_ILI9341.h>

// ESP32 pins (adafruit huzzah ESP32 board)
#define STMPE_CS 32 // chip select pin
#define TFT_CS   15 // chip select pin
#define TFT_DC   33 // data control
#define SD_CS    14 // SD chip select
#define SER_1_I  13 // Serial1.RX slaved pinout
#define SER_1_O  12 // Serial1.TX slaved pinout

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

int waitCount = 0;

void setup() {
  // our debugging output
  Serial.begin(115200);
  delay(10);

  // pm25 sensor baud rate is 9600
  Serial1.begin(9600, SERIAL_8N1, SER_1_I, SER_1_O);
  delay(10);

  // TFT
  tft.begin();
  tftDiagnostics();
  waitMsg(tft);
  delay(1000);

  // wifi
  wifi();
}

void loop() {
  if (Serial1.available() && readPMSdata(&Serial1)) {
    showPm();
    waitCount = 0;
  } else if(waitCount > 5) {
    waitMsg(tft);
    waitCount = 0;
  } else {
    delay(1000);
    waitCount++;
  }
}
