#include "udp.h"
#include <WifiUdp.h>            // for the UDP server - discovery feature

WiFiUDP udp;
const int udpPort = 2705;
char discoveryMessage[] = "NadControler Discovery";

void UdpSetup() {
  udp.begin(udpPort);
  Serial.println("UDP server started");
}

void UdpLoop() {
  // handle udp messages
  int udppacketsize = udp.parsePacket();
  if (udppacketsize) {
    Serial.print("Recevied UDP packet of size ");
    Serial.println(udppacketsize);
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(discoveryMessage);
    udp.endPacket();
  } // if
}

