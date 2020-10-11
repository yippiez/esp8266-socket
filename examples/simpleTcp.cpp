#include <Arduino.h>

#include "EasySocket.h"
#include <ESP8266WiFi.h>
#include <IPAddress.h>


TCPSocket s; // creates the socket
const char* ssid = "ssid";
const char* password = "pass";


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
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());

  Serial.println("Creating the socket.");
  s.connect(IPAddress(192,168,1,37), 1342); // connect the socket to ip:port
}

void loop() {
  delay(500);
  s.send("test", 4); // 4 is the length of the message
  Serial.println("Send a message.");
}