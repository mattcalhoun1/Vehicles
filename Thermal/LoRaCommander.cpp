#include "LoRaCommander.h"

void LoRaCommander::init() {
  logConsole("Commander listening on LoRa");
}

Command LoRaCommander::getNextCommand () {
  return receiveCommand ();
}

unsigned long LoRaCommander::lastConnected() {
  return lastResponse;
}

bool LoRaCommander::isConnected() {
  return (lastResponse > 0);
}

bool LoRaCommander::isCompleteCommand(String msg) {
  // should contain similar to:
  // COMMAND#id:str
  // COMMAND#id:str|num

  int delim = msg.indexOf(delimiterCmd);
  int idDelim = msg.indexOf(delimiterId);

  if (delim == -1 || idDelim == -1 || idDelim > delim) {
    return false;
  }

  int pipeCount = 0;
  String cmd = msg.substring(0, idDelim);
  if (cmd.length() <= 16) {
    for (int c = delim+1; c < msg.length(); c++) {
      if (msg.charAt(c) == delimiterCmd) {
        return false;
      }
      else if (msg.charAt(c) == delimiterParam) {
        pipeCount++;
      }
    }     
  }

  if (cmd.equalsIgnoreCase(CMD_PING) && pipeCount == 0) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_GET_THERMAL) && pipeCount == 0) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_CAMERA_PAN) && pipeCount == 1) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_CAMERA_TILT) && pipeCount == 1) {
    return true;
  }
  else if (cmd.equalsIgnoreCase(CMD_DISPLAY)) {
    return true;
  }
  return false;
}


Command LoRaCommander::receiveCommand () {
  // If there is room in the queue, check for another command
  if (lora->hasMessage()) {
    lastResponse = millis();

    int messageLength = lora->retrieveMessage();
    String command = String((char*)lora->getMessageBuffer());
    int sender = lora->getLastSender();

    if (isCompleteCommand(command)) {
      String params = "";
      long id = 0;
      int delim = command.indexOf(delimiterCmd);

      if (delim != -1) {
        params = command.substring(delim+1);
        params.trim();
        id = atol(command.substring(command.indexOf(delimiterId)+1, command.indexOf(delimiterCmd)).c_str());
        command = command.substring(0, command.indexOf(delimiterId));
      }

      if (command.equalsIgnoreCase(CMD_PING)) {
        Command cmd;
        cmd.setCommandType(Ping);
        cmd.setParams(params);
        cmd.setId(id);
        cmd.setSender(sender);
        return cmd;
      }
      else if (command.equalsIgnoreCase(CMD_GET_THERMAL)) {
        Command cmd;
        cmd.setCommandType(GetThermal);
        cmd.setParams(params);
        cmd.setId(id);
        cmd.setSender(sender);
        return cmd;
      }
      else if (command.equalsIgnoreCase(CMD_CAMERA_PAN)) {
        Command cmd;
        cmd.setCommandType(CameraPan);
        cmd.setParams(params);
        cmd.setId(id);
        cmd.setSender(sender);
        return cmd;
      }
      else if (command.equalsIgnoreCase(CMD_CAMERA_TILT)) {
        Command cmd;
        cmd.setCommandType(CameraTilt);
        cmd.setParams(params);
        cmd.setId(id);
        cmd.setSender(sender);
        return cmd;
      }
      else if (command.equalsIgnoreCase(CMD_DISPLAY)) {
        Command cmd;
        cmd.setCommandType(DisplayText);
        cmd.setParams(params);
        cmd.setId(id);
        cmd.setSender(sender);
        return cmd;
      }
      else {
        logConsole("Unknown command " + command);
      }
    }
    else {
      logConsole("Received invalid formatted command " + command);
    }
  }

  // Notify we are ready
  //sendPing();

  Command nullCommand;
  nullCommand.setCommandType(Nothing);
  return nullCommand;
}

void LoRaCommander::sendReceipt (Command command, ReceiptResult result) {
  String receiptMsg = String(CMD_RECEIPT + String(delimiterId) + String(command.getId()) + String(delimiterCmd) + String(result));
  lora->send(receiptMsg, command.getSender());  
}

void LoRaCommander::sendResult (Command command, CommandResult result) {
  String resultMsg = String(CMD_RESULT + String(delimiterId) + String(command.getId()) + String(delimiterCmd) + String(result));
  lora->send(resultMsg, command.getSender());  
}

void LoRaCommander::sendPing() {
  if (millis() - LORA_COMMANDER_PING_FREQUENCY > lastPing) {
    lastPing = millis();
    String pingMsg = String(CMD_PING + String(delimiterId) + String(sentCount++));
    lora->send(pingMsg, commanderAddress);
  }
}

