#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <WiFiUdp.h>
#include <FastLED.h>
#include "./index.h"

//WiFi setup
#define AP_NAME "WifiChristmasTree"
#define HTTP_MIME "text/html"
#define HTTP_OK_CODE 200
#define HTTP_ERROR_CODE 500
#define HTTP_NOTFOUND_CODE 404
#define WIFI_STATUS_AP 0
#define WIFI_STATUS_CONNECTING 1
#define WIFI_STATUS_CONNECTED 2
#define WIFI_CONNECTION_TIMEOUT_SECONDS 30
AsyncWebServer server(80);
const char* TOKENS_PLACEHOLDER = "*TREE_STATE*";
const char* BAD_RESPONSE = "BAD";
unsigned char apName[50];
unsigned char apPassword[50];
boolean updateWiFi = false;
int mode = 0;
int speed = 50;
int connectionStatus = WIFI_STATUS_CONNECTING;

//UdpSetup
#define GET_IP_CMD "getIp"
WiFiUDP Client;
#define UDP_PORT 2121
char packetBuffer[512]; 

//LedSetup
#define NUM_LEDS 8
#define DATA_PIN D4
#define NUM_MODES 4
#define NUM_RB_COLS 7
#define LED_TYPE WS2811
unsigned int (* modeFns[NUM_MODES])(unsigned int);
long rainbow[] = {0xFF0000, 0x4b0082, 0xFFFF00, 0x00FF00, 0x0000FF, 0xFF4000, 0x70004a};
CRGB leds[NUM_LEDS];
unsigned int itrPos = 0;
unsigned int lightPos = 0;

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
    lightPos = 0;
    Serial.print("Mode updated to ");
    Serial.println(mode);
    request->send_P(HTTP_OK_CODE, HTTP_MIME, TOKENS_PLACEHOLDER, processor);
  }
  else{
    request->send_P(HTTP_ERROR_CODE, HTTP_MIME, BAD_RESPONSE);
  }
}

void handleConnect(AsyncWebServerRequest *request){
  if(request->hasParam("apName") && request->hasParam("apPassword")){
      connectionStatus = WIFI_STATUS_CONNECTING;
      request->arg("apName").getBytes(apName, sizeof(apName));
      request->arg("apPassword").getBytes(apPassword, sizeof(apPassword));
      updateWiFi = true;
      request->send_P(HTTP_OK_CODE, HTTP_MIME, TOKENS_PLACEHOLDER, processor);   
  }else{
      request->send_P(HTTP_ERROR_CODE, HTTP_MIME, TOKENS_PLACEHOLDER, processor);   
  }
}

void handleGetStatus(AsyncWebServerRequest *request){
  request->send_P(HTTP_OK_CODE, HTTP_MIME, TOKENS_PLACEHOLDER, processor);
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
  WiFi.mode(WIFI_AP);
  boolean result = WiFi.softAP(AP_NAME);
  if(result == true)
  {
    connectionStatus = WIFI_STATUS_AP;
    Serial.println("Ready");
    Serial.println(WiFi.softAPIP());
  }
  else
  {
    Serial.println("Failed!");
  }
}

boolean connectToWiFi(boolean updateSettings){
  WiFi.mode(WIFI_STA);
  if(updateSettings)
  {
      WiFi.begin((char*)apName, (char*)apPassword);
  }
  else
  {
      WiFi.begin();
  }
  
  for(int i=0; i< WIFI_CONNECTION_TIMEOUT_SECONDS; i++){
    if(WiFi.status() == WL_CONNECTED){
      break;
    }
    delay(1000);
    Serial.print('.');
  }

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Connection to wifi failed");
    return false;
  }
  connectionStatus = WIFI_STATUS_CONNECTED;
  Serial.println("Connection established!");
  Serial.print("WiFi SSID:");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");  
  Serial.println(WiFi.localIP());
  return true;
}


//Functions for flashing the lights
//This function is for the "chase pattern.
unsigned int setLedChase(unsigned int inPos){
  const unsigned int slowFactor = 5;
  unsigned int slowPos = inPos /slowFactor;
  int stepPos = NUM_LEDS - (slowPos % NUM_LEDS) -1;
  unsigned int rainbowPos = (slowPos / NUM_LEDS) % NUM_RB_COLS;
  
  boolean flipped = false;
  for(uint16_t i=0; i<NUM_LEDS; i++){
    uint8_t r,g,b;

    if(i > stepPos && !flipped){
      rainbowPos++;
      if(7 <= rainbowPos)
          rainbowPos =0;
      flipped = true;
    }
    
    leds[i].g = (rainbow[rainbowPos] & 0xFF0000)>>16;
    leds[i].r = (rainbow[rainbowPos] & 0x00FF00)>>8;
    leds[i].b = rainbow[rainbowPos] &0xFF;    
  }

  inPos++;
  if(inPos == (NUM_LEDS * NUM_RB_COLS *slowFactor))
      inPos = 0;
  return inPos;
}

//This is the function for the rainbow pattern
unsigned int setLedsRainbow(unsigned int inPos)
{
    
    int howWhite = inPos %256;
    unsigned long changing = (inPos/(256)) % NUM_LEDS;
    unsigned long rainbowPos = (inPos/(256*NUM_LEDS)) % NUM_RB_COLS;
    for(uint16_t i=0; i<NUM_LEDS; i++){
      uint8_t r,g,b;
      //CRGB pix;
      if(i!=changing)
      {
        //red and green channels are swapped (WTF)
        leds[i].g = (rainbow[rainbowPos] & 0xFF0000)>>16;
        leds[i].r = (rainbow[rainbowPos] & 0x00FF00)>>8;
        leds[i].b = rainbow[rainbowPos] &0xFF;
        rainbowPos++;
        if(rainbowPos >= NUM_RB_COLS)
            rainbowPos =0;
      }
      else{
        long nextCol = rainbow[(rainbowPos +1) % NUM_RB_COLS];
        int howColoured = 255-howWhite;
        leds[i].g =  (((nextCol &0xFF0000) >>16  )*howColoured + 0xFF*howWhite)/255;
        leds[i].r =  (((nextCol &0xFF00) >>8  )*howColoured + 0xFF*howWhite)/255;
        leds[i].b =  (((nextCol &0xFF)  )*howColoured + 0xFF*howWhite)/255;        
      }      
      
    }
    
    inPos++;
    if((256*NUM_LEDS*sizeof(rainbow))==inPos){
        inPos =0;
    }
     
    return inPos;
}

//Function for the DoublePeak pattern
unsigned int setLedsDoublePeak(unsigned int inPos){

      unsigned short pairNum= inPos / 512;
      short pos = inPos%512;
      unsigned short adjust = 0;
      if(pos < 256)
          adjust = pos;
      else
          adjust = 511 - pos;
  
      for (uint16_t i=0; i<NUM_LEDS; i++)
      {
        short pair = (i/2) %2;
        unsigned short thisAdj = ( pair == pairNum) ? adjust :0;
        leds[i].g = (i%2==0)?  0xFF : thisAdj;
        leds[i].r = (i%2==1)?  0xFF : thisAdj;
        leds[i].b = thisAdj;        
      }
      
      inPos++;
      
     if(1024 == inPos)
          inPos =0;

    return inPos;
}

//This is the function when the LEDS are to be just runed off
unsigned int setLedsOff(unsigned int inPos){
  for (uint16_t i=0; i<NUM_LEDS; i++)
  {    
    leds[i].r = leds[i].g = leds[i].b = 0;
  }
  return 1;
}

void setup()
{  
  Serial.begin(115200);
  modeFns[0] = setLedChase;
  modeFns[1] = setLedsRainbow;
  modeFns[2] = setLedsDoublePeak;
  modeFns[3] = setLedsOff;
  FastLED.addLeds<LED_TYPE, DATA_PIN, GRB>(leds, NUM_LEDS);
  if(!connectToWiFi(false)){
    setupAp();
  }
  Client.begin(UDP_PORT);
  setupWebServer();
}

void loop()
{
  if(updateWiFi){
    updateWiFi = false;
    if(!connectToWiFi(true)){
      setupAp();
    }
  }

  if((itrPos * 1000)  > (100000/(speed+1))) {
    lightPos = modeFns[mode](lightPos);    
    FastLED.show();
    itrPos=0;
  }
  else{
    itrPos++;
  }

  int noBytes = Client.parsePacket();
  if(noBytes){
    Client.read(packetBuffer,noBytes); 
    Serial.print("Got Packet:'");
    Serial.print(packetBuffer);
    Serial.println("'");
    if(0 == strcmp(GET_IP_CMD, packetBuffer) && connectionStatus != WIFI_STATUS_CONNECTING){
      Client.beginPacket(Client.remoteIP(), UDP_PORT);
      if(connectionStatus == WIFI_STATUS_CONNECTED){
        Client.print(WiFi.localIP());
      }
      else if(connectionStatus == WIFI_STATUS_AP){
        Client.print(WiFi.softAPIP());
      }
      
      Client.endPacket();
    }
  }

  delay(1);
}
