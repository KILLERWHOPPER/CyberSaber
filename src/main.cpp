#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <REncoder.h>
#include <SSD1306Wire.h>
#include <Wire.h>

#include "devices.hpp"

#define Encoder_CLK 5
#define Encoder_DT 18
#define Encoder_SW 19
#define SCK 17
#define SDA 16
BLEAdvertising *pAdvertising;  // global variable
uint32_t delayMilliseconds = 1000;

SSD1306Wire display(0x3c, SCK, SDA, GEOMETRY_128_32);

REncoder encoder(Encoder_CLK, Encoder_DT, Encoder_SW);
long dp_num = 0;
long global_flag = 0;
// 0: Menu
void encoderHandler(REncoderWithoutSwitch::Event encoderEvent,
                    RENCODER_ROTATION_VALUE_TYPE encPos) {
  if (global_flag == 0) {
    switch (encoderEvent) {
      case REncoder::Event::REncoder_Event_Rotate_CW:
        Serial.println("IRQ -> Rotation CW to: " + String(encPos));
        dp_num++;
        if (dp_num > 16) {
          dp_num = 16;
        }
        break;
      case REncoder::Event::REncoder_Event_Rotate_CCW:
        Serial.println("IRQ -> Rotation CCW to: " + String(encPos));
        dp_num--;
        if (dp_num < 0) {
          dp_num = 0;
        }
        break;
    }
  }
}

void setup() {
  display.init();

  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);

  encoder.setMinEncoderPosition(-1);
  encoder.setMaxEncoderPosition(1);
  encoder.attachSwitchHandler([]() {
    switch (global_flag) {
      case 0:
        global_flag = 1;
        break;

      case 1:
        global_flag = 0;
        break;
    }

    Serial.println("IRQ -> Switch pushed.");
  });

  encoder.attachEncoderHandler(encoderHandler);
  Serial.begin(115200);
}

uint8_t cir_rs[4] = {2,4,6,8}; 
uint8_t ani_stage = 0;
void loop() {
  display.clear();
  switch (global_flag) {
    case 0:
      display.drawString(64, 16, DEVICE_NAME[dp_num]);
      break;

    case 1:
        if(ani_stage == 4) ani_stage =0;
        display.drawCircle(64, 16, cir_rs[ani_stage]);
        ani_stage++;
  }

  display.display();
  delay(200);
}
