#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <MapleFreeRTOS821.h>
#include <EEPROM.h>

#define MAX_BAR 1
#define resistorValue 153.4
int sensorPressurePin = PA0;

// // stm32-motor-current-7
// static const u1_t PROGMEM APPEUI[8] = {0x54, 0x32, 0x48, 0x28, 0x99, 0x33, 0x84, 0x73}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//     memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x61, 0x88, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//     memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x27, 0xF7, 0xB4, 0x32, 0xB5, 0x11, 0x80, 0x78, 0x4E, 0xBF, 0xD0, 0x9C, 0x2B, 0xC1, 0xCE, 0x8C}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

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

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x15, 0x13, 0x83, 0x2D, 0x6F, 0xDF, 0x02, 0x7B, 0x7B, 0xE0, 0x0D, 0xA0, 0xDA, 0x41, 0x48, 0xD7}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//     memcpy_P(buf, APPKEY, 16);
// }

////stm32-motor-current-4
static const u1_t PROGMEM APPEUI[8] = {0x12, 0x12, 0x12, 0x22, 0x20, 0x20, 0x20, 0x20}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
void os_getArtEui(u1_t *buf)
{
    memcpy_P(buf, APPEUI, 8);
}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = {0xE1, 0x60, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getDevEui(u1_t *buf)
{
    memcpy_P(buf, DEVEUI, 8);
}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = {0x4F, 0xA3, 0x56, 0x12, 0xDD, 0xAA, 0xAB, 0x56, 0xBB, 0x57, 0x80, 0x5C, 0x35, 0xB3, 0xDF, 0x71}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
void os_getDevKey(u1_t *buf)
{
    memcpy_P(buf, APPKEY, 16);
}
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
        // LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
        LMIC_setTxData2(1, &data, sizeof(data), 0);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

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
        digitalWrite(PC13, HIGH);
        break;
    case EV_JOINED:
        digitalWrite(PC13, LOW);
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
            if (result == 1)
            {
                digitalWrite(PC13, HIGH);
                digitalWrite(PB15, HIGH);
                Serial.println("BAT");
            }
            else
            {
                digitalWrite(PC13, LOW);
                digitalWrite(PB15, LOW);
                Serial.println("TAT");
            }
            delay(200);
        }
        data = digitalRead(PB14);

        os_setCallback(&sendjob, do_send);
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
        digitalWrite(PC13, LOW);
        break;

    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

float processPressure(int sensorValue)
{
    float pressure = 0.0;
    // giải phương trình 2 nghiệm để tìm phương trình đường thẳng tuyến tính a=0.0625, b=-0.25
    pressure = (((sensorValue * 3.3 * 1000 * 0.0625 / 4096)) / resistorValue) - 0.25;
    return pressure;
}

static void vReadTask(void *pvParameters)
{
    while (1)
    {
        int sensorValue = analogRead(sensorPressurePin);
        float pressure = processPressure(sensorValue);
        Serial.print("pressure = ");
        Serial.print(pressure);
        Serial.println(" bar");
        if (pressure > 0.08)
        {
            Serial.println("BAT");
            digitalWrite(PC13, HIGH);
        }
        else
        {
            Serial.println("TAT");
            digitalWrite(PC13, LOW);
        }
        vTaskDelay(1000);
    }
}
static void vSendTask(void *pvParameters)
{
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
    while (1)
    {
        os_runloop_once();
    }
}
void setup()
{
    Serial.begin(9600);
    Serial.print("\r\n*******STM32-Modbus-R485_by_NamNam201*********");
    pinMode(PC13, OUTPUT);
    pinMode(PB15, OUTPUT);
    pinMode(PB14, INPUT);

    xTaskCreate(vReadTask,
                "Task1",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);

    xTaskCreate(vSendTask,
                "Task2",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 1,
                NULL);
    vTaskStartScheduler();
}

void loop()
{
}
