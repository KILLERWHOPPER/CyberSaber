#ifndef UTIL_HPP
#define UTIL_HPP

#include <Arduino.h>

#include "MFRC522.h"

void printHex(byte *buffer, byte bufferSize);

void printDec(byte *buffer, byte bufferSize);

void byte2HexStr(byte byte_arr[], char *buf);

void type2str(MFRC522::PICC_Type piccType, char *buf);

#endif