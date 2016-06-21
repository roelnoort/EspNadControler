#include <ESP8266mDNS.h>
#include "FS.h"                 // filesystem - to store configuration
#include "nad.h"
#include "config.h"
#include "udp.h"
#include "webcontroler.h"

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
    Serial.println("Failed to mount file system");
    return;
  } else {
    Serial.println("File system started");

    //storeSettings();
    loadSettings(ssid, password);
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

  WebControlerSetup();

  UdpSetup();
}

void loop(void){
  WebControlerLoop();

  UdpLoop();
  
}
