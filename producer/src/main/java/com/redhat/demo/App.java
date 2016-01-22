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
 
	 
    public static void main( String[] args ) throws Exception
    {
    	
    	Producer		   producer;
       
        String brokerURLMQTT = "tcp://" + System.getProperty("receiverURL",DEFAULT_RECEIVER) +  ":1883";
               	
    	producer = new MqttProducer(brokerURLMQTT, "admin", "admin", "mqtt.receiver");
        
 		producer.run( "an" );
		    
        producer.close();
    }
}
