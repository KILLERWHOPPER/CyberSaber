#pragma once

#include <Arduino.h>
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : "");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : "");
    Serial.print(buffer[i], DEC);
  }
}

void byte2HexStr(byte byte_arr[], char *buf) {
  memset(buf, '\0', sizeof(buf));
  strcpy(buf, "0x");
  for (int i = 0; i < 4; i++) {
    sprintf(&buf[strlen(buf)], "%02X", byte_arr[i]);
  }
}
