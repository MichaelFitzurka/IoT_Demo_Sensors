#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

/************************* WiFi Access Point *********************************/
const char* ssid     = "iotdemo";
const char* password = "change12_me";

WiFiClient wifiClient;

/************************* MQTT client *********************************/
char* mqtt_server           = "192.168.42.1";
int mqtt_server_port        = 1883;
const char* mqtt_user       = "admin";
const char* mqtt_password   = "change12_me";

/************************* Prototypes *********************************/
void callback(char* topic, byte* payload, unsigned int length);

PubSubClient client(mqtt_server, mqtt_server_port, callback, wifiClient);

void lightOn() {
  digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                    // but actually the LED is on; this is because 
                                    // it is acive low on the ESP-01)
}

void lightOff() {
 digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH 
}


void setup() {
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rConnecting to ");
  Serial.println(ssid);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n\rESP8266 lightswitch working!");
  Serial.print("\n\rIP address: ");
  Serial.println(WiFi.localIP());
  
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
}


void callback(char* topic, byte* payload, unsigned int length) {
  char message[11];
  
  memcpy(message, payload, sizeof payload);
  
  Serial.println("Received some data on topic");
  Serial.println(topic);
  Serial.println("Message received");
  Serial.println(message);
  
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

      // subscribe to topic
      if (client.subscribe("iotdemocommand/light")){
        Serial.println("Successfully subscribed");
      }
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// the loop function runs over and over again forever
void loop() {
  if (!client.connected()) {  // Connect to mqtt broker
    reconnect();
  }
  client.loop();

  
}
