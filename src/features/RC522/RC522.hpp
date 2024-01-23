#ifndef RC522_HPP
#define RC522_HPP

#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

#include "global_var.hpp"
#include "pin_define.hpp"
#include "util.hpp"

enum rc522_menu_page {
    FUNC_MENU,
    SCANNING,
    SHOW_INFO
};

extern rc522_menu_page rc522_page;
extern MFRC522 mfrc522;
extern byte nuidPICC[4];
extern uint8_t rc522_status[3];  // Initiated, Running, Menu count

extern char *strbuf;
extern char *strbuf2;

extern const char *rfid_menu[];

void rc522Init();

void rc522ReadUID(void *readRFIDUID);

void rc522();

#endif