#include "webcontroler.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>   // http server
#include "nad.h"
#include "debug.h"
#include "states.h"

ESP8266WebServer server(80);

const int led = 13;


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
message += "<title>NAD Controler Info</title>";
message += "<meta charset='utf-8'>";
message += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
message += "<link rel='stylesheet' href='http://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css'>";

message += "<style>";
message += "[class*=col-] {margin-top:20px;}";
message += "</style>";
message += "<script src='https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js'></script>";
message += "<script src='http://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js'></script>";
message += "</head><body>\n";

message += "<div class='container'>\n";

message += "<div class='row'>\n";

message += "<div class='col-xs-6'>\n";
message += "<button type='button' id='sonos' class='btn btn-default btn-block btn-lg'>SONOS</button>\n";
message += "</div>\n";

message += "<div class='col-xs-6'>\n";
message += "<button type='button' id='appletv' class='btn btn-default btn-block btn-lg'>AppleTV</button>\n";
message += "</div>\n";

message += "<div class='col-xs-6'>\n";
message += "<button type='button' id='xbox' class='btn btn-default btn-block btn-lg'>XBox</button>\n";
message += "</div>\n";

message += "<div class='col-xs-6'>\n";
message += "<button type='button' id='chromecast' class='btn btn-default btn-block btn-lg'>Chromecast</button>\n";
message += "</div>\n";

message += "<div class='col-xs-6'>\n";
message += "<button type='button' id='dvd' class='btn btn-default btn-block btn-lg'>DVD</button>\n";
message += "</div>\n";

message += "<div class='col-xs-6'>\n";
message += "<button type='button' id='mediacenter' class='btn btn-default btn-block btn-lg'>Mediacenter</button>\n";
message += "</div>\n";

message += "</div>\n"; // row

message += "<div class='row'>\n";

message += "<div class='col-xs-6 col-xs-offset-3'>\n";
message += "<button href='poweroff' type='button' id='off' class='btn btn-default btn-block btn-lg'>Off</button>\n";
message += "</div>\n";

message += "</div>\n"; // row

message += "</div>\n";

message += "<script>";
message += "$('#sonos').click(function() {";
message += "$.post('/switchinput/6');";
message += "});";

message += "$('#appletv').click(function() {";
message += "$.post('/switchinput/2');";
message += "});";

message += "$('#xbox').click(function() {";
message += "$.post('/switchinput/4');";
message += "});";

message += "$('#chromecast').click(function() {";
message += "$.post('/switchinput/3');";
message += "});";

message += "$('#dvd').click(function() {";
message += "$.post('/switchinput/7');";
message += "});";

message += "$('#mediacenter').click(function() {";
message += "$.post('/switchinput/1');";
message += "});";

message += "$('#off').click(function() {";
message += "$.post('/poweroff');";
message += "});";

message += "</script>";

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

  if (NadSend("Main.Power=on")) {
    if (NadSend("Main.Source=" + String(input))) {
      server.send(200, "text/html", message);
    }
    else {
      // send server error 500 - internal server error
      server.send(500, "text/html", message);
    }
  } 
  else {
    // send server error 500 - internal server error
    server.send(500, "text/html", message);
  }
  
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

  if (NadSend("Main.Power=off")) {
    server.send(200, "text/html", message);
  }
  else {
    // send server error 500 - internal server error
    server.send(500, "text/html", message);
  }
  
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

void WebControlerSetup() {
  //WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  
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
  DEBUGLOG("HTTP server started");
}

void WebControlerLoop() {
  // handle http messages
  server.handleClient();
}

