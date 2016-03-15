#include "DHT.h"
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <ESP_SSD1306.h>

#define SDAPIN  12
#define SCLPIN  14

#define ROTATION_0  0
#define ROTATION_90 1

#define DHTPIN 4     // what pin we're connected to
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE,15);
ESP_SSD1306 display(false);   //reset disable //oled

void setup() {
  Serial.begin(115200);
  Serial.println("DHTxx test!");
  dht.begin();

  //oled
  Wire.begin(SDAPIN, SCLPIN);        //sda, scl
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false);
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextWrap(false);

  //display.setRotation(ROTATION_90);

  display.setCursor(0, 0);

}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
  display.print("Humidity: ");
  display.print(h);
  display.print(" %\n");
  display.print("Temperature: ");
  display.print(t);
  display.print(" *C ");
  display.display();

}
