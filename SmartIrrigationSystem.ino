#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi Credentials
const char* ssid = "Shivani";
const char* password = "1234567@";

// MQTT Broker
const char* mqtt_server = "192.168.13.68";
const int mqtt_port = 1883;
const char* mqtt_topic_moisture = "irrigation/soilMoisture";
const char* mqtt_topic_temp = "irrigation/temperature";
const char* mqtt_topic_humidity = "irrigation/humidity";
const char* mqtt_topic_rain = "irrigation/rain";
const char* mqtt_topic_pump = "irrigation/pumpControl";

// ThingSpeak API
const char* thingSpeakApiKey = "UEFBE2B2QFY7K9H9";  // Replace with your API Key
const char* thingSpeakURL = "http://api.thingspeak.com/update";

// Sensor & Actuator Pins
#define DHT_PIN 13
#define SOIL_MOISTURE_PIN 33
#define RAIN_ANALOG_PIN 34
#define RAIN_DIGITAL_PIN 5
#define RELAY_PIN 18
#define TEMP_LED 35
#define HUMIDITY_LED 32

// Thresholds
#define MOISTURE_THRESHOLD 2600
#define TEMP_THRESHOLD 30
#define HUMIDITY_THRESHOLD 80
#define RAIN_THRESHOLD 2000  // Adjust based on calibration

DHT dht(DHT_PIN, DHT11);
WiFiClient espClient;
PubSubClient client(espClient);

// Global Variables
int soilMoistureValue;
bool isRaining;
float temperature;
float humidity;

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");

    client.setServer(mqtt_server, mqtt_port);
    reconnectMQTT();

    dht.begin();
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    pinMode(RAIN_ANALOG_PIN, INPUT);
    pinMode(RAIN_DIGITAL_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(TEMP_LED, OUTPUT);
    pinMode(HUMIDITY_LED, OUTPUT);

    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(TEMP_LED, LOW);
    digitalWrite(HUMIDITY_LED, LOW);
}

void loop() {
    checkWiFi();
    if (!client.connected()) reconnectMQTT();

    // Read Soil Moisture
    soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

    // Read Rain Sensor
    int rainAnalog = analogRead(RAIN_ANALOG_PIN);
    int rainDigital = digitalRead(RAIN_DIGITAL_PIN);
    isRaining = (rainDigital == LOW) || (rainAnalog < RAIN_THRESHOLD);  // Adjust based on sensor type

    // Read DHT11 Sensor (Temperature & Humidity)
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp)) temperature = temp;
    if (!isnan(hum)) humidity = hum;

    // Print Sensor Values
    Serial.printf("Soil Moisture: %d | Rain: %s | Temp: %.2f°C | Humidity: %.2f%%\n", 
                  soilMoistureValue, isRaining ? "Yes" : "No", temperature, humidity);

    // Determine Soil Moisture Status
    if (soilMoistureValue > MOISTURE_THRESHOLD) {
        Serial.println("Soil is DRY. Turning Pump ON.");
        digitalWrite(RELAY_PIN, LOW);  // Activate pump
        client.publish(mqtt_topic_pump, "ON");
    } else {
        Serial.println("Soil is WET. Turning Pump OFF.");
        digitalWrite(RELAY_PIN, HIGH);  // Deactivate pump
        client.publish(mqtt_topic_pump, "OFF");
    }

    // Publish MQTT Data
    char msg[10];
    snprintf(msg, sizeof(msg), "%d", soilMoistureValue);
    client.publish(mqtt_topic_moisture, msg);

    snprintf(msg, sizeof(msg), "%.2f", temperature);
    client.publish(mqtt_topic_temp, msg);

    snprintf(msg, sizeof(msg), "%.2f", humidity);
    client.publish(mqtt_topic_humidity, msg);

    client.publish(mqtt_topic_rain, isRaining ? "1" : "0");

    // LED Control
    digitalWrite(TEMP_LED, temperature > TEMP_THRESHOLD);
    digitalWrite(HUMIDITY_LED, humidity > HUMIDITY_THRESHOLD);

    // Send data to ThingSpeak
    sendToThingSpeak();

    delay(5000);
}

void sendToThingSpeak() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(thingSpeakURL) + "?api_key=" + thingSpeakApiKey +
                     "&field1=" + String(soilMoistureValue) +
                     "&field2=" + String(temperature) +
                     "&field3=" + String(humidity) +
                     "&field4=" + String(isRaining ? 1 : 0);

        Serial.println("Sending data to ThingSpeak...");
        http.begin(url);
        int httpResponseCode = http.GET();

        Serial.println(httpResponseCode > 0 ? "ThingSpeak Response: " + String(httpResponseCode) : "Error sending data!");
        http.end();
    }
}

void checkWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected! Reconnecting...");
        WiFi.disconnect();
        WiFi.begin(ssid, password);
    }
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP32_Irrigation")) {
            Serial.println("Connected to MQTT");
            return;
        }
        Serial.println("MQTT connection failed! Retrying in 2s...");
        delay(2000);
    }
}
