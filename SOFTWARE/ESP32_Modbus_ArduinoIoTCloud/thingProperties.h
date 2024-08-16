#include <algorithm>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"
#include "settings.h"
// #include <ArduinoIoTCloud_ConfigWeb.cpp>
// #include <Wifi.h>
// #include <WebServer.h>
// extern settings user_wifi;
// EEPROM.begin(sizeof(struct settings));
// EEPROM.get(0, user_wifi);

#if !(defined(BOARD_HAS_WIFI) || defined(BOARD_HAS_GSM) || defined(BOARD_HAS_LORA) || \
      defined(BOARD_HAS_NB) || defined(BOARD_HAS_ETHERNET) || defined(BOARD_HAS_CATM1_NBIOT))
#error "Please check Arduino IoT Cloud supported boards list: https://github.com/arduino-libraries/ArduinoIoTCloud/#what"
#endif

#if defined(BOARD_HAS_SECRET_KEY)
#define BOARD_ID ""
#endif

void onDoValChange();
void onDoValChange3();
void onPhValChange();
void onPhValChange2();
void onPhValChange3();
void onRainFallChange();
void onSumRainFallChange();
void onTemperatureChange();
void onTempValChange();
void onTempValChange2();
void onTempValChange3();
void onLightValChange();
void onHumidityChange();
void onFirmwareVerChange();
void onSsidValChange();

float do_val;
float do_val3;
float ph_val;
float ph_val2;
float ph_val3;
float do_val3_1;
float rainFall;
float sum_rainFall;
CloudTemperatureSensor temperature;
CloudTemperatureSensor temp_val;
CloudTemperatureSensor temp_val2;
CloudTemperatureSensor temp_val3;
int light_val;
CloudRelativeHumidity humidity;
float firmware_Ver;
bool status_rain;
bool status_rain1;
bool status_rain2;
String ssid_val;
String ph_warning;

void initProperties()
{
#if defined(BOARD_HAS_SECRET_KEY)
  // ArduinoCloud.setBoardId(BOARD_ID);
  // ArduinoCloud.setSecretDeviceKey(SECRET_DEVICE_KEY);

  ArduinoCloud.setBoardId(user.board_id);
  ArduinoCloud.setSecretDeviceKey(user.device_key);
#endif
#if defined(BOARD_HAS_WIFI) || defined(BOARD_HAS_GSM) || defined(BOARD_HAS_NB) || defined(BOARD_HAS_ETHERNET) || defined(BOARD_HAS_CATM1_NBIOT)
  // ArduinoCloud.addProperty(led, Permission::Write).onUpdate(onLedChange);
  // ArduinoCloud.addProperty(potentiometer, Permission::Read).publishOnChange(10);
  // ArduinoCloud.addProperty(seconds, Permission::Read).publishOnChange(1);
  ArduinoCloud.addProperty(do_val, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(do_val3, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(rainFall, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(sum_rainFall, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(temperature, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(temp_val, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(ph_warning, READWRITE, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(ph_val, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(light_val, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(humidity, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(firmware_Ver, READWRITE, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(ssid_val, READWRITE, ON_CHANGE, NULL);

  ArduinoCloud.addProperty(temp_val2, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(ph_val2, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(temp_val3, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(ph_val3, READWRITE, 60 * SECONDS, NULL);
  ArduinoCloud.addProperty(do_val3_1, READWRITE, 60 * SECONDS);

  ArduinoCloud.addProperty(status_rain, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(status_rain1, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(status_rain2, READ, ON_CHANGE, NULL);
#elif defined(BOARD_HAS_LORA)
  ArduinoCloud.addProperty(led, 1, READWRITE, ON_CHANGE, onLedChange);
  ArduinoCloud.addProperty(potentiometer, 2, READ, ON_CHANGE);
  ArduinoCloud.addProperty(seconds, 3, READ, 5 * MINUTES);
#endif
}

#if defined(BOARD_HAS_ETHERNET)
/* DHCP mode */
// EthernetConnectionHandler ArduinoIoTPreferredConnection;
/* Manual mode. It will fallback in DHCP mode if SECRET_OPTIONAL_IP is invalid or equal to "0.0.0.0" */
EthernetConnectionHandler ArduinoIoTPreferredConnection(SECRET_OPTIONAL_IP, SECRET_OPTIONAL_DNS, SECRET_OPTIONAL_GATEWAY, SECRET_OPTIONAL_NETMASK);
#elif defined(BOARD_HAS_WIFI)
// WiFiConnectionHandler ArduinoIoTPreferredConnection(user_wifi.ssid, user_wifi.password);
WiFiConnectionHandler ArduinoIoTPreferredConnection(user.ssid, user.password);
#elif defined(BOARD_HAS_GSM)
GSMConnectionHandler ArduinoIoTPreferredConnection(SECRET_PIN, SECRET_APN, SECRET_LOGIN, SECRET_PASS);
#elif defined(BOARD_HAS_LORA)
LoRaConnectionHandler ArduinoIoTPreferredConnection(SECRET_APP_EUI, SECRET_APP_KEY, _lora_band::EU868, NULL, _lora_class::CLASS_A);
#elif defined(BOARD_HAS_NB)
NBConnectionHandler ArduinoIoTPreferredConnection(SECRET_PIN, SECRET_APN, SECRET_LOGIN, SECRET_PASS);
#elif defined(BOARD_HAS_CATM1_NBIOT)
CatM1ConnectionHandler ArduinoIoTPreferredConnection(SECRET_PIN, SECRET_APN, SECRET_LOGIN, SECRET_PASS);
#endif
