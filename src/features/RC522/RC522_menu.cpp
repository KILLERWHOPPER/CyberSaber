#include "RC522.hpp"

void rc522() {
  switch (rc522_page) {
    case FUNC_MENU:
      // RFID menu
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      if (rc522_status[2] == 2) {
        display.drawString(64, 16, "Back");
      } else {
        display.drawString(64, 16, rfid_menu[rc522_status[2]]);
      }
      break;

    case SCANNING:
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
        xTaskCreatePinnedToCore(rc522ReadUID, "RC522ReadUID", 4096, NULL, 1,
                                &th_p[1], 0);
        rc522_status[1] = 1;
      }
      break;

    case SHOW_INFO:
      // RFID info display
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, strbuf);
      display.drawString(0, 16, strbuf2);

      rc522_status[0] = 0;
      rc522_status[1] = 0;
      break;
  }
}