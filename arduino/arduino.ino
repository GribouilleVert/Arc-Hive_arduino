#include <SoftwareSerial.h>
#include <Arduino_JSON.h>
#include "DHT.h"

const int DHT_PIN_INDOOR = 7;
const int DHT_PIN_OUTDOOR = 9;
const int PRESENCE_PIN = 4;

//DHT22
DHT indoorDht(DHT_PIN_INDOOR, DHT22);
DHT outdoorDht(DHT_PIN_OUTDOOR, DHT22);

//XBee
SoftwareSerial XBee(2, 3);

const unsigned long MIN_TIME_BETWEEN_UPDATES = 5000;
unsigned long timeSinceLastUpdate = 0;
unsigned long lastUpdateMilis = 0;

JSONVar response;

void setup() {  
  Serial.begin(9600);
  
  XBee.begin(9600);
  indoorDht.begin();
  outdoorDht.begin();

  pinMode(PRESENCE_PIN, INPUT);
}

bool presenceRegistered = false;
void loop() {
  if (timeSinceLastUpdate > MIN_TIME_BETWEEN_UPDATES) {
    lastUpdateMilis = millis();
    timeSinceLastUpdate = 0;
    
    JSONVar dht;
    JSONVar indoorDhtData;
    indoorDhtData["humidity"] = indoorDht.readHumidity();
    indoorDhtData["temperature"] = indoorDht.readTemperature();
    dht["indoor"] = indoorDhtData;
    JSONVar outdoorDhtData;
    outdoorDhtData["humidity"] = outdoorDht.readHumidity();
    outdoorDhtData["temperature"] = outdoorDht.readTemperature();
    dht["outdoor"] = outdoorDhtData;

    
    JSONVar presence = presenceRegistered;
    response["presence_sensor"] = presence;
    response["dht"] = dht;
    
    String responseString = JSON.stringify(response);
    XBee.println(responseString);

    //Reset
    presenceRegistered = false;
  } else {
      timeSinceLastUpdate = millis() - lastUpdateMilis;
  }

  if (digitalRead(PRESENCE_PIN) && !presenceRegistered) {
    presenceRegistered = true;
  }

}
