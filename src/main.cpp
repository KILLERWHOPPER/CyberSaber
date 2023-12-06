// // This example takes heavy inpsiration from the ESP32 example by
// ronaldstoner
// // Based on the previous work of chipik / _hexway / ECTO-1A & SAY-10
// // See the README for more info
// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>

// #include "devices.hpp"

// BLEAdvertising *pAdvertising;  // global variable
// uint32_t delayMilliseconds = 1000;

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Starting ESP32 BLE");

//   // This is specific to the AirM2M ESP32 board
//   // https://wiki.luatos.com/chips/esp32c3/board.html
//   pinMode(12, OUTPUT);
//   pinMode(13, OUTPUT);

//   BLEDevice::init("AirPods 69");

//   // Create the BLE Server
//   BLEServer *pServer = BLEDevice::createServer();
//   pAdvertising = pServer->getAdvertising();

//   // seems we need to init it with an address in setup() step.
//   esp_bd_addr_t null_addr = {0xFE, 0xED, 0xC0, 0xFF, 0xEE, 0x69};
//   pAdvertising->setDeviceAddress(null_addr, BLE_ADDR_TYPE_RANDOM);
// }

// void loop() {
//   // Turn lights on during "busy" part
//   digitalWrite(12, HIGH);
//   digitalWrite(13, HIGH);

//   // First generate fake random MAC
//   esp_bd_addr_t dummy_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//   for (int i = 0; i < 6; i++) {
//     dummy_addr[i] = random(256);

//     // It seems for some reason first 4 bits
//     // Need to be high (aka 0b1111), so we
//     // OR with 0xF0
//     if (i == 0) {
//       dummy_addr[i] |= 0xF0;
//     }
//   }

//   BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

//   // Randomly pick data from one of the devices
//   // First decide short or long
//   // 0 = long (headphones), 1 = short (misc stuff like Apple TV)
//   int device_choice = random(2);
//   if (device_choice == 0) {
//     int index = random(17);
//     oAdvertisementData.addData(std::string((char *)DEVICES[index], 31));
//   } else {
//     int index = random(12);
//     oAdvertisementData.addData(std::string((char *)SHORT_DEVICES[index],
//     23));
//   }


//   int adv_type_choice = random(3);
//   if (adv_type_choice == 0) {
//     pAdvertising->setAdvertisementType(ADV_TYPE_IND);
//   } else if (adv_type_choice == 1) {
//     pAdvertising->setAdvertisementType(ADV_TYPE_SCAN_IND);
//   } else {
//     pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
//   }

//   // Set the device address, advertisement data
//   pAdvertising->setDeviceAddress(dummy_addr, BLE_ADDR_TYPE_RANDOM);
//   pAdvertising->setAdvertisementData(oAdvertisementData);

//   // Set advertising interval

//   // pAdvertising->setMinInterval(0x20);
//   // pAdvertising->setMaxInterval(0x20);
//   // pAdvertising->setMinPreferred(0x20);
//   // pAdvertising->setMaxPreferred(0x20);

//   // Start advertising
//   Serial.println("Sending Advertisement...");
//   pAdvertising->start();

//   // Turn lights off while "sleeping"
//   digitalWrite(12, LOW);
//   digitalWrite(13, LOW);
//   delay(delayMilliseconds);  // delay for delayMilliseconds ms
//   pAdvertising->stop();
// }

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <REncoder.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "devices.hpp"

BLEAdvertising *pAdvertising;  // global variable
uint32_t delayMilliseconds = 1000;
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

REncoder encoder(/*CLK*/ 5, /*DT*/ 18, /*SW*/ 19);
long dp_num = 0;

void encoderHandler(REncoderWithoutSwitch::Event encoderEvent,
                    RENCODER_ROTATION_VALUE_TYPE encPos) {
  switch (encoderEvent) {
    case REncoder::Event::REncoder_Event_Rotate_CW:
      Serial.println("IRQ -> Rotation CW to: " + String(encPos));
      dp_num++;
      break;

    case REncoder::Event::REncoder_Event_Rotate_CCW:
      Serial.println("IRQ -> Rotation CCW to: " + String(encPos));
      dp_num--;
      break;
  }
}

void setup() {
  Wire.begin(/*SDA*/ 16, /*SCL*/ 17);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  encoder.setMinEncoderPosition(-1);
  encoder.setMaxEncoderPosition(1);
  encoder.attachSwitchHandler([]() {
    dp_num = 0;
    Serial.println("IRQ -> Switch pushed.");
  });

  encoder.attachEncoderHandler(encoderHandler);
  Serial.begin(115200);

    BLEDevice::init("AirPods 69");

    // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pAdvertising = pServer->getAdvertising();

    // seems we need to init it with an address in setup() step.
    esp_bd_addr_t null_addr = {0xFE, 0xED, 0xC0, 0xFF, 0xEE, 0x69};
    pAdvertising->setDeviceAddress(null_addr, BLE_ADDR_TYPE_RANDOM);
}

void loop() {
    // Set the fake device mac address
    esp_bd_addr_t dummy_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < 6; i++) {
      dummy_addr[i] = random(256);

      // It seems for some reason first 4 bits
      // Need to be high (aka 0b1111), so we
      // OR with 0xF0
      if (i == 0) {
        dummy_addr[i] |= 0xF0;
      }
    }

    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();

    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("0.91 OLED");

    display.setCursor(0, 10);
    display.print("Run time: ");
    display.print(millis() / 1000);
    display.print("s");

    display.setCursor(0, 20);
    display.print("Number: ");
    display.print(dp_num);

    display.display();

    delay(100);
}
