#include <ESP8266mDNS.h>
#include "FS.h"                 // filesystem - to store configuration
#include "nad.h"
#include "config.h"
#include "udp.h"
#include "apconfig.h"
#include "webcontroler.h"
#include "debug.h"

char* ssid;
char* password;

const int led = 13;

enum OperationModes {bootup, configuration, operational};
OperationModes CurrentMode = bootup;

void SetupServerOrAP() {
  if (loadSettings(ssid, password)) {
    WiFi.begin(ssid, password);

    // Wait for connection, with timeout on i
    int i = 0;
    while ((WiFi.status() != WL_CONNECTED) && (i < 50)) {
      delay(500);
      i++;
      DEBUGLOG(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      CurrentMode = operational;
      DEBUGLOG("Connected to WiFi --> operational mode");
      DEBUGLOG("Connected to ");
      DEBUGLOG(WiFi.SSID());
      DEBUGLOG("IP address: ");
      DEBUGLOG(WiFi.localIP());
      WebControlerSetup();
    }
    else {
      CurrentMode = configuration;
      DEBUGLOG("*NOT* Connected to WiFi --> configuration mode");
      AccessPointSetup;
    }
  }
  else {
    CurrentMode = configuration;
    DEBUGLOG("settings cannot be loaded --> configuration mode");
    AccessPointSetup();
  }
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    DEBUGLOG("Failed to mount file system");
    return;
  } else {
    DEBUGLOG("File system started");
  }
 
  if (MDNS.begin("nadcontroler")) {
    DEBUGLOG("MDNS responder started");
  }

  UdpSetup();
}

void loop(void){

  switch (CurrentMode) {
  case bootup:
    SetupServerOrAP();
  break;
  case configuration:
    AccessPointLoop();
  break;
  case operational:
    WebControlerLoop();
  break;
  }
  
  UdpLoop();
  
}
