#include "EEPROMConfig.h"
#include <config.h>

void EEPROM_writeStruct(int address, lorawanConfig &lorawanConfig)
{
    const byte *p = (const byte *)(const void *)&lorawanConfig;
    for (unsigned int i = 0; i < sizeof(lorawanConfig); i++)
        EEPROM.write(address + i, *p++);
}

void EEPROM_readStruct(int address, lorawanConfig &lorawanConfig)
{
    byte *p = (byte *)(void *)&lorawanConfig;
    for (unsigned int i = 0; i < sizeof(lorawanConfig); i++)
        *p++ = EEPROM.read(address + i);
}

bool isNewEEPROM(const uint8_t *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (data[i] != 0xFF)
        {
            return false;
        }
    }
    return true;
}
