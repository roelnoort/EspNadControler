#include <ESP8266mDNS.h>
#include "FS.h"                 // filesystem - to store configuration
#include "nad.h"
#include "config.h"
#include "udp.h"
#include "apconfig.h"
#include "webcontroler.h"
#include "debug.h"
#include "states.h"

char* ssid;
char* password;

const int led = 13;


void SetupServerOrAP() {
  if (loadSettings(ssid, password)) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(ssid, password);

    // Wait for connection, with timeout on i
    int i = 0;
    while ((WiFi.status() != WL_CONNECTED) && (i < 50)) {
      if (WiFi.status() == WL_CONNECTED) DEBUGLOG("connected");
      if (WiFi.status() == WL_NO_SHIELD) DEBUGLOG("no shield");
      if (WiFi.status() == WL_IDLE_STATUS) DEBUGLOG("IDLE");
      if (WiFi.status() == WL_NO_SSID_AVAIL) DEBUGLOG("NO SSID AVAIL");
      if (WiFi.status() == WL_SCAN_COMPLETED) DEBUGLOG("SCAN COMPLETE");
      if (WiFi.status() == WL_CONNECT_FAILED) DEBUGLOG("CONNECT FAILED");
      if (WiFi.status() == WL_CONNECTION_LOST) DEBUGLOG("CONNECTION LOST");
      if (WiFi.status() == WL_DISCONNECTED) DEBUGLOG("DISCONNECTED");
      
      delay(500);
      i++;
      DEBUGLOG(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      SetState(operational);
      DEBUGLOG("Connected to WiFi --> operational mode");
      DEBUGLOG("Connected to ");
      DEBUGLOG(WiFi.SSID());
      DEBUGLOG("IP address: ");
      DEBUGLOG(WiFi.localIP());
      WebControlerSetup();
      if (MDNS.begin("nadcontroler")) {
        DEBUGLOG("MDNS responder started");
      }
    }
    else {
      SetState(configuration);
      DEBUGLOG("*NOT* Connected to WiFi --> configuration mode");
      AccessPointSetup();
    }
  }
  else {
    SetState(configuration);
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

  UdpSetup();
}

void loop(void){

  switch (GetState()) {
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
