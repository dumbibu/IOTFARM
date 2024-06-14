#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>


const int digitalPin = D0; 
const int analogPin = A0;  


const char* ssid = "WIFINAME";
const char* password = "PASS"; 


const char* sensorId = "6O6ZEA";


const char* apiUrl = "https://farm.dijinx.com/api/v1/farm/reading/upload";

void setup() {
  Serial.begin(9600); 
  pinMode(digitalPin, INPUT); 

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    int digitalValue = digitalRead(digitalPin);


    int analogValue = analogRead(analogPin);

    int minValue = 145;
    int maxValue = 1023;


    analogValue = constrain(analogValue, minValue, maxValue);


    float percentage = 100.0 - ((analogValue - minValue) * 100.0 / (maxValue - minValue));
    //old formula
    //float percentage = (100 - ((analogValue / 1000.00) * 100));

    if (percentage > 100.0) {
    percentage = 100.0;
    }
    int roundedPercentage = round(percentage);


    Serial.print("Digital Value: ");
    Serial.println(digitalValue);
    Serial.print("Analog Value: ");
    Serial.print(analogValue);
    Serial.print(" (");
    Serial.print(percentage);
    Serial.println("%)");


    sendSensorData(roundedPercentage);

    delay(3600000);
    

  } else {
    Serial.println("WiFi not connected. Retrying...");
    WiFi.reconnect();
    delay(5000);
  }
}

void sendSensorData(int moisture) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); 
    HTTPClient http;
    http.begin(client, apiUrl);  

    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["sensor_id"] = sensorId;
    jsonDoc["moisture"] = moisture;

    String requestBody;
    serializeJson(jsonDoc, requestBody);

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  } 
}

