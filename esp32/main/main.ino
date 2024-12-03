#include <AsyncTCP.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <deque>
#include <esp_wifi.h>  // ESP-IDF WiFi API

const int DHT_PIN = 5;
const int MAX_DATA_LENGTH = 48; // Limit list to 48 values (24 hours of data if sampled every 30 mins)
const int MEASSUREMENT_INTERVAL = 1800000; // 30 minutes in milliseconds

// Wifi name and password
const char* ssid = "Netis 2.4G";
const char* password = "password";

// Allowed MAC addresses (whitelisted clients)
const char* allowedMACs[] = {
    "88:6c:60:c2:56:28",   // Redmi Note 13 Pro address
    "4C:ED:FB:CA:C1:6D"    // PC address
};

// Create an AsyncWebServer instance on port 80
AsyncWebServer server(80);

// DHT11 instance on pin D26
DHT dht(DHT_PIN, DHT11);

std::deque<float> temperatureList;
std::deque<float> humidityList;

SemaphoreHandle_t dataMutex;

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);

  // init dht instance
  dht.begin();
  delay(2000);

  // Connect to Wi-Fi
  WiFi.softAP("SSID", "PASSWORD");
  esp_wifi_start();  // Start the WiFi driver
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  digitalWrite(2, HIGH);
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  dataMutex = xSemaphoreCreateMutex();
  xTaskCreate(readSensorTask, "Read Sensor Task", 2048, NULL, 1, NULL);

  // Route to serve JSON data
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
      
      if (isMACAllowed(request)) { // If clients MAC is in the whitelist

      JsonDocument doc; // Create dinamic JSON document
      doc["t"] = get_temperature(); // Add data to it
      doc["h"] = get_humidity();
      doc["s"] = get_status();
      doc["n"] = get_noise();
      doc["l"] = get_noise();
      doc["o"] = getUptime(); // Time since ESP was powered

      String jsonString; // Convert JSOM document to string
      serializeJson(doc, jsonString);

      request->send(200, "application/json", jsonString); // Send response
    } else {
      request->send(403, "text/plain", "Access Denied"); // Client is not in whitelist, deny access
    }
  });

  server.on("/prev", HTTP_GET, [](AsyncWebServerRequest *request){

    if (isMACAllowed(request)) { // If clients MAC is in the whitelist
    
      JsonDocument doc; // Create dynamic JSON document
      JsonArray data = doc["data"].to<JsonArray>(); // Create nested array (array in array)
    
      xSemaphoreTake(dataMutex, portMAX_DELAY); // Add previous data to lists
      for (size_t i = 0; i < temperatureList.size(); i++) {
        JsonObject reading = data.add<JsonObject>();
        reading["t"] = temperatureList[i];
        reading["h"] = humidityList[i];
      }
      xSemaphoreGive(dataMutex);
    
      String output; // Convert dynamic JSON documet to string
      serializeJson(doc, output);

      request->send(200, "application/json", output); // Send response
    } else {
      request->send(403, "text/plain", "Access Denied"); // Client is not in whitelist, deny access
    }
  });

  // Start the server
  server.begin();
  Serial.println("Server started.");
}

void loop() {}

float get_temperature() {
  float temp = dht.readTemperature();
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println(" C ");
  return temp;
}

float get_humidity() {
  float hum = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" % ");
  return hum;
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

// This is a task, that runs every 30 mins.
// It saves temp and humidity to list.
// It's made for statistics
void readSensorTask(void *parameter) {
    for (;;) {
        float temp = get_temperature();
        float hum = get_humidity();
        
        if (!isnan(temp) && !isnan(hum)) {
            xSemaphoreTake(dataMutex, portMAX_DELAY);
            if (temperatureList.size() >= MAX_DATA_LENGTH) temperatureList.pop_front();
            if (humidityList.size() >= MAX_DATA_LENGTH) humidityList.pop_front();
            temperatureList.push_back(temp);
            humidityList.push_back(hum);
            xSemaphoreGive(dataMutex);

            Serial.printf("New Data Added - Temp: %.2f°C, Humidity: %.2f%%\n", temp, hum);
        } else {
            Serial.println("Failed to read from DHT sensor!");
        }
        vTaskDelay(MEASSUREMENT_INTERVAL / portTICK_PERIOD_MS);  // Delay for 30 minutes
    }
}

bool isMACAllowed(AsyncWebServerRequest *request) {
  // Extract client's IP
  IPAddress clientIP = request->client()->remoteIP();
    
  // Iterate through connected stations
  wifi_sta_list_t stationList;
  esp_wifi_ap_get_sta_list(&stationList);
  for (int i = 0; i < stationList.num; i++) {
    wifi_sta_info_t station = stationList.sta[i];
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
              station.mac[0], station.mac[1], station.mac[2],
              station.mac[3], station.mac[4], station.mac[5]);
        
    // Check if the MAC is allowed
    for (const char* allowedMAC : allowedMACs) {
      if (strcmp(macStr, allowedMAC) == 0) {
        Serial.printf("Client allowed: MAC=%s, IP=%s\n", macStr, clientIP.toString().c_str());
          return true;
        }
      }
  }
  Serial.printf("Access Denied: IP=%s\n", clientIP.toString().c_str());
  return false;
}

String getUptime() {
    // Get the number of milliseconds since the ESP32 was powered on
    unsigned long uptimeMillis = millis();

    // Convert to hours and minutes
    unsigned long uptimeSeconds = uptimeMillis / 1000;
    unsigned int hours = (uptimeSeconds / 3600) % 24;
    unsigned int minutes = (uptimeSeconds / 60) % 60;

    // Format the time as "HH:MM"
    char timeBuffer[6]; // HH:MM format needs 6 characters including null terminator
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", hours, minutes);

    return String(timeBuffer);
}


