/**
 * Connected Cactus Application - Send soil moisture values to the cloud with Watson IoT Platform
 * Original idea by Travis Claypool from TechShop Round Rock
 *
 * The base code for connectivity to the platform is based off of the recipe by Ant elder to connect
 * an esp8266 to the Watson IoT Platform.
 * It can be found here (https://developer.ibm.com/recipes/tutorials/connect-an-esp8266-with-the-arduino-sdk-to-the-ibm-iot-foundation/).
 *
 *
 * Authors: Ant Elder, Oliver Rodriguez, Travis Claypool
 * License: Apache License v2
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient/releases/tag/v2.3

//-------- Customise these values -----------
//The two variables below are your wireless network credentials to connect to wifi
const char* ssid = "";
const char* password = "";

int sensorPin = A0; 
int sensorValue = 0;

//The following variables come from the Watson Iot Platform after you register your device 
#define ORG ""
#define DEVICE_TYPE ""
#define DEVICE_ID ""
#define TOKEN ""
//-------- Customise the above values --------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/status/fmt/json";
char subTopic[]= "iot-2/cmd/command/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void setup() {
   pinMode(5, OUTPUT);
 Serial.begin(115200);
 Serial.println();

 Serial.print("Connecting to ");
 Serial.print(ssid);
 //if (strcmp (WiFi.SSID(), ssid) != 0) {
 WiFi.begin(ssid, password);
 //}
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");

 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP()); 

 
  Serial.println("");
  
  client.setCallback(callback);
}

void loop() {
  
  //Invoke connection function*/
  iotfConnection();
  
  //Invoke publish function*/
  iotfPublish();
 
  digitalWrite(5, HIGH);
  delay(500);
  sensorValue = analogRead(sensorPin);
  digitalWrite(5, LOW); 
 
}

void iotfConnection() {
  if (!!!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
     }
     Serial.println();
     iotfSubscribe();
   }
 client.loop();  
}

void iotfPublish() {

 String payload = "{\"d\":{\"device\":\"";
 payload += DEVICE_ID;
 payload += "\",\"moisture\": \"";
 payload += sensorValue;
 payload += "\"}}";

 Serial.print("Sending payload: ");
 Serial.println(payload);
 
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
 
 delay(1000); 

}

void iotfSubscribe() {
  client.subscribe(subTopic);  
}

void callback(char* subTopic, byte* payload, unsigned int length) {
 Serial.println("callback invoked");
 
  // copy the payload content into a char*
  char* data;
  data = (char*) malloc(length + 1);
  memcpy(data, payload, length);
  data[length] = '\0';
  Serial.println(String(data));
  free(data);
}


