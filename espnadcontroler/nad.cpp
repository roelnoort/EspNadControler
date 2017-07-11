#include "nad.h"
#include "debug.h"

bool ClearSerialBuffer() {
  bool dataRead = false;

  String s;
  while (Serial.available() > 0) {
    s = Serial.readString();
    DEBUGLOG("I READ=" + s);
    dataRead = true;
  }

  return dataRead;
}

bool NadSend(String command) {
  bool success = false;

  // make sure there is no incoming serial data
  // we will get a reply directly after our command
  // and we don't want to threat data received before our
  // command as the reply.
  ClearSerialBuffer();

  // Send the command
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
  success = ClearSerialBuffer();

  return success;
}

String NadCommandReply(String command) {
  String reply = "";

  // make sure there is no incoming serial data
  // we will get a reply directly after our command
  // and we don't want to threat data received before our
  // command as the reply.
  ClearSerialBuffer();

  // Send the command
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
  while (Serial.available() > 0) {
    reply += Serial.readString();
    DEBUGLOG("REPLY READ=" + reply);
  }

  reply.replace("\r", "");

  return reply;
}
