#!/bin/bash

export receiverURL="192.168.178.103"
export mqttTopic="iotdemo/temperature/4711"
# export mqttTopic="iotdemocommand/light"
export mqttMessage="30,0"
# export mqttMessage="an"

if [ $# -eq 1 ]
  then
    export mqttMessage=$1
fi

echo "Starting the producer to send messages "
java -DreceiverURL=$receiverURL -DmqttTopic=$mqttTopic -DmqttMessage=$mqttMessage -jar producer/target/iot_producer-jar-with-dependencies.jar
