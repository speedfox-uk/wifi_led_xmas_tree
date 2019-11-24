#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include "./index.h"

#define AP_NAME "WifiChristmasTree"
#define HTTP_MIME "text/html"
#define HTTP_OK_CODE 200
#define HTTP_ERROR_CODE 500
#define HTTP_NOTFOUND_CODE 404
#define WIFI_STATUS_AP 0
#define WIFI_STATUS_CONNECTING 1
#define WIFI_STATUS_CONNECTED 2
AsyncWebServer server(80);
const char* TOKENS_PLACEHOLDER = "*TREE_STATE*";
const char* BAD_RESPONSE = "BAD";
char apName[50];
char apPassword[50];
boolean connectToWiFi = false;
int mode = 0;
int speed = 50;
int connectionStatus = WIFI_STATUS_AP;

String processor(const String& var)
{
  char valBuffer[20];
  if(var == "TREE_STATE"){
    sprintf(valBuffer, "%d,%d,%d", mode, speed, connectionStatus);
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
    request->send_P(HTTP_OK_CODE, HTTP_MIME, TOKENS_PLACEHOLDER, processor);
  }
  else{
    request->send_P(HTTP_ERROR_CODE, HTTP_MIME, BAD_RESPONSE);
  }
}

void handleSetMode(AsyncWebServerRequest *request){
  if(request->hasParam("mode")){
    String arg = request->arg("mode");
    mode = arg.toInt();
    Serial.print("Mode updated to ");
    Serial.println(mode);
    request->send_P(HTTP_OK_CODE, HTTP_MIME, TOKENS_PLACEHOLDER, processor);
  }
  else{
    request->send_P(HTTP_ERROR_CODE, HTTP_MIME, BAD_RESPONSE);
  }
}

void handleConnect(AsyncWebServerRequest *request){
  connectionStatus = WIFI_STATUS_CONNECTING;
  request->send_P(HTTP_OK_CODE, HTTP_MIME, TOKENS_PLACEHOLDER, processor);
}

void handleGetStatus(AsyncWebServerRequest *request){
  request->send_P(HTTP_OK_CODE, HTTP_MIME, TOKENS_PLACEHOLDER, processor);
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
  server.on("/connect", HTTP_ANY, handleConnect);
  server.on("/getStatus", HTTP_ANY, handleGetStatus);
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
  if(connectToWiFi){
  }
}
