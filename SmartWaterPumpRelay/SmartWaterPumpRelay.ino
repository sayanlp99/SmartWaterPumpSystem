#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>

WiFiClient wifiClient;


bool isPumpOn();
void handlePump();
void readPump();
void fetchDataFromTank();
void parseJSON(String);

bool isTankFull;

String pumpStatus;
String waterLevel;
String response;
String espTankUrl = "http://192.168.35.112/";

int RELAY = 2;

const char* ssid     = "Sam_2.4";
const char* password = "7980362177";

HTTPClient http;
ESP8266WebServer server(80);

void fetchDataFromTank(){
  String query = espTankUrl + "getDataFromTank";
  http.begin(wifiClient, query);
  int httpCode = http.GET();
  Serial.print(F("HTTPCODE:"));
  Serial.println(httpCode);
  if (httpCode > 0){
    String payload = http.getString();
    Serial.print(F("Raw Data from Internet:"));
    Serial.println(payload);
    parseJSON(payload);
    http.end();
    Serial.println("W: " + waterLevel + "    P: " + pumpStatus);
  }
}

int recWaterLevel;
int recPumpStatus;

void parseJSON(String jsonRes){
  char DincWl = jsonRes.charAt(17);
  Serial.println("DincWl: " + DincWl);
  String twL;
  String tPs;
  if(DincWl == '"'){
    Serial.println("if");
    twL = jsonRes.substring(16,17);
    Serial.println("twL: " + twL);
    tPs = jsonRes.substring(37,38);
    Serial.println("tPs: " + tPs);
  }
  else{
    Serial.println("else");
    twL = jsonRes.substring(16,18);
    Serial.println("twL: " + twL);
    tPs = jsonRes.substring(38,39);
    Serial.println("tPs: " + tPs);
  }
  pumpStatus = tPs;
  waterLevel = twL;
}

void handlePump(){
  if(isPumpOn()){
    response = "{\"message\":\"Pump already ON\"}";
    Serial.println("handlePump: " + response);
    server.send(200, "text/html", response);
  }
  else{
    digitalWrite(RELAY, HIGH);
    response = "{\"message\":\"Pump turned ON successfully\"}";
    Serial.println("handlePump: " + response);
    server.send(200, "text/html", response);
  }
}

void readPump(){
  Serial.println("Reading PUMP");
}

bool isPumpOn(){
  if(digitalRead(RELAY)){
    Serial.println("PUMP is on");
    return true;
  }
  else{
    Serial.println("PUMP is off");
    return false;
  }
}

void setup() {
  pinMode(RELAY, OUTPUT);
  Serial.begin(9600);
  Serial.println("=====ESP restarting=====");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if(WiFi.status()==WL_CONNECTED){
    server.on("/getPumpState", readPump);
    server.on("/changePumpState", handlePump);
  }
  server.begin();
}

void loop() {
//   if(nowTime == "5:00" || nowTime == "5:01"){
//     fetchDataFromTank();
//     if(waterLevel != "100" && pumpStatus != "1"){
//       digitalWrite(RELAY, HIGH);  
//     }
//     else{
//       Serial.println("Water is full or Pump is already on");  
//     }
//   }
//   delay(5000);
fetchDataFromTank();
delay(5000);
}
