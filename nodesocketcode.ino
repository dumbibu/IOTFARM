#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>


const char* ssid = "WIFI";
const char* password = "PASS";


const char* websockets_server_host = "farm.dijinx.com"; 
const uint16_t websockets_server_port = 443; 

WebSocketsClient webSocket;


const int relayPin = D1; 
bool relayActivated = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      webSocket.beginSSL(websockets_server_host, websockets_server_port, "/ws");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      webSocket.sendTXT("{\"arduino_id\":\"YOURSENSORID\"}");
      break;
    case WStype_TEXT:
      {
        Serial.printf("[WSc] Received text: %s\n", payload);
        DynamicJsonDocument doc(256);
        deserializeJson(doc, payload);
        int duration = doc["payload"]["duration"];
        activateRelay(duration);
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] Received binary data\n");
      break;
  }
}

void activateRelay(int durationInMinutes) {
  if (!relayActivated) {
    Serial.printf("Activating relay for %d minutes\n", durationInMinutes);
    pinMode(relayPin, HIGH); 
    relayActivated = true;
    delay(durationInMinutes * 60000); 
    pinMode(relayPin, LOW);
    
    Serial.println("Relay deactivated");
    relayActivated = false;
    
    
  } else {
    Serial.println("Relay already activated, ignoring command");
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(relayPin, LOW);

  webSocket.beginSSL(websockets_server_host, websockets_server_port, "/ws");
  
  webSocket.onEvent(webSocketEvent);

  webSocket.setReconnectInterval(5000); 

void loop() {
  webSocket.loop();
}
