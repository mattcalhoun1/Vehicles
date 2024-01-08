#include "LoRaTrans.h"

LoRaTrans::LoRaTrans () {
  logConsole("rfm9x...");
  rfm9x = new RH_RF95(LORA_RFM9X_CS, LORA_RFM9X_INT);
  delay(500);
  logConsole("rfm9x ok");
  logConsole("reliability manager...");
  rfm9x_manager = new RHReliableDatagram(*rfm9x, LORA_ADDR_SELF);
  logConsole("reliability manager ok");

  logConsole("reset output pinmode");
  pinMode(LORA_RFM9X_RST, OUTPUT);
  logConsole("reset output pinmode ok");

  logConsole("reset low");
  digitalWrite(LORA_RFM9X_RST, HIGH);
  logConsole("reset low ok");

  reset();

  /*if (!rfm9x->init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }*/


  if (!rfm9x_manager->init()) {
    logConsole("RFM9X radio init failed");
    while (1);
  }
  logConsole("RFM9X radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rfm9x->setFrequency(LORA_RFM9X_FREQ)) {
    logConsole("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rfm9x->setTxPower(14, true);

  logConsole("RFM9X radio @", false);  logConsole(String(LORA_RFM9X_FREQ), false);  logConsole(" MHz");

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
    logConsole("Broadcasting ", false); logConsole(String(length), false); logConsole(" to ", false); logConsole(String(LORA_ADDR_BROADCAST));
    return rfm9x_manager->sendtoWait(message, length, LORA_ADDR_BROADCAST);
}

bool LoRaTrans::send(String message, int address) {
  return send((uint8_t*)message.c_str(), message.length(), address);
}

int LoRaTrans::getLastRssi() {
  return rfm9x->lastRssi();
}

bool LoRaTrans::send(uint8_t *message, int length, uint8_t address) {

  logConsole("Sending ", false); logConsole(String(length), false); logConsole(" to ", false); logConsole(String(address));

  // if necessary break apart the message into multiples
  int sent = 0;
  int retries = 0;
  int maxRetries = 2;
  int chunkSize = length <= LORA_MAX_CHUNK_SIZE ? length : LORA_MAX_CHUNK_SIZE;
  bool chunking = length > LORA_MAX_CHUNK_SIZE;
  bool chunkNoticeSent = false;

  while (sent < length and retries <= maxRetries) {
    // If chunking, send that notice first
    if (chunking && !chunkNoticeSent) {
      chunkNoticeSent = beginChunking(address);
      if (!chunkNoticeSent) {
        retries++;
      }
    }
    else {
      // if this is the last chunk, the chunk size might be smaller
      if (length - sent < chunkSize) {
        chunkSize = length - sent;
      }

      if (rfm9x_manager->sendtoWait(message + sent, chunkSize, address)) {
        // Now wait for a reply from the server
        uint8_t len = sizeof(buf);
        uint8_t from;
        if (rfm9x_manager->recvfromAckTimeout(buf, &len, 2000, &from)) {
          buf[len] = 0; // terminate the buffer
          logConsole("Got reply from #", false);
          logConsole(from, false, DEC); logConsole(": ", false);
          logConsole((char*)buf);

          // on to the next chunk
          sent += chunkSize;
          logConsole("Sent ", false); logConsole(sent, false, DEC); logConsole(" of ", false); logConsole(length, true, DEC);
        }
        else {
          logConsole("Acknowledgement not received!");
          retries += 1;
        }
      }
      else {
        logConsole("Message not sent!");
        retries += 1;
      }
    }
  }

  bool footerSuccess = true;
  if (sent >= length) {
    logConsole("Sent all " + String(sent) + " bytes successfully");

    // send the chunk end notice with checksum if chunking
    if (chunking) {
      unsigned long checksum = 0;
      for (int i = 0; i < length; i++) {
        checksum += message[i];
      }
      if (!endChunking(address, checksum)) {
        logConsole("Chunk end notice failed!");
      }
    }
  }
  else {
    logConsole("Send failed");
  }

  return sent >= length && footerSuccess;
}


bool LoRaTrans::beginChunking(int address) {
  return send("CHUNKSTART", address);
}

bool LoRaTrans::endChunking(int address, unsigned long checksum) {
  return send("CHUNKEND:" + String(checksum), address);
}

uint8_t LoRaTrans::retrieveMessage() {
  uint8_t data[] = "  OK";

  // Wait for a message addressed to us from the client
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (rfm9x_manager->recvfromAck(buf, &len, &from))
  {
    buf[len] = 0;
    logConsole("got request from : 0x", false);
    logConsole(from, false, HEX);
    logConsole(": ", false);
    logConsole((char*)buf);
    logConsole("RSSI: ", false); logConsole(rfm9x->lastRssi(), true, DEC);

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
  logConsole("Resetting LoRa...");
  digitalWrite(LORA_RFM9X_RST, LOW);
  delay(10);
  digitalWrite(LORA_RFM9X_RST, HIGH);
  delay(10);
  logConsole("LoRa successfully reset.");
}

void LoRaTrans::logConsole (String msg, bool newline) {
  if (LOG_ENABLED) {
    if (newline) {
      Serial.println(msg);
    }
    else {
      Serial.print(msg);
    }
  }
}

void LoRaTrans::logConsole (int msg, bool newline, int base) {
  if (LOG_ENABLED) {
    if (newline) {
      Serial.println(msg, base);
    }
    else {
      Serial.print(msg, base);
    }
  }
}

void LoRaTrans::logConsole (String msg) {
  logConsole(msg, true);
}