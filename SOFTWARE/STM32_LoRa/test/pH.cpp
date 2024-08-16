#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <EmonLib.h>
#include "Canopus_Modbus.h"
#include "Rak3172_Canopus.h"
#include "SoftSerialSTM32.h"
// #include <STM32FreeRTOS.h>
#include <MapleFreeRTOS821.h>
// #include "LowPower.h"
// Modbus
// SoftSerialSTM32 SWSerial0(PB6, PB7);
// HardwareSerial Serial3;
ModbusMaster node[4];
uint8_t result;

// const int sensorPin = 2; // Chân đầu vào của cảm biến
EnergyMonitor SCT013[4];
int pinSCT1 = PA0; // cảm biến kết nối với chân A0
int pinSCT2 = PA1;
int pinSCT3 = PA2;
int pinSCT4 = PA3;
int vol = 230;
int pulseCount = 0;
bool sensor_Water = false;
int metalProximitySensorPin = PB15;

// // stm32-motor-current-9
// static const u1_t PROGMEM APPEUI[8] = {0xEE, 0xEF, 0xAF, 0x8E, 0xEF, 0xAF, 0x7D, 0xDE}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x98, 0x8D, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x2C, 0x13, 0x7D, 0x7E, 0x8C, 0x2D, 0x00, 0xBA, 0x9E, 0xA7, 0xCF, 0xB4, 0x9E, 0xE1, 0x23, 0x0C}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

// // stm32-motor-current-8
// static const u1_t PROGMEM APPEUI[8] = {0xE4, 0x33, 0x99, 0x84, 0xE2, 0x82, 0x87, 0x23}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0xE4, 0x8A, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x4F, 0x75, 0x79, 0x7E, 0x8C, 0xFD, 0x94, 0xE2, 0xB6, 0x80, 0x44, 0xAA, 0xAA, 0x7F, 0x7D, 0x25}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }
// stm32-motor-current-7
static const u1_t PROGMEM APPEUI[8] = {0x54, 0x32, 0x48, 0x28, 0x99, 0x33, 0x84, 0x73}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
void os_getArtEui(u1_t *buf)
{
    memcpy_P(buf, APPEUI, 8);
}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = {0x61, 0x88, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getDevEui(u1_t *buf)
{
    memcpy_P(buf, DEVEUI, 8);
}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = {0x27, 0xF7, 0xB4, 0x32, 0xB5, 0x11, 0x80, 0x78, 0x4E, 0xBF, 0xD0, 0x9C, 0x2B, 0xC1, 0xCE, 0x8C}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
void os_getDevKey(u1_t *buf)
{
    memcpy_P(buf, APPKEY, 16);
}

// // stm32-motor-current-6
// static const u1_t PROGMEM APPEUI[8] = {0x09, 0xF9, 0x90, 0x02, 0x99, 0x35, 0x82, 0x37}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x8C, 0x86, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0xC0, 0xA1, 0xAF, 0x7C, 0x9D, 0x9E, 0x7F, 0x02, 0x0F, 0x32, 0xAA, 0x24, 0x6D, 0x09, 0x17, 0x26}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

// ////stm32-motor-current-5
// static const u1_t PROGMEM APPEUI[8] = {0x44, 0x44, 0x33, 0x33, 0x33, 0x22, 0x22, 0x22}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x55, 0x51, 0x52, 0x53, 0x25, 0x34, 0x23, 0x12}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x68, 0x66, 0x38, 0x92, 0x12, 0xFB, 0xF7, 0x55, 0x67, 0xAC, 0x9A, 0xF5, 0x8A, 0xBC, 0xD0, 0x40}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

// ////test
// static const u1_t PROGMEM APPEUI[8] = {0x83, 0x29, 0xEE, 0x83, 0x8E, 0xDF, 0xFA, 0xFD}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x49, 0x80, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x15, 0x13, 0x83, 0x2D, 0x6F, 0xDF, 0x02, 0x7B, 0x7B, 0xE0, 0x0D, 0xA0, 0xDA, 0x41, 0x48, 0xD7}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

// ////stm32-motor-current-4
// static const u1_t PROGMEM APPEUI[8] = {0x12, 0x12, 0x12, 0x22, 0x20, 0x20, 0x20, 0x20}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0xE1, 0x60, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x4F, 0xA3, 0x56, 0x12, 0xDD, 0xAA, 0xAB, 0x56, 0xBB, 0x57, 0x80, 0x5C, 0x35, 0xB3, 0xDF, 0x71}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }
// ////stm32-motor-current-3
// static const u1_t PROGMEM APPEUI[8] = {0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x23, 0x23}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x21, 0x23, 0x25, 0x24, 0x26, 0x24, 0x25, 0x24}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0xAA, 0x7D, 0x74, 0x3C, 0x20, 0x4E, 0xC1, 0x78, 0xC5, 0xCE, 0xC2, 0xD4, 0x62, 0xEE, 0x77, 0x34}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

// ////stm32-motor-current
// static const u1_t PROGMEM APPEUI[8] = {0x25, 0x25, 0x24, 0x24, 0x23, 0x23, 0x21, 0x21}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x65, 0x35, 0x42, 0x21, 0x21, 0x51, 0xFB, 0x25}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x59, 0x08, 0xBC, 0xDE, 0xEC, 0x8F, 0xFE, 0x74, 0x55, 0xAF, 0x73, 0x3B, 0x9B, 0xEE, 0x2B, 0x3D}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

// // stm32-motor-current-1
// static const u1_t PROGMEM APPEUI[8] = {0x53, 0x35, 0x52, 0x52, 0x21, 0x63, 0x54, 0x20}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0xFC, 0x5B, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x81, 0x2A, 0x96, 0xB0, 0xF6, 0xC2, 0xAC, 0x50, 0x79, 0x2E, 0x9F, 0x61, 0x3A, 0xAC, 0xD0, 0xEC}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

// // stm32-test
// static const u1_t PROGMEM APPEUI[8] = {0x83, 0x29, 0xEE, 0x83, 0x8E, 0xDF, 0xFA, 0xFD}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x49, 0x80, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x15, 0x13, 0x83, 0x2D, 0x6F, 0xDF, 0x02, 0x7B, 0x7B, 0xE0, 0x0D, 0xA0, 0xDA, 0x41, 0x48, 0xD7}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

static osjob_t sendjob;
unsigned char mydata[18];
uint8_t data;

// unsigned char mydata1[]= "Hello NamNam201";

bool sensor_SCT013 = false;
bool previousValue = false;
// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 1000;
unsigned times_send = 0;

// Pin mapping for STM32
const lmic_pinmap lmic_pins = {
    .nss = PA4,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PB0,
    .dio = {PB8, PB9, LMIC_UNUSED_PIN},
};

void printHex2(unsigned v)
{
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void do_send(osjob_t *j)
{
    // Check if there is not a current TX/RX job running

    if (LMIC.opmode & OP_TXRXPEND)
    {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
        // LMIC_setTxData2(1, &data, sizeof(data), 0);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

// extern volatile unsigned long timer0_millis;
// void addMillis(unsigned long extra_millis) {
//   uint8_t oldSREG = SREG;
//   cli();
//   timer0_millis += extra_millis;
//   SREG = oldSREG;
//   sei();
// }
// bool checkWater(){
//     if(pulseCount> 5){
//         times_send = 0;
//         sensor_Water = true;
//         Serial.println("Device2 is working");
//     } else {
//         sensor_Water = false;
//         Serial.println("Device2 is not working");
//     }
//     mydata[1] = sensor_Water ? 1 : 0;
//     return sensor_Water;
// }

// check Currents from SCT013 devices
void checkCurrent(int num_devices)
{
    int SCT_pins[4] = {pinSCT1, pinSCT2, pinSCT3, pinSCT4};
    for (int i = 0; i < 4; ++i)
    {
        SCT013[i].current(SCT_pins[i], 60.606);
    }
    delay(1000);
    for (int i = 0; i < num_devices; i++)
    {
        double Irms = SCT013[i].calcIrms(1480);
        Serial.print(i);
        Serial.print(" : ");
        Serial.println(Irms);
        Serial.print("---");
        sensor_SCT013 = Irms < 0.85 ? false : true;
        // if (Irms < 0.85)
        // {
        //     sensor_SCT013 = false;
        //     // Serial.println("Device is not working");
        // }
        // else
        // {
        //     sensor_SCT013 = true;
        //     // Serial.println("Device is working");
        // }
        mydata[i] = sensor_SCT013 ? 1 : 0;
        delay(100);
    }
}

void calculateBytes(int value, int &highByte, int &lowByte)
{
    //  calculate Bytes for value
    lowByte = value % 256;  // lowByte
    highByte = value / 256; // highByte
}

// check Parameters from PH6000V device via Modbus
void checkParameters_PH6000V(int num_InputRegisters, int device_ID, ModbusMaster node)
{
    node.begin(device_ID, Serial3 /*SWSerial0*/);
    Serial.println("*******STM32-Modbus-R485_by_NamNam201*********");
    Serial.println("=====readSensor1======");
    result = node.readInputRegisters(0, num_InputRegisters);
    if (result == node.ku8MBSuccess)
    {
        // for (uint8_t i = 0; i < num_InputRegisters; i++)
        // {
        //     int values = node.getResponseBuffer(i);
        //     Serial.println(values);
        // }
        mydata[4] = node.getResponseBuffer(0) % 256;
        mydata[5] = node.getResponseBuffer(0) / 256;

        mydata[6] = node.getResponseBuffer(2) % 256;
        mydata[7] = node.getResponseBuffer(2) / 256;
    }
    else
    {
        // for (int i = 4; i <= 7; ++i)
        // {
        //     mydata[i] = 0;
        // }
        Serial.println("Read Fail node ID=1");
    }
    Serial.println(mydata[5] * 256 + mydata[4]);
    Serial.println(mydata[7] * 256 + mydata[6]);
    delay(1000);
    // return values;
}
// check Parameters from PH china device via Modbus
void checkParameters_PH(int num_InputRegisters, int device_ID, ModbusMaster node)
{
    node.begin(device_ID, Serial3 /*SWSerial0*/);
    Serial.println("*******STM32-Modbus-R485_by_NamNam201*********");
    Serial.println("=====readSensor1======");
    result = node.readHoldingRegisters(1008, num_InputRegisters);
    if (result == node.ku8MBSuccess)
    {
        uint16_t hregs1[num_InputRegisters];
        for (uint8_t i = 0; i < num_InputRegisters; i++)
        {
            hregs1[i] = node.getResponseBuffer(i);
            // Serial.println(hregs1[i]);
        }
        // convert 2x int16 to 32-bit
        uint32_t pH = (hregs1[0] << 0 | hregs1[1] << 16);
        uint32_t temp = (hregs1[2] << 0 | hregs1[3] << 16);

        float pH_val, temp_val;
        memcpy(&pH_val, &pH, sizeof(float));
        memcpy(&temp_val, &temp, sizeof(float));
        Serial.print("Float Value: ");
        Serial.println(pH_val);
        Serial.println(temp_val);

        mydata[4] = int(pH_val * 100) % 256;
        mydata[5] = int(pH_val * 100) / 256;

        mydata[6] = int(temp_val * 10) % 256;
        mydata[7] = int(temp_val * 10) / 256;
    }
    else
    {
        Serial.println("Read Fail node ID=1");
    }
    // Serial.println(mydata[5] * 256 + mydata[4]);
    // Serial.println(mydata[7] * 256 + mydata[6]);
    // delay(100);
    // return values;
}
// Check values of humidity and temperature
void checkParameters_ES35(int num_HoldingRegisters, int device_ID, ModbusMaster node)
{
    node.begin(device_ID, Serial3 /*SWSerial0*/);
    Serial.println("=====readSensor2======");

    result = node.readHoldingRegisters(0, num_HoldingRegisters); // Read Registers 0 -> 5
    if (result == node.ku8MBSuccess)                             // Read success
    {
        mydata[8] = node.getResponseBuffer(0) % 256;
        mydata[9] = node.getResponseBuffer(0) / 256;

        mydata[10] = node.getResponseBuffer(1) % 256;
        mydata[11] = node.getResponseBuffer(1) / 256;
    }
    else
        Serial.println("Read Fail node 1"); // read fail
    Serial.println(mydata[9] * 256 + mydata[8]);
    Serial.println(mydata[11] * 256 + mydata[10]);
    delay(100);
}

void check_lightIntensity(int num_HoldingRegisters, int device_ID, ModbusMaster node)
{
    node.begin(device_ID, Serial3 /*SWSerial0*/);
    result = node.readHoldingRegisters(0, 9); // Read Registers 0 -> 9
    delay(10);
    // Serial.print(result);
    if (result == node.ku8MBSuccess) // Read success
    {
        Serial.println("=====readSensor3======");
        int sumIntensity = node.getResponseBuffer(7) * 65536 + node.getResponseBuffer(8);
        mydata[12] = (sumIntensity - (sumIntensity / 65536)) % 256;
        mydata[13] = (sumIntensity - (sumIntensity / 65536)) / 256;
        mydata[14] = sumIntensity / 65536;
    }
    else
    {
        Serial.print("Read Fail node 3"); // read fail
    }
    Serial.println(mydata[14] * 65536 + mydata[13] * 256 + mydata[12]);
    delay(100);
}

void check_rainFall(int num_HoldingRegisters, int device_ID, ModbusMaster node)
{
    node.begin(device_ID, Serial3 /*SWSerial0*/);
    result = node.readHoldingRegisters(0, 1); // Read Registers 0 -> 5
    delay(10);
    // Serial.print(result);
    if (result == node.ku8MBSuccess) // Read success
    {
        Serial.println("=====readSensor4======");
        mydata[15] = node.getResponseBuffer(0) % 256;
        mydata[16] = node.getResponseBuffer(0) / 256;
    }
    else
    {
        Serial.println("Read Fail node ID=4");
    }
    Serial.println(mydata[16] * 256 + mydata[15]);
    delay(100);
}
// void do_sleep(unsigned int sleepyTime) {
//     unsigned int fours = sleepyTime / 4;
//     unsigned int twos = (sleepyTime % 4) / 2;
//     unsigned int ones = (sleepyTime % 2);

//     for ( int x = 0; x < fours; x++) {
//         if (checkCurrent() && checkWater()) {return;}
//         // put the processor to sleep for 4 seconds
//         LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
//         Serial.print(pulseCount);
//         pulseCount = 0 ;
//     }
//     for ( int x = 0; x < twos; x++) {
//         if (checkCurrent() && checkWater()) {return;}
//         // put the processor to sleep for 2 seconds
//         LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
//     }
//     for ( int x = 0; x < ones; x++) {
//         if (checkCurrent() && checkWater()) {return;}
//         // put the processor to sleep for 1 seconds
//         LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
//     }
//     addMillis(sleepyTime * 1000);
// }

void onEvent(ev_t ev)
{
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        break;
    case EV_JOINED:
        Serial.println(F("EV_JOINED"));
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("AppSKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i)
            {
                if (i != 0)
                    Serial.print("-");
                printHex2(artKey[i]);
            }
            Serial.println("");
            Serial.print("NwkSKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i)
            {
                if (i != 0)
                    Serial.print("-");
                printHex2(nwkKey[i]);
            }
            Serial.println();
        }
        LMIC_setLinkCheckMode(0);
        break;

    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        break;
    case EV_TXCOMPLETE:
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
        if (LMIC.txrxFlags & TXRX_ACK)
            Serial.println(F("Received ack"));
        if (LMIC.dataLen)
        {
            uint8_t result = LMIC.frame[LMIC.dataBeg + 0];
            Serial.print(F("Received "));
            Serial.print(LMIC.dataLen);
            Serial.println(F(" bytes of payload"));
            Serial.println(result);
            // if (result == 1)
            // {
            //     digitalWrite(PC13, HIGH);
            //     digitalWrite(PB15, HIGH);
            //     Serial.println("BAT");
            // }
            // else
            // {
            //     digitalWrite(PC13, LOW);
            //     digitalWrite(PB15, LOW);
            //     Serial.println("TAT");
            // }
            // delay(200);
        }
        // data = digitalRead(PB14);

        // Schedule next transmission
        // os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
        // if (times_send < 5){
        //     times_send += 1;
        //     checkCurrent();
        //     checkWater();
        //     os_setCallback(&sendjob, do_send);
        // }
        // else {
        //     os_setCallback(&sendjob, do_send);
        //     do_sleep(TX_INTERVAL);
        // }
        // int* values ;
        // values = checkParameters_PH6000V(3, 2);
        // if (values != nullptr){
        //     for (int i= 0; i< 3; i++ ){
        //         Serial.println(values[i]);
        //     }
        // }
        checkParameters_PH(4, 5, node[0]);
        // mydata[17] = !digitalRead(metalProximitySensorPin);
        // checkParameters_PH6000V(5, 1, node[0]);
        // checkParameters_ES35(2,2, node[1]);
        // check_lightIntensity(2, 3, node[2]);
        // check_rainFall( 1, 4, node[3]);
        // checkCurrent(4);
        // checkWater();
        os_setCallback(&sendjob, do_send);
        // pulseCount = 0 ;
        break;
    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_SCAN_FOUND:
    ||    Serial.println(F("EV_SCAN_FOUND"));
    ||    break;
    */
    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        break;
    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;

    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.print("\r\n*******STM32-Modbus-R485_by_NamNam201*********");
    pinMode(PC13, OUTPUT);
    // pinMode(PB15, OUTPUT);
    // pinMode(PB14, INPUT);
    // SWSerial0.begin(9600);
    // Serial_Canopus.begin(19200, SERIAL_8N1);

    // //comunication Config stm32 vs Modbus
    // node.begin(1, Serial3);
    // initialization of measure current objects
    Serial3.begin(9600);
    // int devices_ID[3] = {3};
    // for (int i = 0; i < 1; ++i)
    // {
    //     node[i].begin(devices_ID[i], Serial3 /*SWSerial0*/);
    // }
    // int SCT_pins[4] = {pinSCT1, pinSCT2, pinSCT3, pinSCT4};
    // for (int i = 0; i < 4; ++i)
    // {
    //     SCT013[i].current(SCT_pins[i], 60.606);
    // }
    // // attachInterrupt(digitalPinToInterrupt(sensorPin), [](){pulseCount++;}, FALLING);
    // #ifdef VCC_ENABL
    //     // For Pinoccio Scout boards
    //     pinMode(VCC_ENABLE, OUTPUT);
    //     digitalWrite(VCC_ENABLE, HIGH);
    //     delay(1000);
    // #endif

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);

    // Now set up two Tasks to run independently.
    // Khởi tạo FreeRTOS và bắt đầu lập lịch cho các tasks
    // xTaskCreate(Task1, "Task1", 128, NULL, 1, NULL);
    // xTaskCreate(Task2, "Task2", 128, NULL, 1, NULL);
    // vTaskStartScheduler();
}

void loop()
{
    os_runloop_once();
    // checkParameters_PH6000V(3, 1, node[0]);
    // checkParameters_PH(4, 1, node[0]);
    // delay(1000);
    // checkParameters_ES35(2, 2, node[1]);
    // check_rainFall( 1, 4, node[4]);
    // delay(1000);
}
