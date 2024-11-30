#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>


const char* ssid = "Netis 2.4G";
const char* password = "password";

// Create an AsyncWebServer instance on port 80
AsyncWebServer server(80);


void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  digitalWrite(2, HIGH);
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  // Route to serve JSON data
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){

    JsonDocument doc;
    doc["t"] = get_temperature();
    doc["h"] = get_humidity();
    doc["s"] = get_status();
    doc["n"] = get_noise();
    doc["l"] = get_noise();

    String jsonString;
    serializeJson(doc, jsonString);

    request->send(200, "application/json", jsonString);
  });

  // Start the server
  server.begin();
}

void loop() {

}

double get_temperature() {
  return 24.5;
}

int get_humidity() {
  return 60;
}

String get_status() {
  return "OK";
}

int get_noise() {
  return 100;
}

int get_light() {
  return 50;
}
