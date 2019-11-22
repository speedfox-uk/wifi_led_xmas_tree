#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include "./index.h"

#define AP_NAME "WifiChristmasTree"
#define HTTP_MIME "text/html"
#define HTTP_OK_CODE 200
AsyncWebServer server(80);


void handleIndexRequest(AsyncWebServerRequest *request){
  Serial.println("Sening main page");
  request->send(HTTP_OK_CODE,HTTP_MIME, indexPage);
}


void setup()
{
  Serial.begin(115200);
  Serial.println();  
  setupAp();
  setupWebServer();
}

void setupWebServer(){
  server.on("/", HTTP_ANY, handleIndexRequest);
  server.on("/index.html", HTTP_ANY, handleIndexRequest);
  server.begin(); 
}


void setupAp()
{
  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP(AP_NAME);
  if(result == true)
  {
    Serial.println("Ready");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    Serial.println("Failed!");
  }
}

void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
}
