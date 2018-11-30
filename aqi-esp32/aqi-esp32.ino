#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ESP32 pins (adafruit huzzah ESP32 board)
#define STMPE_CS 32 // chip select pin
#define TFT_CS   15 // chip select pin
#define TFT_DC   33 // data control
#define SD_CS    14 // SD chip select
#define SER_1_I  13 // Serial1.RX slaved pinout
#define SER_1_O  12 // Serial1.TX slaved pinout

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// streamed serial data from PM25 module
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

int textsize = 2;
char label[] = "AQI";
char label2[] = " 2.5um PPM";

int previousReading = 0;

void updateAqi(uint16_t pm25) {
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

  // only repaint when readings change
  previousReading = pm25;
}


void showPm() {
  updateAqi(data.pm25_env);

  // reading data was successful!
  Serial.println();
  Serial.println("---------------------------------------");
  Serial.println("Concentration Units (standard)");
  Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
  Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
  Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
  Serial.println("---------------------------------------");
  Serial.println("Concentration Units (environmental)");
  Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
  Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
  Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
  Serial.println("---------------------------------------");
  Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
  Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
  Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
  Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
  Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
  Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
  Serial.println("---------------------------------------");
}


boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    //    Serial.println("stream is empty");
    return false;
  }

  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}




void setup() {
  // our debugging output
  Serial.begin(115200);
  delay(10);

  // sensor baud rate is 9600
  Serial1.begin(9600, SERIAL_8N1, SER_1_I, SER_1_O);
  delay(10);

  // TFT
  tft.begin();
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
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(textsize);
  tft.setCursor(10, 40); // pixel values
  tft.println("Waiting for AQI sensor");
  delay(1000);
}

void loop() {
  if (readPMSdata(&Serial1)) {
    showPm();
  }
}
