#!/bin/bash


export mqttTopic="iotdemo/voltage/4711"
export mqttMessage="1999,1"

echo "Starting the producer to send messages "
java  -DmqttTopic=$mqttTopic -DmqttMessage=$mqttMessage -jar producer/target/iot_producer-jar-with-dependencies.jar
