#include "mraa.hpp"
#include <iostream>
#include <unistd.h>
#include <mosquitto.h>
#include <string.h>
#include "SFE_LSM9DS0.h"

using namespace std;


void my_publish_callback(struct mosquitto *mosq, void *obj, int mid)
{
  printf("message published\n");

}

int main()
{
  LSM9DS0 *imu;
  imu = new LSM9DS0(0x6B, 0x1D);

  char id[30];
  bool clean_session 		= true;
  struct mosquitto *mosq 	= NULL;
  char *host 				= "192.168.178.118";
  int port 					= 1883;
  int keepalive 			= 60;
  char *bind_addr 			= NULL;
  int count					= 0;
  char message[50];
  char topicGyro[50];

  mosquitto_lib_init();

  mosq = mosquitto_new(id, clean_session, NULL);
  if(!mosq){
	  printf("Error:Out of memory\n");
	  return 1;
  }

  mosquitto_publish_callback_set(mosq, my_publish_callback);

  mosquitto_username_pw_set(mosq, "admin", "change12_me");

  int ret = mosquitto_connect_bind (mosq, host, port, keepalive, bind_addr);
  if (ret) {
      fprintf (stderr, "Can't connect to Mosquitto server\n");
      exit (-1);
   }


  uint16_t imuResult = imu->begin();
  cout<<hex<<"Chip ID: 0x"<<imuResult<<dec<<" (should be 0x49d4)"<<endl;

  bool newGyroData = false;
  bool overflow = false;

  // Create String for MQTT Topics
  sprintf (topicGyro, "iotdemo/gyroscope/%d", imuResult);

  mosquitto_loop_start(mosq);

  // Loop and report data
  while (1)
  {
    while ((newGyroData ) != true)
    {
      if (newGyroData != true)
      {
        newGyroData = imu->newGData();
      }
    }

    newGyroData = false;

    // Of course, we may care if an overflow occurred; we can check that
    //  easily enough from an internal register on the part. There are functions
    //  to check for overflow per device.
    overflow = imu->gDataOverflow();

    if (overflow)
    {
      cout<<"WARNING: DATA OVERFLOW!!!"<<endl;
    }

    imu->readGyro();

    sprintf (message, "%f, %f, %f, %d", imu->calcGyro(imu->gx),
    		                            imu->calcGyro(imu->gy),
									    imu->calcGyro(imu->gz), count);

    cout<<"Message to '"<<topicGyro<<"' is '"<<message<<"'\n";

    ret = mosquitto_publish (mosq, NULL, topicGyro, strlen(message), message, 0, false);
    if (ret) {
    	fprintf (stderr, "Can't publish to Mosquitto server\n");
    	exit (-1);
    }

    count ++;
    sleep(1);
  }

  mosquitto_disconnect(mosq);

  mosquitto_loop_stop(mosq, false);

  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();

  return MRAA_SUCCESS;
}
