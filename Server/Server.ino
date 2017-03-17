#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "grrandifi";
const char* password = "kissa123";

ESP8266WebServer server(80);

const int led = 13;
const int statusLed = D6;

// for led fade in and fade out
int statusBrightness = 0;
int fadeAmount = 5;

// status led fade in fade out 
unsigned long previousMillis = 0;
long statusBlinkDelay = 40;

// do not disturb timer in milliseconds
const int dndTime = 10000;
int dndTimeLeft = 0;
int dndPreviousMillis = 0;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void blinkStatusLed() {
  analogWrite(statusLed, statusBrightness);
  statusBrightness = statusBrightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (statusBrightness <= 0 || statusBrightness >= 255) {
    fadeAmount = -fadeAmount;
  }
}

void setup(void){
  pinMode(led, OUTPUT);
  pinMode(statusLed, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  unsigned long currentMillis = millis();
    
  server.handleClient();

  if (dndTimeLeft > 0) {
    if(currentMillis - previousMillis > statusBlinkDelay) {
      previousMillis = currentMillis;
      blinkStatusLed();
    }
    dndTimeLeft = dndTimeLeft - min(dndTimeLeft, currentMillis);
  }  
}
