#include <SPI.h>// #include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>

char WAITING_MSG[] = "Waiting for AQI sensor";
char label[] = "AQI";
char label2[] = " 2.5um PPM";
int previousReading = 0;
int textsize = 2;

void tftDiagnostics() {
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  tft.setRotation(1);
}

void waitMsg(Adafruit_ILI9341 tft) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(textsize);
  tft.setCursor(160 - (strlen(WAITING_MSG) * 3 * textsize), 110);
  tft.println(WAITING_MSG);
}

void updateAqi(uint16_t pm25, Adafruit_ILI9341 tft) {
  // only repaint when readings change
  if (previousReading != pm25) {
    char s[16];
    sprintf(s, "%d", pm25);
  
    if (pm25 < 51) {
      tft.setTextColor(ILI9341_BLACK);
      tft.fillScreen(ILI9341_GREEN);
    } else if (pm25 < 101) {
      tft.setTextColor(ILI9341_BLACK);
      tft.fillScreen(ILI9341_YELLOW);
    } else if (pm25 < 151) {
      tft.setTextColor(ILI9341_BLACK);
      tft.fillScreen(ILI9341_ORANGE);
    } else if (pm25 < 201) {
      tft.setTextColor(ILI9341_WHITE);
      tft.fillScreen(ILI9341_RED);
    } else if (pm25 < 301) {
      tft.setTextColor(ILI9341_WHITE);
      tft.fillScreen(ILI9341_PURPLE);
    } else {
      tft.setTextColor(ILI9341_WHITE);
      tft.fillScreen(ILI9341_MAROON);
    }
  
    // label
    textsize = 3;
    tft.setTextSize(textsize);
    tft.setCursor(160 - (strlen(label) * 3 * textsize), 35);
    tft.println(label);

    // AQI value
    textsize = 10;
    tft.setCursor(160 - (strlen(s) * 3 * textsize), 85);
    tft.setTextSize(textsize);
    tft.println(s);    

    // label 2
    textsize = 2;
    tft.setTextSize(textsize);
    tft.setCursor(160 - (strlen(label2) * 3 * textsize), 190);
    tft.println(label2);
  }

  previousReading = pm25;
}
