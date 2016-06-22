#include <ESP8266mDNS.h>
#include "FS.h"                 // filesystem - to store configuration
#include "nad.h"
#include "config.h"
#include "udp.h"
#include "webcontroler.h"
#include "debug.h"

//const char* ssid = "burton2G";
//const char* password = "customflyingv";

char* ssid;
char* password;

const int led = 13;



void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    DEBUGLOG("Failed to mount file system");
    return;
  } else {
    DEBUGLOG("File system started");

    //storeSettings();
    loadSettings(ssid, password);
  }
  
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUGLOG(".");
  }
  
  DEBUGLOG("Connected to ");
  DEBUGLOG(ssid);
  DEBUGLOG("IP address: ");
  DEBUGLOG(WiFi.localIP());

  if (MDNS.begin("nadcontroler")) {
    DEBUGLOG("MDNS responder started");
  }

  WebControlerSetup();

  UdpSetup();
}

void loop(void){
  WebControlerLoop();

  UdpLoop();
  
}
