#ifndef EVIL_APPLE_JUICE_HPP
#define EVIL_APPLE_JUICE_HPP

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "global_var.hpp"

enum juice_menu_page {
    DEVICE_MENU,
    SENDING
};
extern const char *DEVICE_NAME[];
extern const uint8_t DEVICES[][31];
/*
  These are more general home devices
  It seems these can work over long distances, especially AppleTV Setup
*/
extern const uint8_t SHORT_DEVICES[][23];

extern juice_menu_page juice_page;
extern BLEAdvertising *pAdvertising;
extern uint32_t delayMilliseconds;
extern uint8_t juice_status[3];  // Initiated, Running, device count

void evilAppleJuiceInit();

void evilAppleJuiceStart(void *startJuice);

void evilAppleJuice();

#endif