#include "udp.h"
#include <WifiUdp.h>            // for the UDP server - discovery feature
#include "debug.h"

WiFiUDP udp;
const int udpPort = 2705;
char discoveryMessage[] = "NadControler Discovery";

void UdpSetup() {
  udp.begin(udpPort);
  DEBUGLOG("UDP server started");
}

void UdpLoop() {
  // handle udp messages
  int udppacketsize = udp.parsePacket();
  if (udppacketsize) {
    DEBUGLOG("Recevied UDP packet of size ");
    DEBUGLOG(udppacketsize);
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(discoveryMessage);
    udp.endPacket();
  } // if
}

