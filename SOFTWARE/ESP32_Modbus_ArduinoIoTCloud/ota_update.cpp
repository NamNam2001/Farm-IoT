#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"
#include "settings.h"
#include "EEPROM.h"

String VersionFirware = "";
String URL_fw_Bin;
String URL_fw_Version;

#define URL_fw_Bin_Environmental "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Environmental.ino.bin"
#define URL_fw_Version_Environmental "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_Environmental.txt"

#define URL_fw_Bin_Environmental_1 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Environmental_1.ino.bin"
#define URL_fw_Version_Environmental_1 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_Environmental_1.txt"

#define URL_fw_Bin_PH1 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Modbus_PH1.ino.bin"
#define URL_fw_Version_PH1 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_PH1.txt"

#define URL_fw_Bin_PH2 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Modbus_PH2.ino.bin"
#define URL_fw_Version_PH2 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_PH2.txt"

#define URL_fw_Bin_PH3 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Modbus_PH3.ino.bin"
#define URL_fw_Version_PH3 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_PH3.txt"

#define URL_fw_Bin_PH4 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Modbus_PH4.ino.bin"
#define URL_fw_Version_PH4 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_PH4.txt"

#define URL_fw_Bin_PH5 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Modbus_PH5.ino.bin"
#define URL_fw_Version_PH5 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_PH5.txt"

#define URL_fw_Bin_PH6 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Modbus_PH6.ino.bin"
#define URL_fw_Version_PH6 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_PH6.txt"

#define URL_fw_Bin_PH7 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Modbus_PH6.ino.bin"
#define URL_fw_Version_PH7 "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_PH6.txt"

#define URL_fw_Bin_test "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb_Modbus_test.ino.bin"
#define URL_fw_Version_test "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version_test.txt"

void firmwareUpdate(void)
{
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
    httpUpdate.setLedPin(2, LOW);
    t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        Serial.println("Da update duoc chua");
        user.FirmwareVer = VersionFirware;
        user.mode = 1;
        EEPROM.put(0, user);
        EEPROM.commit();
        ESP.restart();
        break;
    }
}
int FirmwareVersionCheck(void)
{
    if (strcmp(user.things, "Environmental") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_Environmental;
        URL_fw_Version = URL_fw_Version_Environmental;
    }
    else if (strcmp(user.things, "Environmental1") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_Environmental_1;
        URL_fw_Version = URL_fw_Version_Environmental_1;
    }
    else if (strcmp(user.things, "PH1") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_PH1;
        URL_fw_Version = URL_fw_Version_PH1;
    }
    else if (strcmp(user.things, "PH2") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_PH2;
        URL_fw_Version = URL_fw_Version_PH2;
    }
    else if (strcmp(user.things, "PH3") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_PH3;
        URL_fw_Version = URL_fw_Version_PH3;
    }
    else if (strcmp(user.things, "PH4") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_PH4;
        URL_fw_Version = URL_fw_Version_PH4;
    }
    else if (strcmp(user.things, "PH5") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_PH5;
        URL_fw_Version = URL_fw_Version_PH5;
    }
    else if (strcmp(user.things, "PH6") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_PH6;
        URL_fw_Version = URL_fw_Version_PH6;
    }
    else if (strcmp(user.things, "PH7") == 0)
    {
        URL_fw_Bin = URL_fw_Bin_PH7;
        URL_fw_Version = URL_fw_Version_PH7;
    }
    else
    {
        URL_fw_Bin = URL_fw_Bin_test;
        URL_fw_Version = URL_fw_Version_test;
    }
    String payload;
    int httpCode;
    // String fwurl = "";
    // fwurl += URL_fw_Version;
    // fwurl += "?";
    // fwurl += String(rand());
    Serial.println(URL_fw_Version);
    WiFiClientSecure *client = new WiFiClientSecure;

    if (client)
    {
        client->setCACert(rootCACertificate);

        // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
        HTTPClient https;

        if (https.begin(*client, URL_fw_Version))
        { // HTTPS
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTP header
            delay(100);
            httpCode = https.GET();
            delay(100);
            if (httpCode == HTTP_CODE_OK) // if version received
            {
                payload = https.getString(); // save received version
            }
            else
            {
                Serial.print("error in downloading version file:");
                Serial.println(httpCode);
            }
            https.end();
        }
        delete client;
    }

    if (httpCode == HTTP_CODE_OK) // if version received
    {
        payload.trim();
        if (payload.equals(user.FirmwareVer))
        {
            Serial.printf("\nDevice already on latest firmware version:%s\n", user.FirmwareVer);
            return 0;
        }
        else
        {
            Serial.println(payload);
            VersionFirware = payload;
            Serial.println("New firmware detected");
            return 1;
        }
    }
    return 0;
}

void UpdateOTA()
{
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to '");
        Serial.print(user.ssid);
        Serial.println("'");
        WiFi.begin(user.ssid, user.password);
        delay(2000);
    }
    Serial.print("You're connected to '");
    Serial.print(WiFi.SSID());
    Serial.println("'");
    if (digitalRead(0) == LOW)
    {
        user.mode = 1;
        EEPROM.put(0, user);
        EEPROM.commit();
        ESP.restart();
    }
    if (FirmwareVersionCheck())
    {
        firmwareUpdate();
    }
    else
    {
        user.mode = 1;
        EEPROM.put(0, user);
        EEPROM.commit();
    }
}