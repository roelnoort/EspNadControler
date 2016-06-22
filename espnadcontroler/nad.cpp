#include "nad.h"
#include "debug.h"

bool NadSend(String command) {
  bool success = false;
  
  Serial.print("\r");
  Serial.print(command);
  Serial.print("\r");  

  // all commands should get a reply.
  // wait max 5 seconds for this
  int i = 0;
  while ((Serial.available() == 0) && (i < 50)) {
    delay(100); // wait for 100 ms
    i++;
  }
  
  // And read all data that the receiver sends back to us.
  String s;
  while (Serial.available() > 0) {
    s = Serial.readString();
    DEBUGLOG("I READ=" + s);
    success = true;
  }

  return success;
}
