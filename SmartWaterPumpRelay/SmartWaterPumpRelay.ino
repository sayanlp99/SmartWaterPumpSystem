#include <ESP8266WiFi.h>

void handlePump();
void readPump();
void fetchDataFromTank();

bool isTankFull;

String pumpStatus;
String waterLevel;
String response;
String espTankUrl = "http://10.0.0.7/";

int RELAY = 2;;

const char* ssid     = "Wi-Fi";
const char* password = "00000000";

WiFiServer server(80);

void fetchDataFromTank(){
  
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
  
}

bool isPumpOn(){
  if(digitalRead(RELAY)){
    Serial.println("PUMP is on");
    return true;
  }
  else{
    Seial.println("PUMP is off");
    return false;
  }
}

void setup() {
  pinMode(RELAY, OUTPUT);
  Serial.begin(9600);
  Serial.println("=====ESP restarting=====")
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
  
}
