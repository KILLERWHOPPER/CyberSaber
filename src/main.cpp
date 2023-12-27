#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <MFRC522.h>
#include <REncoder.h>
#include <SPI.h>
#include <SSD1306Wire.h>
#include <Wire.h>
#include <string.h>

#include "global_var.hpp"
#include "pin_define.hpp"
#include "util.hpp"

#define USE_MULTCORE 1

#define NR_OF_READERS 2

TaskHandle_t th_p[2];  // Task handle list

// Display Stub
SSD1306Wire display(0x3c, SDA, SCK, GEOMETRY_128_32);

// BLE Stub
BLEAdvertising *pAdvertising;
uint32_t delayMilliseconds = 1000;
uint8_t juice_status[2] = {0, 0};  // Initiated, Running

// RFID Stub
MFRC522 mfrc522 = MFRC522(SDA_RC, RST_RC);
byte nuidPICC[4];
uint8_t rc522_status[3] = {0, 0, 0};  // Initiated, Running
char *strbuf = (char *)malloc(32);
char *strbuf2 = (char *)malloc(32);

// Encoder Stub
REncoder encoder(Encoder_CLK, Encoder_DT, Encoder_SW);

void evilAppleJuiceInit();

void evilAppleJuiceStart(void *startJuice);

void rc522Init();

void rc522Read(void *readRFID);

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

        case 3:
          // Choose RFID function
          global_count++;
          if (global_count > 2) {
            global_count = 2;
          }
          break;
      }
      break;
    case REncoder::Event::REncoder_Event_Rotate_CCW:
      if (global_flag != 2) {
        global_count--;
        if (global_count < 0) {
          global_count = 0;
        }
      }
      break;
  }
}

void switchHandler() {
  switch (global_flag) {
    case 0:
      switch (global_count) {
        case 0:
          // Switch to Evil Apple Juice choose device
          global_flag = 1;
          global_count = 0;
          break;
        case 1:
          // Switch to RFID Menu
          global_flag = 3;
          global_count = 0;
          break;
      }
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
      break;

    case 3:
      if (global_count == 2) {
        // Back to menu
        global_flag = 0;
        global_count = 0;
      } else if (global_count == 0) {
        // Start RFID read
        global_flag = 4;
        global_count = 0;
      }
      break;

    case 4:
      // Stop RFID read
      global_count = 0;
      global_flag = 3;
      break;

    case 5:
      // Back to RFID read
      global_flag = 4;
      global_count = 0;
      break;
  }
}

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

void rc522Init() {
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
}

void rc522Read(void *readRFID) {
  while (1) {
    // Serial.print("Reading");
    if (!mfrc522.PICC_IsNewCardPresent()) continue;

    // Check readable
    if (!mfrc522.PICC_ReadCardSerial()) continue;

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);

    // Check NUID
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println("Unsupported Tag");
      continue;
    }

    // Store NUID in nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = mfrc522.uid.uidByte[i];
    }
    rc522_status[2] = 1;
    byte2HexStr(nuidPICC, strbuf);
    type2str(piccType, strbuf2);
    Serial.print("UID in HEX:");
    printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();

    Serial.print("UID in DEC:");
    printDec(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();

    // Send sleep command
    mfrc522.PICC_HaltA();

    global_flag = 5;
    delay(1);
  }
}
