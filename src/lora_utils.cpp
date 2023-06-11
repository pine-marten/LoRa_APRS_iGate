#include <LoRa.h>
#include "configuration.h"
#include "pins_config.h"
#include "display.h"

extern Configuration  Config;
extern int            stationMode;

namespace LoRa_Utils {

void setup() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
  long freq;
  if (stationMode == 1 || stationMode == 2) {
    freq = Config.loramodule.iGateFreq;
  } else {
    freq = Config.loramodule.digirepeaterTxFreq;
  }
  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");
    show_display("ERROR", "Starting LoRa failed!");
    while (true) {
      delay(1000);
    }
  }
  LoRa.setSpreadingFactor(Config.loramodule.spreadingFactor);
  LoRa.setSignalBandwidth(Config.loramodule.signalBandwidth);
  LoRa.setCodingRate4(Config.loramodule.codingRate4);
  LoRa.enableCrc();
  LoRa.setTxPower(Config.loramodule.power);
  Serial.println("LoRa init done!");  
}

void sendNewPacket(const String &typeOfMessage, const String &newPacket) {
  digitalWrite(greenLed,HIGH);
  LoRa.beginPacket();
  LoRa.write('<');
  if (typeOfMessage == "APRS")  {
    LoRa.write(0xFF);
  } else if (typeOfMessage == "LoRa") {
    LoRa.write(0xF8);
  }
  LoRa.write(0x01);
  LoRa.write((const uint8_t *)newPacket.c_str(), newPacket.length());
  LoRa.endPacket();
  digitalWrite(greenLed,LOW);
  Serial.print("---> LoRa Packet Tx    : ");
  Serial.println(newPacket);
}

String generatePacket(String aprsisPacket) {
  String firstPart, messagePart;
  aprsisPacket.trim();
  firstPart = aprsisPacket.substring(0, aprsisPacket.indexOf(","));
  messagePart = aprsisPacket.substring(aprsisPacket.indexOf("::")+2);
  return firstPart + ",TCPIP," + Config.callsign + "::" + messagePart;
}

String receivePacket() {
  String loraPacket = "";
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      int inChar = LoRa.read();
      loraPacket += (char)inChar;
    }
  }
  return loraPacket;
}

void changeFreqTx() {
  delay(500);
  LoRa.setFrequency(Config.loramodule.digirepeaterTxFreq);
}

void changeFreqRx() {
  delay(500);
  LoRa.setFrequency(Config.loramodule.digirepeaterRxFreq);
}

}