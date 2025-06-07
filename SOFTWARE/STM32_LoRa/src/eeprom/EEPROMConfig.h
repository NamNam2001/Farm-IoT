#ifndef EEPROMCONFIG_H
#define EEPROMCONFIG_H

#include <EEPROM.h>
#include "lora/LoRaWAN.h"

void EEPROM_writeStruct(int address, lorawanConfig &lorawanConfig);
void EEPROM_readStruct(int address, lorawanConfig &lorawanConfig);
bool isNewEEPROM(const uint8_t *data, size_t length);
void handleInterruptResetEEPROM();
#endif
