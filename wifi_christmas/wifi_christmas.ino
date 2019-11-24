#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include "./index.h"

#define AP_NAME "WifiChristmasTree"
#define HTTP_MIME "text/html"
#define HTTP_OK_CODE 200
#define HTTP_ERROR_CODE 500
#define HTTP_NOTFOUND_CODE 404
AsyncWebServer server(80);
const char* tokens_placeholder = "*TREE_STATE*";
const char* bad_response = "BAD";
int mode = 0;
int speed = 50;

String processor(const String& var)
{
  char valBuffer[20];
  if(var == "TREE_STATE"){
    sprintf(valBuffer, "%d,%d", mode, speed);
    return String(valBuffer);
  }
  
  return String();
}

void handleIndexRequest(AsyncWebServerRequest *request){
  Serial.println("Sening main page");
  request->send_P(HTTP_OK_CODE, HTTP_MIME, indexPage, processor);
}

void handleSetSpeed(AsyncWebServerRequest *request){
  if(request->hasParam("speed")){
    String arg = request->arg("speed");
    speed = arg.toInt();
    Serial.print("Speed updated to ");
    Serial.println(speed);
    request->send_P(HTTP_OK_CODE, HTTP_MIME, tokens_placeholder, processor);
  }
  else{
    request->send_P(HTTP_ERROR_CODE, HTTP_MIME, bad_response);
  }
}

void handleSetMode(AsyncWebServerRequest *request){
  if(request->hasParam("mode")){
    String arg = request->arg("mode");
    mode = arg.toInt();
    Serial.print("Mode updated to ");
    Serial.println(mode);
    request->send_P(HTTP_OK_CODE, HTTP_MIME, tokens_placeholder, processor);
  }
  else{
    request->send_P(HTTP_ERROR_CODE, HTTP_MIME, bad_response);
  }
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
  server.on("/setSpeed", HTTP_ANY, handleSetSpeed);
  server.on("/setMode", HTTP_ANY, handleSetMode);
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
