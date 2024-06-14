#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// Define the pins
const int digitalPin = D0; // Connect the digital pin of the sensor to D0
const int analogPin = A0;  // Connect the analog pin of the sensor to A0

// WiFi credentials
const char* ssid = "CONNECT(IBRAHIM)2.4_plus"; // Replace with your network SSID
const char* password = "ibrahim999@"; // Replace with your network password

// Sensor ID
const char* sensorId = "6O6ZEA";

// API URL
const char* apiUrl = "https://farm.dijinx.com/api/v1/farm/reading/upload";

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(digitalPin, INPUT); // Set digital pin as input

  // Connect to WiFi
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
    // Optionally, enter deep sleep or restart after failure
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Read digital value
    int digitalValue = digitalRead(digitalPin);

    // Read analog value
    int analogValue = analogRead(analogPin);
// Define the minimum and maximum values
int minValue = 145;
int maxValue = 1023;

// Ensure the analog value is within the expected range
analogValue = constrain(analogValue, minValue, maxValue);

// Map the analog value to the percentage
    float percentage = 100.0 - ((analogValue - minValue) * 100.0 / (maxValue - minValue));
    // Convert analog value to percentage
    //float percentage = (100 - ((analogValue / 1000.00) * 100));
     // Ensure the percentage does not exceed 100%
  if (percentage > 100.0) {
    percentage = 100.0;
  }
    int roundedPercentage = round(percentage);

    // Print values
    Serial.print("Digital Value: ");
    Serial.println(digitalValue);
    Serial.print("Analog Value: ");
    Serial.print(analogValue);
    Serial.print(" (");
    Serial.print(percentage);
    Serial.println("%)");

    // Call the API
    sendSensorData(roundedPercentage);

    delay(900000); // Delay for 15 minutes
    

  } else {
    Serial.println("WiFi not connected. Retrying...");
    WiFi.reconnect();
    delay(5000);
  }
}

void sendSensorData(int moisture) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // Disable certificate verification (not recommended for production)

    HTTPClient http;
    http.begin(client, apiUrl);  // Use the new API with WiFiClientSecure

    http.addHeader("Content-Type", "application/json");

    // Create JSON object
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["sensor_id"] = sensorId;
    jsonDoc["moisture"] = moisture;

    // Serialize JSON object to string
    String requestBody;
    serializeJson(jsonDoc, requestBody);

    // Send HTTP POST request
    int httpResponseCode = http.POST(requestBody);

    // Print response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // End HTTP connection
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  } 
}

