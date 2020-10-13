#include <Arduino.h>
#include "EasySocket.h"
#include <ESP8266WiFi.h>
#include <IPAddress.h>


TCPSocket s;
const char* ssid = "ssid";
const char* password = "pass";
char buffer[512];

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print(WiFi.localIP());// Print the IP address

  Serial.println("Creating the reverse echo server.");
  s.connect(IPAddress(192,168,1,37), 1342);
}

void loop() {
  delay(500);
  if(s.data_available() != -1){
    s.read(buffer);
    s.send(buffer);
    Serial.print(buffer);
    memset(buffer, 0, 512); // clean the buffer
    }
}