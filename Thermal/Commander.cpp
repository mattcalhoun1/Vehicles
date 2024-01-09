#include "Commander.h"

void Commander::logConsole (String msg) {
  if (LOG_ENABLED) {
    Serial.println(msg);
  }
}

CommandType Command::getCommandType () {
  return commandType;
}

void Command::setCommandType (CommandType commandType) {
  this->commandType = commandType;
}

String Command::getParams () {
  return params;
}

void Command::setParams (String params) {
  this->params = params;
}


void Command::setId (long id) {
  this->id = id;
}

long Command::getId () {
  return id;
}

void Command::setSender (int sender) {
  this->sender = sender;
}

int Command::getSender () {
  return sender;
}

