#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <string.h>

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
  digitalWrite(5, HIGH); 
}

void lightOff() {
 digitalWrite(5, LOW);  
}

void blink(int count) {
  for(unsigned i = 1; i <= count; ++i) {
      lightOn();
      delay(500);
      lightOff();
      delay(500);
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(5, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  lightOff();
  
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
}


void callback(char* topic, byte* payload, unsigned int length) {
  String text = ((char*)payload);
  
  if ( strstr((char*)payload,"an") != NULL ) {
    Serial.println("an");
    lightOn();
  } else if ( strstr((char*)payload,"aus") != NULL ) {
    Serial.println("aus");
    lightOff() ;
  } else
    blink(10);
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
