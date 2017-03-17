#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "dnd.h"
#include "halarm.h"

const char* ssid = "grrandifi";
const char* password = "kissa123";

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound(){

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
}

void handleDisturb() {
  String message = "<h1>RiksaLert Riksan hälytysjärjestelmä</h1>";
  message += "<br>";
  message += "Riksa on häiriöttömässä moodissa vielä: ";
  message += _min(dndUntil, dndUntil - currentMillis);
  message += " ajanhetkeä";
  message += "<br>";
  message += "<button><a href=\"/disturb\">hälert!</button>";
  message += "<br>";
  if ( ! isDnDActive()) {
    Serial.println("Start halarm");
    startDisturb();
  } else {
    Serial.println("Prevent halarm");
  }
  server.send(200, "text/html", message);
}

void setup(void){
  Serial.begin(115200);
  Serial.println("BEGIN");

  setupDisturb();
  setupDnd();

  WiFi.begin(ssid, password);
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

  server.on("/disturb", handleDisturb);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  handleDoNotDisturb();
  if (isDnDActive()) {
    stopDisturb();
  } else {
    tickDisturb();
  }
}

