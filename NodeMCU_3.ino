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

//Thingspeak settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "CYNEKV37Y4JJVRQ5";
const int updateThingSpeakInterval = 16 * 1000;
long lastConnectionTime = 0;
boolean lastConnected = false;
int failedCounter = 0;


#include <ESP8266WiFi.h>
const char* ssid     = "lifez";
const char* password = "0845918889";
// Initialize Arduino Ethernet Client
WiFiClient client;

void updateThingSpeak(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);

    lastConnectionTime = millis();

    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();

      failedCounter = 0;
    }
    else
    {
      failedCounter++;

      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");
      Serial.println();
    }

  }
  else
  {
    failedCounter++;

    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");
    Serial.println();

    lastConnectionTime = millis();
  }
}

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

  if(millis() - lastConnectionTime > updateThingSpeakInterval)
  {
    updateThingSpeak("field1="+String(h));
  }

}
