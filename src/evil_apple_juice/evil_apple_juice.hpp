#ifndef EVIL_APPLE_JUICE_HPP
#define EVIL_APPLE_JUICE_HPP

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "global_var.hpp"

extern BLEAdvertising *pAdvertising;
extern uint32_t delayMilliseconds;
extern uint8_t juice_status[2];  // Initiated, Running

void evilAppleJuiceInit();

void evilAppleJuiceStart(void *startJuice);

#endif