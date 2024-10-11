#include <EEPROM.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

uint16_t AddressWrite = 0x10; // Địa chỉ trong EEPROM

typedef struct
{
    uint32_t timeStart;     // Biến timeStart
    uint32_t interval_run;  // Biến interval_run
    uint32_t interval_wait; // Biến interval_wait
    uint16_t times;         // Biến times
} ConfigDevice;

typedef ConfigDevice XaNhap;

XaNhap Xanhap2;

// Hàm ghi một struct vào EEPROM
void EEPROM_writeStruct(int address, const XaNhap &deviceConfig)
{
    const byte *p = (const byte *)(const void *)&deviceConfig;
    for (unsigned int i = 0; i < sizeof(deviceConfig); i++)
        EEPROM.write(address + i, *p++);
}

// Hàm đọc một struct từ EEPROM
void EEPROM_readStruct(int address, XaNhap &deviceConfig)
{
    byte *p = (byte *)(void *)&deviceConfig;
    for (unsigned int i = 0; i < sizeof(deviceConfig); i++)
        *p++ = EEPROM.read(address + i);
}

void setup()
{
    Serial.begin(9600);

    // // Ghi struct Xanhap vào EEPROM
    // EEPROM_writeStruct(AddressWrite, Xanhap);
    // Serial.println("Ghi du lieu vao EEPROM");
}

void loop()
{
    // Đọc lại struct Xanhap2 từ EEPROM
    EEPROM_readStruct(AddressWrite, Xanhap2);
    Serial.println("Doc du lieu tu EEPROM");

    // In ra Serial để kiểm tra
    Serial.print("timeStart: ");
    Serial.println(Xanhap2.timeStart);

    Serial.print("interval_run: ");
    Serial.println(Xanhap2.interval_run);

    Serial.print("interval_wait: ");
    Serial.println(Xanhap2.interval_wait);

    Serial.print("times: ");
    Serial.println(Xanhap2.times);
    delay(2000);
}
