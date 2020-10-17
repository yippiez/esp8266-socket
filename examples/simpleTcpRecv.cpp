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
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address

  Serial.println("\nCreating the socket.");
  s.connect(IPAddress(192,168,1,30), 1342);
}

void loop() {
  delay(500);
  s.send("test\n");
  if(s.data_available() == 1){
    s.read(buffer);
    Serial.println(buffer);
    memset(buffer, 0, 512);
    }
}