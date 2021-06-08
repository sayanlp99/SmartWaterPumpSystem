#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>

long readUltrasonicDistance(int, int);
void calculateTank();
void getDataFromTank();

long cm = 0;
long v = 0;
long volume = 354493.44;
long r = 336;

int TRIGGER_PIN = 0;
int ECHO_PIN = 1;

const char* ssid     = "Sam_2.4";
const char* password = "7980362177";

String response;

//HTTPClient http;
ESP8266WebServer server(80);

long readUltrasonicDistance(int triggerPin, int echoPin){
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds *0.01723
  return (pulseIn(echoPin, HIGH)*0.01723);
}

void calculateTank(){
  cm =readUltrasonicDistance(TRIGGER_PIN, ECHO_PIN);
  v = 100.0-((3.14*(r^2)*cm)/volume)*100.0;
  Serial.print("Tank is: ");
  Serial.print(v);
  Serial.print("% full !\n");
}

void getDataFromTank(){
  calculateTank();
  response = "{\"water_level\":\"" + String(v) + "\", \"pump_status\":\""+0+"\"}";
  Serial.println("handlePump: " + response);
  server.send(200, "text/html", response);
}

void setup(){
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
    server.on("/getDataFromTank", getDataFromTank);
//    server.on("/changePumpState", handlePump);
  }
  server.begin();
}

void loop(){
  server.handleClient();
}
