#ifndef RC522_HPP
#define RC522_HPP

#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

#include "global_var.hpp"
#include "pin_define.hpp"
#include "util.hpp"

extern MFRC522 mfrc522;
extern byte nuidPICC[4];
extern uint8_t rc522_status[3];  // Initiated, Running

void rc522Init();

void rc522ReadUID(void *readRFIDUID);

#endif