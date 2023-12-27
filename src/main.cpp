#include <Arduino.h>

#include <SSD1306Wire.h>
#include <Wire.h>

#include "global_var.hpp"
#include "pin_define.hpp"
#include "util.hpp"
#include "handler.hpp"
#include "evil_apple_juice/evil_apple_juice.hpp"
#include "RC522/RC522.hpp"

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
  switch (global_flag) {
    case 0:
      // Menu
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.drawString(64, 16, menu[global_count]);
      break;

    case 1:
      // Evil Apple Juice choose device
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      if (global_count == 17) {
        display.drawString(64, 16, "Back");
      } else {
        display.drawString(64, 16, DEVICE_NAME[global_count]);
      }
      break;

    case 2:
      // Sending packets
      // Display
      if (ani_stage == 4) ani_stage = 0;
      display.drawCircle(64, 16, cir_rs[ani_stage]);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, DEVICE_NAME[global_count]);
      ani_stage++;
      // Logic
      if (juice_status[0] == 0) {
        evilAppleJuiceInit();
        juice_status[0] = 1;
      }
      if (juice_status[1] == 0) {
        xTaskCreatePinnedToCore(evilAppleJuiceStart, "JuiceRun", 4096, NULL, 1,
                                &th_p[0], 0);
        juice_status[1] = 1;
      }
      break;

    case 3:
      // RFID menu
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      if (global_count == 2) {
        display.drawString(64, 16, "Back");
      } else {
        display.drawString(64, 16, rfid_menu[global_count]);
      }
      break;

    case 4:
      // RFID read
      // Display
      if (ani_stage == -1) ani_stage = 3;
      display.drawCircle(64, 16, cir_rs[ani_stage]);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, "Reading RFID");
      ani_stage--;

      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      // Logic
      if (rc522_status[0] == 0) {
        rc522Init();
        rc522_status[0] = 1;
      }
      if (rc522_status[1] == 0) {
        xTaskCreatePinnedToCore(rc522Read, "RC522Read", 4096, NULL, 1, &th_p[1],
                                0);
        rc522_status[1] = 1;
      }
      break;

    case 5:
      // RFID info display
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, strbuf);
      display.drawString(0, 16, strbuf2);
      break;
  }

  if (global_flag != 2) {
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
  }

  if (global_flag != 4) {
    if (rc522_status[1] == 1) {
      // Stop RFID read
      vTaskDelete(th_p[1]);
      rc522_status[1] = 0;
    }
    // if (rc522_status[0] == 1) {
    //   // Stop RFID device
    //   //mfrc522.PCD_SoftPowerDown();
    //   rc522_status[0] = 0;
    // }
  }

  display.display();
  delay(200);
}

