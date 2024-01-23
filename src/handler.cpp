#include "handler.hpp"

REncoder encoder(Encoder_CLK, Encoder_DT, Encoder_SW);
void encoderHandler(REncoderWithoutSwitch::Event encoderEvent,
                    RENCODER_ROTATION_VALUE_TYPE encPos) {
  switch (encoderEvent) {
    case REncoder::Event::REncoder_Event_Rotate_CW:
      switch (global_page) {
        case MENU:
          // Choose main menu option
          global_count++;
          if (global_count > 5) {
            global_count = 5;
          }
          break;

        case EVIL_APPLE_JUICE:
          // Choose Evil Apple Juice device
          if (juice_page == DEVICE_MENU) {
            juice_status[2]++;
            if (juice_status[2] > 17) {
              juice_status[2] = 17;
            }
          }
          break;

        case RC522:
          // Choose RFID function
          if (rc522_page == FUNC_MENU) {
            rc522_status[2]++;
            if (rc522_status[2] > 2) {
              rc522_status[2] = 2;
            }
          }
          break;
      }
      break;

    case REncoder::Event::REncoder_Event_Rotate_CCW:
      switch (global_page) {
        case MENU:
          // Choose main menu option
          global_count--;
          if (global_count < 0) {
            global_count = 0;
          }
          break;

        case EVIL_APPLE_JUICE:
          // Choose Evil Apple Juice device
          if (juice_page == DEVICE_MENU) {
            juice_status[2]--;
            if (juice_status[2] < 0) {
              juice_status[2] = 0;
            }
          }
          break;

        case RC522:
          // Choose RFID function
          if (rc522_page == FUNC_MENU) {
            rc522_status[2]--;
            if (rc522_status[2] < 0) {
              rc522_status[2] = 0;
            }
          }
          break;
      }
      break;
  }
}

void switchHandler() {
  switch (global_page) {
    case MENU:
      switch (global_count) {
        case EVIL_APPLE_JUICE:
          // Switch to Evil Apple Juice choose device
          global_page = EVIL_APPLE_JUICE;
          break;
        case RC522:
          // Switch to RFID Menu
          global_page = RC522;
          break;
      }
      break;

    case EVIL_APPLE_JUICE:
      if (juice_page == SENDING) {
        // Stop Evil Apple Juice send packets
        juice_page = DEVICE_MENU;
        juice_status[0] = 0;
        juice_status[1] = 0;
      } else if (juice_status[2] == 17) {
        // Back to menu
        global_page = MENU;
        juice_status[2] = 0;
      } else {
        // Start Evil Apple Juice send packets
        juice_page = SENDING;
        ani_stage = 0;
      }
      break;

    case RC522:
      if (rc522_page == SCANNING) {
        // Stop RFID read
        rc522_page = FUNC_MENU;
        rc522_status[0] = 0;
        rc522_status[1] = 0;
      } else if (rc522_page == SHOW_INFO) {
        rc522_page = SCANNING;
        ani_stage = 0;
      } else if (rc522_status[2] == 2) {
        // Back to menu
        global_page = MENU;
        rc522_status[2] = 0;
      } else if(rc522_status[2] == 0) {
        // Start RFID read
        rc522_page = SCANNING;
        ani_stage = 0;
      }
      break;
  }
}
