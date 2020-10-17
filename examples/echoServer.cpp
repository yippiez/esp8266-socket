#include <Arduino.h>
#include "EasySocket.h"
#include <ESP8266WiFi.h>
#include <IPAddress.h>


TCPSocket *s = new TCPSocket; // creates the socket
TCPSocket *client;

const char* ssid = "ssid";
const char* password = "pass";
char buffer[512];

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
  Serial.println(WiFi.localIP());// Print the IP address

  Serial.println("Creating the echo server.");
  s->bind(WiFi.localIP(), 1342);
  s->listen();
}

void loop() {
  // put your main code here, to run repeatedly:

  client = s->accept();
  
  delay(1000);
  Serial.println("Waiting for a client");
  if(client){
    Serial.println("Got a client");
  }

  while(client){
    delay(1000);

    if(!client->conn_alive){
      break;
    }

    if(client->data_available() == 1){
      client->read(buffer);
      Serial.println(buffer);
      client->send(buffer);
      memset(buffer,0,512);
    }
  }
  client = NULL;
}