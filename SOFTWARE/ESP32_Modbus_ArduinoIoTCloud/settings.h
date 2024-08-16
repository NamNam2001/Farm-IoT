#ifndef SETTINGS_H
#define SETTINGS_H

struct settings
{
  char ssid[30];
  char password[30];
  int mode;
  String FirmwareVer;
  char board_id[40];
  char device_key[30];
  char things[15];
};

extern settings user;

#endif