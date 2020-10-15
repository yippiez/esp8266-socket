#include <Arduino.h>
#include "EasySocket.h"
#include <ESP8266WiFi.h>
#include <IPAddress.h>


TCPSocket *s = new TCPSocket; // creates the socket
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
  Serial.println(WiFi.localIP());// Print the IP address

  Serial.println("Creating the echo server.");
  s->bind(WiFi.localIP(), 1342);
  s->listen();
}

void loop() {

  TCPSocket *client = s->accept(); // returns 0/NULL when there is no connection
  delay(100);

  if(client){
    Serial.println("Got a client");
  }
  
  while(client){
    delay(500);
    if(client->data_available() == 1){
      client->read(buffer);
      Serial.println(buffer);
      client->send(buffer);
      memset(buffer,0,512);
    }
  }
}