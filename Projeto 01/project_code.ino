#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11

/******** Adafruit IO Config ***********/
#define IO_USERNAME  ""
#define IO_KEY       ""

#define WIFI_SSID ""
#define WIFI_PASS ""

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// Sensor DHT
uint8_t DHTPin = 4; 
               
// Inicializa o sensor DHT.
DHT dht(DHTPin, DHTTYPE);

// Feeds
AdafruitIO_Feed *temperatureFeed = io.feed("temperature");
AdafruitIO_Feed *maxTemperatureFeed = io.feed("maxTemperature");
AdafruitIO_Feed *minTemperatureFeed = io.feed("minTemperature");

AdafruitIO_Feed *humidityFeed = io.feed("humidity");

float temperature = 0;
float maxTemperature = 0;
float minTemperature = 100;

float humidity = 0;

unsigned int publishingInterval = 60*1000;
 
void setup() {
  Serial.begin(115200);
  
  pinMode(DHTPin, INPUT);
  pinMode(2, OUTPUT);

  dht.begin();

  // wait for serial monitor to open
  while(! Serial);           
  
  // connect to io.adafruit.com
  Serial.println("Conectando com Adafruit IO ");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  digitalWrite(2, HIGH);
}

void loop() {
  io.run();
  publish();
}

void publish() {
  updateTemperature();
  updateHumidity();

  temperatureFeed->save(temperature);
  maxTemperatureFeed->save(maxTemperature);
  minTemperatureFeed->save(minTemperature);

  humidityFeed->save(humidity);

  printData();
  
  delay(publishingInterval);
}

void updateTemperature() {
  temperature = dht.readTemperature();
  if(maxTemperature < temperature) {
    maxTemperature = temperature;
  }
  if(minTemperature > temperature) {
    minTemperature = temperature;
  }
}

void updateHumidity() {
  humidity = dht.readHumidity();
}

void printData() {
  Serial.print("Temperatura atual: ");
  Serial.println(temperature);
  Serial.print("Temperatura mínima: ");
  Serial.println(minTemperature);
  Serial.print("Temperatura máxima: ");
  Serial.println(maxTemperature);

  Serial.print("Humidade: ");
  Serial.println(humidity);
}
