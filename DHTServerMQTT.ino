/*
  DHTServer - ESP8266 with a DHT sensor as an input
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <PubSubClient.h>

/************************* DHT22 Sensor *********************************/
#define DHTTYPE DHT22
#define DHTPIN  02

/************************* WiFi Access Point *********************************/
const char* ssid     = "Steiner_Keller";
const char* password = "pat4president";

/************************* MQTT Server *********************************/
char* mqtt_server           = "192.168.178.80";
int mqtt_server_port        = 1883;
const char* mqtt_user       = "admin";
const char* mqtt_password   = "admin";
String      message         = "";
String      topicTemp       = "";
String      topicHumid      = "";

/************************* ESP8266 WiFiClient *********************************/
WiFiClient wifiClient;

/************************* MQTT client *********************************/
PubSubClient client(mqtt_server, mqtt_server_port, wifiClient );

/************************* DHT Sensor *********************************/
DHT dht(DHTPIN, DHTTYPE, 11);


float         humidity, temp_f;           // Values read from sensor

unsigned long previousMillis = 0;         // will store last temp was read
const long    interval = 2000;            // interval at which to read sensor

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
    temp_f = dht.readTemperature(true);     // Read temperature as Fahrenheit
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
  topicTemp       = "temperature/"+ String( ESP.getChipId() );
  topicHumid      = "humidity/"+ String( ESP.getChipId() );

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
    Serial.print(" ");

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

  // Now we can publish stuff!
  message = "<value>" + String((int)temp_f) + "</value>";

  Serial.print(F("\nSending temperature value "));
  Serial.print(message);
  client.publish(topicTemp.c_str(), message.c_str());

  message = "<value>" + String((int)humidity) + "</value>";
  Serial.print(F("\nSending humidity value "));
  Serial.print(message);
  client.publish(topicHumid.c_str(), message.c_str());

  delay(1000);
}
