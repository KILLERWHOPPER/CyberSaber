#include "handler.hpp"

REncoder encoder(Encoder_CLK, Encoder_DT, Encoder_SW);
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
