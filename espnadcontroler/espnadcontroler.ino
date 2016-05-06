#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>   // http server
#include <ESP8266mDNS.h>
#include <WifiUdp.h>            // for the UDP server - discovery feature
#include <ArduinoJson.h>        // json - to store configuration
#include "FS.h"                 // filesystem - to store configuration

//const char* ssid = "burton2G";
//const char* password = "customflyingv";

char* ssid;
char* password;

ESP8266WebServer server(80);
WiFiUDP udp;

const int led = 13;
const int udpPort = 2705;
char discoveryMessage[] = "NadControler Discovery";

String htmlStyleTag = "<style>\
.button {\
    background-color: #4CAF50;\
    border: none;\
    color: white;\
    padding: 15px 32px;\
    text-align: center;\
    text-decoration: none;\
    display: inline-block;\
    font-size: 16px;\
    margin: 4px 2px;\
    cursor: pointer;\
}\
</style>";


void handleRoot() {
  digitalWrite(led, 1);

  String message = "";

  message += "<!DOCTYPE HTML>\n";
  message += "<html><head>";
  message += htmlStyleTag;
  message += "<title>NAD Controler Info</title>";
  message += "</head><body>\n";
  message += "<h1>NAD T758 controler</h1>\n";
  message += "<p>This is the RS232 controler for the NAD T758.</p>\n";
  message += "<p><a href='switchinput/6' class='button'>SONOS</a></p>\n";
  message += "<p><a href='switchinput/2' class='button'>AppleTV</a></p>\n";
  message += "<p><a href='switchinput/1' class='button'>Mediacenter</a></p>\n";
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

bool loadSettings() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* myssid = json["ssid"];
  const char* mypwd = json["pwd"];
  ssid = new char[strlen(myssid)+1];
  password = new char[strlen(mypwd)+1];
  strcpy(ssid, myssid);
  strcpy(password, mypwd);
  //String s(myssid);
  //ssid = json["ssid"];
  //password = json["pwd"];
  
  // Real world application would store these values in some variables for
  // later use.

  Serial.print("Loaded ssid: ");
  Serial.println(ssid);
  Serial.print("Loaded pwd: ");
  Serial.println(password);
  return true;
}

bool storeSettings() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ssid"] = "burton2G";
  json["pwd"] = "customflyingv";

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  } else {
    Serial.println("File system started");

    //storeSettings();
    loadSettings();
  }
  
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

  udp.begin(udpPort);
  Serial.println("UDP server started");
}

void loop(void){
  // handle http messages
  server.handleClient();

  // handle udp messages
  int udppacketsize = udp.parsePacket();
  if (udppacketsize) {
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(discoveryMessage);
    udp.endPacket();
  } // if
  
}
