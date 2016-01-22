/*
  DHTServer - ESP8266 with a DHT sensor as an input
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <PubSubClient.h>


extern "C" {
#include "user_interface.h"
uint16 readvdd33(void);
}

/************************* DHT22 Sensor *********************************/
#define DHTTYPE DHT22
#define DHTPIN  02

/************************* WiFi Access Point *********************************/
const char* ssid     = "iotdemo";
const char* password = "change12_me";

/************************* MQTT Server *********************************/
char* mqtt_server           = "192.168.42.1";
int mqtt_server_port        = 1883;
const char* mqtt_user       = "admin";
const char* mqtt_password   = "change12_me";
String      message         = "";
String      topicTemp       = "";
String      topicHumid      = "";
String      topicVoltage    = "";

/************************* ESP8266 WiFiClient *********************************/
WiFiClient wifiClient;

/************************* MQTT client *********************************/
PubSubClient client(mqtt_server, mqtt_server_port, wifiClient );

/************************* DHT Sensor *********************************/
DHT dht(DHTPIN, DHTTYPE, 11);


float         humidity, temp_f;           // Values read from sensor
int           voltage;                    // ESP voltage

unsigned long previousMillis = 0;         // will store last temp was read
const long    interval = 2000;            // interval at which to read sensor

unsigned long count = 0;                  // counter for messagepoints

/*************not used yet, for subscription of messages ******************************/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/************* Utility function to retrieve data from DHT22 ******************************/
void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_f = dht.readTemperature();         // Read temperature as Celsius
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp_f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}

/************* Functionname says it all! ******************************/
void setup(void) {

  Serial.begin(115200);
  dht.begin();

  // Create String for MQTT Topics
  topicTemp       = "iotdemo/temperature/"+ String( ESP.getChipId() );
  topicHumid      = "iotdemo/humidity/"+ String( ESP.getChipId() );
  topicVoltage    = "iotdemo/voltage/"+ String( ESP.getChipId() );

  Serial.print("Chip-ID =");
  Serial.print ( ESP.getChipId() );

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rConnecting to ");
  Serial.println(ssid);


  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n\rESP8266 & DHT22 based temperature and humidity sensor working!");
  Serial.print("\n\rIP address: ");
  Serial.println(WiFi.localIP());
}

/******* Utility function to connect or re-connect to MQTT-Server ********************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(mqtt_server);
    Serial.print(" with ");
    Serial.print(mqtt_user);
    Serial.print(" / ");
    Serial.print(mqtt_password);

    // Attempt to connect
    if (client.connect(mqtt_server, mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/************* Functionname says it all! ******************************/
void loop(void) {


  if (!client.connected()) {  // Connect to mqtt broker
    reconnect();
  }
  client.loop();

  gettemperature();           // read sensordata
  count = count +1;           // increase counter

  voltage = readvdd33();

  // Now we can publish stuff!
  message = String((int)temp_f) + ", " + String(count);

  Serial.print(F("\nSending temperature value in Celsius <"));
  Serial.print(message);
  Serial.print(">");
  client.publish(topicTemp.c_str(), message.c_str());

  message = String((int)humidity) + ", " + String(count);

  Serial.print(F("\nSending humidity value <"));
  Serial.print(message);
  Serial.print(">");
  client.publish(topicHumid.c_str(), message.c_str());

  message = String((int)voltage) + ", " + String(count);

  Serial.print(F("\nSending sensor voltage <"));
  Serial.print(message);
  Serial.print(">");
  client.publish(topicVoltage.c_str(), message.c_str());

  delay(1000);
}
