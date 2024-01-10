#include <Arduino.h>
#include "LoRaTrans.h"
#include "Commander.h"
#include "Globals.h"

#ifndef LORACOMMANDER_H
#define LORACOMMANDER_H
#define LORA_COMMANDER_PING_FREQUENCY 30000 // millis between pings

class LoRaCommander : public Commander {
  public:
    LoRaCommander(LoRaTrans* _lora, int _commanderAddress) {lora = _lora; commanderAddress = _commanderAddress;}
    void init ();
    bool isConnected();
    unsigned long lastConnected();
    Command getNextCommand ();
    Command extractCommand (String rawCommand, int sender);
    void sendResult (Command command, CommandResult result);
    void sendReceipt (Command command, ReceiptResult result);
    void sendCommand (Command command);
    void sendPing();

  private:
    unsigned long lastResponse = 0;
    Command receiveCommand ();
    unsigned long lastPing = 0;
    LoRaTrans* lora;
    int commanderAddress;
    bool isCompleteCommand(String msg);

    const char delimiterCmd = ':';
    const char delimiterId = '_';
    const char delimiterParam = '|';

    long sentCount = 0;
};
#endif