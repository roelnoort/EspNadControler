#include "config.h"
#include <ArduinoJson.h>        // json - to store configuration
#include "FS.h"                 // filesystem - to store configuration
#include "debug.h"

bool loadSettings(char*ssid, char*password) {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    DEBUGLOG("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    DEBUGLOG("Config file size is too large");
    configFile.close();
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);
  configFile.close();
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    DEBUGLOG("Failed to parse config file");
    return false;
  }

  const char* myssid = json["ssid"];
  const char* mypwd = json["pwd"];
  ssid = new char[strlen(myssid)+1];
  password = new char[strlen(mypwd)+1];
  strcpy(ssid, myssid);
  strcpy(password, mypwd);

  DEBUGLOG("Loaded ssid: ");
  DEBUGLOG(ssid);
  DEBUGLOG(strlen(ssid));
  DEBUGLOG("Loaded pwd: ");
  DEBUGLOG(password);
  
  return (strlen(ssid) > 0);
}

bool storeSettings(String ssid, String password) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ssid"] = ssid;
  json["pwd"] = password;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    DEBUGLOG("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  configFile.close();
  return true;
}
