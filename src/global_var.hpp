#ifndef GLOBAL_VAR_HPP
#define GLOBAL_VAR_HPP

#include <Arduino.h>

extern int global_count;
extern int global_flag;
extern uint8_t cir_rs[];
extern int ani_stage;

extern char *strbuf;
extern char *strbuf2;

extern const char* menu[];
extern const char* rfid_menu[];

extern const char* DEVICE_NAME[];

extern const uint8_t DEVICES[][31];

/*
  These are more general home devices
  It seems these can work over long distances, especially AppleTV Setup
*/
extern const uint8_t SHORT_DEVICES[][23];

#endif