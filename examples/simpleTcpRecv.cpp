#include <Arduino.h>

#include "EasySocket.h"
#include <ESP8266WiFi.h>
#include <IPAddress.h>


TCPSocket s;
const char* ssid = "ssid";
const char* password = "pass";

void setup() {
  // put your setup code here, to run once:
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
  Serial.print(WiFi.localIP());// Print the IP address

  Serial.println("Creating the socket.");
  s.connect(IPAddress(192,168,1,37), 1342);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  s.send("test", 4); 
  Serial.println(s.read());
}