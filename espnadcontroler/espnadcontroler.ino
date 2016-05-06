#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "burton2G";
const char* password = "customflyingv";

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);

  String message = "";

  message += "<!DOCTYPE HTML>\n";
  message += "<html><head><title>NAD Controler Info</title></head><body>\n";
  message += "<h1>NAD T758 controler</h1>\n";
  message += "<p>This is the RS232 controler for the NAD T758.</p>\n";
  message += "<p><a href='switchinput/6'>SONOS</a></p>\n";
  message += "<p>Use HTTP POST via </p>\n";
  message += "<p>http://&lt;hostname&gt;:&lt;port&gt;/switchinput/&lt;source number&gt;</p>\n";
  message += "<p>http://&lt;hostname&gt;:&lt;port&gt;/poweroff</p>\n";
  message += "</body></html>\n";
  
  server.send(200, "text/html", message);
  digitalWrite(led, 0);
}

void handleSwitchInput() {
  digitalWrite(led, 1);

  String message = "";
  int i = server.uri().lastIndexOf("/");
  int input = server.uri().substring(i+1).toInt();

  if (server.method() == HTTP_GET) {
    message += "<!DOCTYPE HTML>\n";
    message += "<html><head><title>Switch input</title></head><body>\n";
    message += "<h1>Switch input</h1>";
    message += "<p>Trying to switch input to "+ String(input) + "</p>";
    message += "</body></html>\n";
  }
  
  server.send(200, "text/html", message);
  digitalWrite(led, 0);
}

void handlePowerOff() {
  digitalWrite(led, 1);

  String message = "";

  if (server.method() == HTTP_GET) {
    message += "<!DOCTYPE HTML>\n";
    message += "<html><head><title>Power off amp</title></head><body>\n";
    message += "<h1>Powering off</h1>";
    message += "<p>Trying to power off the amplifier</p>";
    message += "</body></html>\n";
  }
  
  server.send(200, "text/html", message);
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

void setup(void){
  pinMode(led, OUTPUT);
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

  server.on("/switchinput/1", handleSwitchInput);
  server.on("/switchinput/2", handleSwitchInput);
  server.on("/switchinput/3", handleSwitchInput);
  server.on("/switchinput/4", handleSwitchInput);
  server.on("/switchinput/5", handleSwitchInput);
  server.on("/switchinput/6", handleSwitchInput);
  server.on("/switchinput/7", handleSwitchInput);
  server.on("/switchinput/8", handleSwitchInput);
  server.on("/switchinput/9", handleSwitchInput);

  server.on("/poweroff", handlePowerOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
