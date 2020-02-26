#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>

#define DHTPIN D1                                                                                                    
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "snamellit7"; // Enter your WiFi name
const char* password =  "D6e&u^BF*Z!v"; // Enter WiFi password
const char* mqttServer = "nas.snamellit.com";
const int mqttPort = 1883;
const char* mqttUser = "otfxknod";
const char* mqttPassword = "nSuUc1dDLygF";

 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("DHT11 test!"));

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("Drybox", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("esp/test", "hello"); //Topic name
  client.subscribe("esp/test");
  
  dht.begin();

}

void sendMeasurement(float temperature, float humidity) {
 
  const size_t capacity = JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);
  char buffer[512];
  
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;

  serializeJson(doc, buffer);
  client.publish("drybox/sensor", buffer);
}

void printMeasurement(float temperature, float humidity) {  
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C "));

}
void loop() {
   delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  printMeasurement(t, h);
  sendMeasurement(t, h);
}
