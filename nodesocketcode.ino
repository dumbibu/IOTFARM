#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "CONNECT(IBRAHIM)2.4_plus";
const char* password = "ibrahim999@";

// WebSocket server URL
const char* websockets_server_host = "farm.dijinx.com"; // Server URL
const uint16_t websockets_server_port = 443; // Server port for HTTPS

WebSocketsClient webSocket;

// Pin for the relay
const int relayPin = D1; // Assuming the relay is connected to GPIO pin D2
bool relayActivated = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      // Immediate reconnection
      webSocket.beginSSL(websockets_server_host, websockets_server_port, "/ws");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      // Send JSON message to the WebSocket server
      webSocket.sendTXT("{\"arduino_id\":\"6O6ZEA\"}");
      break;
    case WStype_TEXT:
      {
        Serial.printf("[WSc] Received text: %s\n", payload);
        // Parse JSON payload
        DynamicJsonDocument doc(256);
        deserializeJson(doc, payload);
        int duration = doc["payload"]["duration"];
        // Activate relay for the specified duration
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
    pinMode(relayPin, HIGH); // Turn on the relay
    relayActivated = true;
    delay(durationInMinutes * 60000); // Convert minutes to milliseconds
    pinMode(relayPin, LOW);
    
    Serial.println("Relay deactivated");
    relayActivated = false;
    
    
  } else {
    Serial.println("Relay already activated, ignoring command");
  }
}

void setup() {
  // Start Serial
  Serial.begin(115200);

  // Connect to WiFi
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

  // Set relay pin as output
  pinMode(relayPin, LOW);

  // WebSocket connection
  webSocket.beginSSL(websockets_server_host, websockets_server_port, "/ws");
  
  // Event handler
  webSocket.onEvent(webSocketEvent);

  // Set timeouts
  webSocket.setReconnectInterval(5000); // Try to reconnect every 5 seconds
}

void loop() {
  // WebSocket loop
  webSocket.loop();
}
