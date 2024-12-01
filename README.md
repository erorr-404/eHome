# Description

This is a service for monitoring electricity, temperature, humidity, sound, light, noise at howe via mobile app.

## Hardware

* **ESP32** chip
* **DHT11** temperature and humidity sensor
* Cheap Chineese noname microphone I got in Arduino starter kit
* Photoresistor

## Work scheme

### Electricity check

I use ESP32 as a server, that plugged into rosette. If mobile client cant reach server, it means there is no electicity at home. It also means client cant get temperature, humidity and other info.

### Temperature and humidity check

Every time ESP32 gets request, it grabs values from DHT11 sensor and sends them as response in JSON object.

### Microphone

Not implemented yet.

### Light

Not implemented yet.
