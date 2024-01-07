#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Globals.h"

#ifndef LORATRANS_H
#define LORATRANS_H
class LoRaTrans {
  public:
    LoRaTrans();
    bool hasMessage ();
    uint8_t retrieveMessage();
    uint8_t* getMessageBuffer ();
    bool broadcast (String message);
    bool broadcast(uint8_t *message, uint8_t length);
    bool send (String message, int address);
    bool send(uint8_t *message, uint8_t length, uint8_t address);
    int getLastRssi ();

  private:
    void reset ();
    RH_RF95* rfm9x;
    RHReliableDatagram* rfm9x_manager;
    uint8_t buf[LORA_RFM9X_MAX_MESSAGE_LEN];
};
#endif