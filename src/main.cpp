#include <Arduino.h>
#include <SSD1306Wire.h>
#include <Wire.h>

#include "features/RC522/RC522.hpp"
#include "features/evil_apple_juice/evil_apple_juice.hpp"
#include "global_var.hpp"
#include "handler.hpp"
#include "pin_define.hpp"
#include "util.hpp"

#define USE_MULTCORE 1
#define NR_OF_READERS 2

TaskHandle_t th_p[2];  // Task handle list

// Display Stub
SSD1306Wire display(0x3c, SDA, SCK, GEOMETRY_128_32);

void setup() {
  // Initialize display
  display.init();
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  // Initialize incoder
  encoder.setMinEncoderPosition(-1);
  encoder.setMaxEncoderPosition(1);
  encoder.attachSwitchHandler(switchHandler);
  encoder.attachEncoderHandler(encoderHandler);
  // Start serial port
  Serial.begin(115200);
}

void loop() {
  display.clear();
  switch (global_page) {
    case MENU:
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(64, 16, menu[global_count]);
      if (juice_status[1] == 1) {
        // Stop Evil Apple Juice
        vTaskDelete(th_p[0]);
        juice_status[1] = 0;
      }
      if (juice_status[0] == 1) {
        // Stop BLE device
        pAdvertising->stop();
        juice_status[0] = 0;
      }
      if (rc522_status[1] == 1) {
        // Stop RFID read
        vTaskDelete(th_p[1]);
        rc522_status[1] = 0;
      }
      break;
    case EVIL_APPLE_JUICE:
      evilAppleJuice();
      break;
    case RC522:
      rc522();
      break;
  }

  display.display();
  delay(200);
}
