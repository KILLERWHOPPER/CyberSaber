#ifndef GLOBAL_VAR_HPP
#define GLOBAL_VAR_HPP

#include <Arduino.h>
#include <SSD1306Wire.h>

enum page_index {
    MENU = 0,
    EVIL_APPLE_JUICE = 1,
    RC522 = 2
};

extern SSD1306Wire display;
extern TaskHandle_t th_p[];
extern page_index global_page;

extern int global_count;
extern uint8_t cir_rs[];
extern int ani_stage;



extern const char* menu[];


#endif