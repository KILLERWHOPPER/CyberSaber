#include "RC522.hpp"

MFRC522 mfrc522 = MFRC522(SDA_RC, RST_RC);
byte nuidPICC[4];
uint8_t rc522_status[3] = {0, 0, 0};  // Initiated, Running

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
