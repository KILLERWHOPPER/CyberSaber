#include "evil_apple_juice.hpp"

juice_menu_page juice_page = DEVICE_MENU;
void evilAppleJuice() {
  switch (juice_page) {
    case DEVICE_MENU:
      // Evil Apple Juice choose device
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      if (juice_status[2] == 17) {
        display.drawString(64, 16, "Back");
      } else {
        display.drawString(64, 16, DEVICE_NAME[juice_status[2]]);
      }
      break;

    case SENDING:
      // Sending packets
      // Display
      if (ani_stage == 4) ani_stage = 0;
      display.drawCircle(64, 16, cir_rs[ani_stage]);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, DEVICE_NAME[juice_status[2]]);
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
  }
}