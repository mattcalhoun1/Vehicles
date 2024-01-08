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
    long retrieveMessage();
    uint8_t* getMessageBuffer ();
    uint8_t* getChunkInBuffer ();
    int getChunkInBufferSize();
    bool broadcast (String message);
    bool broadcast(uint8_t *message, uint8_t length);
    bool send (String message, int address);
    bool send(uint8_t *message, int length, uint8_t address);
    int getLastRssi ();
    unsigned long getChunkInBufferTime ();
    unsigned long getMessageBufferTime ();

  private:
    void reset ();
    RH_RF95* rfm9x;
    RHReliableDatagram* rfm9x_manager;
    uint8_t buf[LORA_RFM9X_MAX_MESSAGE_LEN];
    uint8_t chunkInBuffer[LORA_MAX_CHUNK_IN_BUFFER_SIZE];
    int chunkInBufferSize = 0; // how many received bytes are in the buffer
    void logConsole(String msg);
    void logConsole(String msg, bool newline);
    void logConsole(int msg, bool newline, int base);
    bool endChunking(int address, unsigned long checksum);
    bool beginChunking(int address);
    void appendBufferToChunked (int chunkSize);
    long extractBufferChecksum ();
    long calculateChunkInBufferChecksum ();
    unsigned long chunkInBufferTime = 0;
    unsigned long messageBufferTime = 0;
};
#endif