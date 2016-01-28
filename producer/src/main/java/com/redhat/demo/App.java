package com.redhat.demo;

import javax.jms.ConnectionFactory;

import org.apache.activemq.ActiveMQConnectionFactory;


/**
 * Hello world!
 *
 */
public class App 
{
	
	// Default Values for message producer
    private static final String DEFAULT_RECEIVER	= "localhost";
    private static final String DEFAULT_MQTT_TOPIC  = "iotdemo/voltage/4711";
    private static final String DEFAULT_MQTT_MESSAGE = "2000,1";
 
	 
    public static void main( String[] args ) throws Exception
    {
    	
    	Producer		   producer;
       
        String brokerURLMQTT = "tcp://" + System.getProperty("receiverURL",DEFAULT_RECEIVER) +  ":1883";
        String mqttTopic = System.getProperty("mqttTopic",DEFAULT_MQTT_TOPIC);
        String mqttMessage = System.getProperty("mqttMessage",DEFAULT_MQTT_MESSAGE);
               
        
    	producer = new MqttProducer(brokerURLMQTT, "admin", "change12_me", "mqtt.receiver" );
        
 		producer.run( mqttTopic, mqttMessage );
		    
        producer.close();
    }
}
