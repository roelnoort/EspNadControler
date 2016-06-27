#include "apconfig.h"
#include "Arduino.h"
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "debug.h"
#include "config.h"
#include "states.h"

ESP8266WebServer ap(80);

void handleApRoot() {
  String message = "";
  message += "<!DOCTYPE HTML>\n";
  message += "<html><head><title>Setup NAD Controler</title></head><body>\n";
  message += "<h1>Connect your Wifi</h1>"; 

  int n = WiFi.scanNetworks();
  if (n == 0) {
    message += "no networks found <br>";
  }
  else {
    message += String(n) + " networks found <br>";
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      message += String(i) + " : ";
      message += WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") ";
      message += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      message += "<br>";
      delay(10);
    }
  }
  
  message += "<form action='storesettings' method='post'>";
  message += "Network name (ssid): <br>";
  message += "<input type='text' name='ssid'> <br>";
  message += "Password: <br>";
  message += "<input type='text' name='password'> <br><br>";
  message += "<input type='submit' name='Save'> <br>";
  message += "</form>";
  message += "</body></html>\n";
    
  ap.send(200, "text/html", message);
}

void handleApStoreSettings() {
  String message = "";

  String ssid = ap.arg("ssid");
  String pwd = ap.arg("password");

  for (int i = 0; i < ap.args(); i++) {
    DEBUGLOG(ap.argName(i) + ": " + ap.arg(i));
  } 
  
  DEBUGLOG("user saved network settings");
  DEBUGLOG(ssid);
  DEBUGLOG(pwd);
  
  message += "<!DOCTYPE HTML>\n";
  message += "<html><head><title>Saving your settings</title></head><body>\n";
  message += "<h1>Settings saved</h1>"; 
  message += "SSID : " + ssid + "<br>";
  message += "Password : " + pwd + "<br>";

  if (storeSettings(ssid, pwd)) {
    message += "SETTINGS STORED SUCCESFULLY <br>";
    SetState(bootup);
  }
  else {
    message += "ERROR SAVING SETTINGS <br>";
  }


  for (int i = 0; i < ap.args(); i++) {
    message += " " + ap.argName(i) + ": " + ap.arg(i) + "\n";
  } 
    
  message += "</body></html>\n";
    
  ap.send(200, "text/html", message);
}

void AccessPointSetup() {
  // The access point ssid is 'nadcontroler'.
  // We can access the website via http://192.168.4.1 once we are connected to the AP's wifi
  // For some reason http://nadcontroler does not seem to work in AP mode.
  WiFi.mode(WIFI_AP);
  WiFi.disconnect();
  WiFi.softAP("nadcontroler", "test");
  ap.on("/", handleApRoot);
  ap.on("/storesettings", handleApStoreSettings);
  ap.begin();
}

void AccessPointLoop() {
  ap.handleClient();
}
