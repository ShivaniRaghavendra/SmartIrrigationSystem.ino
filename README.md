# Smart Irrigation System with ESP32

This project implements an automated irrigation system using an ESP32 microcontroller. It monitors soil moisture, rain detection, temperature, and humidity, and controls a water pump accordingly. The system publishes sensor data to an MQTT broker and uploads it to ThingSpeak for remote monitoring.

## Features
- **Soil Moisture Monitoring:** Reads soil moisture levels and activates a water pump if the soil is too dry.
- **Rain Detection:** Determines whether it is raining using an analog and digital rain sensor.
- **Temperature & Humidity Monitoring:** Uses a DHT11 sensor to measure environmental conditions.
- **MQTT Communication:** Publishes sensor readings and pump status to an MQTT broker.
- **ThingSpeak Integration:** Sends sensor data to ThingSpeak for remote monitoring.

## Components Used
- ESP32
- DHT11 Temperature & Humidity Sensor
- Soil Moisture Sensor
- Rain Sensor (Analog & Digital)
- Relay Module (to control the water pump)
- LEDs (for temperature and humidity alerts)

## Circuit Connections
| Component          | ESP32 Pin |
|-------------------|-----------|
| DHT11 Sensor     | GPIO 13   |
| Soil Moisture Sensor | GPIO 33   |
| Rain Sensor (Analog) | GPIO 34   |
| Rain Sensor (Digital) | GPIO 5    |
| Relay Module (Pump) | GPIO 18   |
| Temperature LED  | GPIO 35   |
| Humidity LED     | GPIO 32   |

## Installation & Setup
### 1. Install Required Libraries
Ensure you have the following libraries installed in the Arduino IDE:
- `WiFi.h`
- `PubSubClient.h`
- `HTTPClient.h`
- `DHT.h`

### 2. Update WiFi & MQTT Credentials
Modify the following lines in the code to match your network and MQTT broker details:
```cpp
const char* ssid = "Your_SSID";
const char* password = "Your_Password";
const char* mqtt_server = "Your_MQTT_Broker_IP";
```

### 3. Update ThingSpeak API Key
Replace the API key in the code:
```cpp
const char* thingSpeakApiKey = "Your_ThingSpeak_API_Key";
```

### 4. Upload the Code
Compile and upload the code to your ESP32 using Arduino IDE.

## Usage
- The ESP32 continuously reads sensor values and controls the water pump based on soil moisture levels.
- Sensor readings and pump status are published to the MQTT broker.
- Data is sent to ThingSpeak for remote monitoring.
- LED indicators turn on if temperature or humidity exceed set thresholds.

## MQTT Topics
| Topic                  | Description |
|-----------------------|-------------|
| `irrigation/soilMoisture` | Soil moisture value |
| `irrigation/temperature`  | Temperature reading |
| `irrigation/humidity`     | Humidity reading |
| `irrigation/rain`         | Rain detection status |
| `irrigation/pumpControl`  | Pump ON/OFF status |

## Future Enhancements
- Mobile app integration for real-time monitoring
- Advanced calibration of soil moisture thresholds
- Solar-powered operation

## License
This project is open-source under the MIT License.

