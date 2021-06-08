#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <RTClib.h>


RTC_DS3231 rtc;

bool isPumpOn();
void handlePump();
void readPump();
void fetchDataFromTank();
void getTime();

bool isTankFull;

String pumpStatus;
String waterLevel;
String response;
String nowTime;
String espTankUrl = "http://10.0.0.7/";

int RELAY = 2;

const char* ssid     = "Sam_2.4";
const char* password = "7980362177";

HTTPClient http;
ESP8266WebServer server(80);

void fetchDataFromTank(){
  String query = espTankUrl + "getDataFromTank";
  http.begin(query);
  int httpCode = http.GET();
  Serial.print(F("HTTPCODE:"));
  Serial.println(httpCode);
  if (httpCode > 0){
    String payload = http.getString();
    Serial.print(F("Raw Data from Internet:"));
    Serial.println(payload);
    const size_t capacity = JSON_OBJECT_SIZE(2) + 40;
    DynamicJsonBuffer jsonBuffer(capacity);
    const char* json = payload.c_str();
    JsonObject& root = jsonBuffer.parseObject(json);
    const char* water_level = root["water_level"];
    const char* pump_status = root["pump_status"];
    waterLevel = water_level;
    pumpStatus = pump_status;
    http.end();
    Serial.println("W: " + waterLevel + "    P: " + pumpStatus);
  }
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

void getTime(){
  nowTime = "";
  DateTime now = rtc.now();
  if ((now.hour()) < 10)
    nowTime += "0";
  nowTime += now.hour();
  nowTime += ":";
  if ((now.minute()) < 10)
    nowTime += "0";
  nowTime += now.minute();
  Serial.print(F("Time:"));
  Serial.println(nowTime);
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
  if(nowTime == "5:00" || nowTime == "5:01"){
    fetchDataFromTank();
    if(waterLevel != "100" && pumpStatus != "1"){
      digitalWrite(RELAY, HIGH);  
    }
    else{
      Serial.println("Water is full or Pump is already on");  
    }
  }
  delay(5000);
}
