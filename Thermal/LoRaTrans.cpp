#include "LoRaTrans.h"

LoRaTrans::LoRaTrans () {
  Serial.println("rfm9x...");
  rfm9x = new RH_RF95(LORA_RFM9X_CS, LORA_RFM9X_INT);
  delay(500);
  Serial.println("rfm9x ok");
  Serial.println("reliability manager...");
  rfm9x_manager = new RHReliableDatagram(*rfm9x, LORA_ADDR_SELF);
  Serial.println("reliability manager ok");

  Serial.println("reset output pinmode");
  pinMode(LORA_RFM9X_RST, OUTPUT);
  Serial.println("reset output pinmode ok");

  Serial.println("reset low");
  digitalWrite(LORA_RFM9X_RST, HIGH);
  Serial.println("reset low ok");

  reset();

  /*if (!rfm9x->init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }*/


  if (!rfm9x_manager->init()) {
    Serial.println("RFM9X radio init failed");
    while (1);
  }
  Serial.println("RFM9X radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rfm9x->setFrequency(LORA_RFM9X_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rfm9x->setTxPower(14, true);

  Serial.print("RFM9X radio @");  Serial.print((int)LORA_RFM9X_FREQ);  Serial.println(" MHz");

}

bool LoRaTrans::hasMessage() {
   return rfm9x_manager->available();
}

uint8_t* LoRaTrans::getMessageBuffer () {
  return buf;
}

bool LoRaTrans::broadcast(String message) {
  return broadcast((uint8_t*)message.c_str(), message.length());
}

bool LoRaTrans::broadcast(uint8_t *message, uint8_t length) {
    Serial.print("Broadcasting "); Serial.print(length); Serial.print(" to "); Serial.println(LORA_ADDR_BROADCAST);
    return rfm9x_manager->sendtoWait(message, length, LORA_ADDR_BROADCAST);
}

bool LoRaTrans::send(String message, int address) {
  return send((uint8_t*)message.c_str(), message.length(), address);
}

int LoRaTrans::getLastRssi() {
  return rfm9x->lastRssi();
}

bool LoRaTrans::send(uint8_t *message, uint8_t length, uint8_t address) {

    Serial.print("Sending "); Serial.print(length); Serial.print(" to "); Serial.println(address);

    if (rfm9x_manager->sendtoWait(message, length, address)) {
      // Now wait for a reply from the server
      uint8_t len = sizeof(buf);
      uint8_t from;
      if (rfm9x_manager->recvfromAckTimeout(buf, &len, 2000, &from)) {
        buf[len] = 0; // terminate the buffer
        Serial.print("Got reply from #");
        Serial.print(from); Serial.print(": ");
        Serial.println((char*)buf);
        return true;
      } else {
        Serial.println("No reply, is anyone listening?");
        return false;
      }
    } else {
      Serial.println("sendtoWait failed");
      return false;
    }

  return false;
}

uint8_t LoRaTrans::retrieveMessage() {
  uint8_t data[] = "  OK";

  // Wait for a message addressed to us from the client
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (rfm9x_manager->recvfromAck(buf, &len, &from))
  {
    buf[len] = 0;
    Serial.print("got request from : 0x");
    Serial.print(from, HEX);
    Serial.print(": ");
    Serial.println((char*)buf);
    Serial.print("RSSI: "); Serial.println(rfm9x->lastRssi(), DEC);

    // echo last button
    data[0] = buf[8];
    // Send a reply back to the originator client
    if (!rfm9x_manager->sendtoWait(data, sizeof(data), from)) {
      Serial.println("sendtoWait failed");
      return 0;
    }

    return len;
  }

  return 0;
}

void LoRaTrans::reset () {
    // manual reset
  Serial.println("Resetting LoRa...");
  digitalWrite(LORA_RFM9X_RST, LOW);
  delay(10);
  digitalWrite(LORA_RFM9X_RST, HIGH);
  delay(10);
  Serial.println("LoRa successfully reset.");

}