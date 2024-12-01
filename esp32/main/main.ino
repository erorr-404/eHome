#include <AsyncTCP.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <deque>

const int DHT_PIN = 26;
const int MAX_DATA_LENGTH = 48; // Limit list to 48 values (24 hours of data if sampled every 30 mins)

// Wifi name and password
const char* ssid = "Netis 2.4G";
const char* password = "password";

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

  server.on("/prev", HTTP_GET, [](AsyncWebServerRequest *request){
    JsonDocument doc;
    
    JsonArray data = doc.createNestedArray("data");
    
    xSemaphoreTake(dataMutex, portMAX_DELAY);
    for (size_t i = 0; i < temperatureList.size(); i++) {
        JsonObject reading = data.createNestedObject();
        reading["t"] = temperatureList[i];
        reading["h"] = humidityList[i];
    }
    xSemaphoreGive(dataMutex);
    
    String output;
    serializeJson(doc, output);

    request->send(200, "application/json", output);
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
        vTaskDelay(SAMPLE_INTERVAL / portTICK_PERIOD_MS);  // Delay for 30 minutes
    }
}
