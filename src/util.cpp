#include "util.hpp"
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : "");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : "");
    Serial.print(buffer[i], DEC);
  }
}

void byte2HexStr(byte byte_arr[], char *buf) {
  memset(buf, '\0', sizeof(buf));
  strcpy(buf, "UID: 0x");
  for (int i = 0; i < 4; i++) {
    sprintf(&buf[strlen(buf)], "%02X", byte_arr[i]);
  }
}

void type2str(MFRC522::PICC_Type piccType, char *buf) {
  memset(buf, '\0', sizeof(buf));
  strcpy(buf, "Type: ");
  switch (piccType) {
    case MFRC522::PICC_TYPE_UNKNOWN:
      strcat(buf, "Unknown");
      break;
    case MFRC522::PICC_TYPE_ISO_14443_4:
      strcat(buf, "ISO/IEC 14443-4");
      break;
      // PICC compliant with ISO/IEC 14443-4
    case MFRC522::PICC_TYPE_ISO_18092:
      strcat(buf, "ISO/IEC 18092");
      break;  // PICC compliant with ISO/IEC 18092 (NFC)
    case MFRC522::PICC_TYPE_MIFARE_MINI:
      strcat(buf, "MIFARE_MINI");
      break;  // MIFARE Classic protocol, 320 bytes
    case MFRC522::PICC_TYPE_MIFARE_1K:
      strcat(buf, "MIFARE_1K");
      break;  // MIFARE Classic protocol, 1KB
    case MFRC522::PICC_TYPE_MIFARE_4K:
      strcat(buf, "MIFARE_4K");
      break;  // MIFARE Classic protocol, 4KB
    case MFRC522::PICC_TYPE_MIFARE_UL:
      strcat(buf, "MIFARE_UL");
      break;  // MIFARE Ultralight or Ultralight C
    case MFRC522::PICC_TYPE_MIFARE_PLUS:
      strcat(buf, "MIFARE_PLUS");
      break;  // MIFARE Plus
    case MFRC522::PICC_TYPE_MIFARE_DESFIRE:
      strcat(buf, "MIFARE_DESFIRE");
      break;  // MIFARE DESFire
    case MFRC522::PICC_TYPE_TNP3XXX:
      strcat(buf, "TNP3XXX");
      break;  // Only mentioned in NXP AN 10833 MIFARE Type Identification
              // procedure
    case MFRC522::PICC_TYPE_NOT_COMPLETE:
      strcat(buf, "NOT_COMPLETE");
      break;  // SAK indicates UID is not complete.
  }
}
