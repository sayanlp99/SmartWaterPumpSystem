#include <ESP8266WiFi.h>

void handleRoot();
void handleRelay();

const char* ssid     = "ESP8266";
const char* password = "98989898";

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>Smart Water pump System</title>
  <style>
    font-family: Helvetica;
    display: inline-block;
    margin: 0px auto;
    text-align: center;
      .button{
        background-color: #195B6A;
        border: none;
        color: white;
        padding: 16px 40px
      };
      text-decoration: none;
      font-size: 30px;
      margin: 2px;
      cursor: pointer;
      .button2{
        background-color: #77878A
      };
    </style>
</head>
<body>

</body>
</html>
)=====";

WiFiServer server(80);

void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

bool isTankFull(){
}

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if(WiFi.status()==WL_CONNECTED)
  {
    server.on("/", handleRoot);
    server.on("/turn_relay_on", handleRelay); //form action is handled here
  }
  server.begin();
}

void loop() {
  getDataFromTank();

}
