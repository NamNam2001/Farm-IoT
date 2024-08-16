#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <Arduino.h>

extern String VersionFirware;

void firmwareUpdate(void);
int FirmwareVersionCheck(void);
void UpdateOTA(void);

#endif