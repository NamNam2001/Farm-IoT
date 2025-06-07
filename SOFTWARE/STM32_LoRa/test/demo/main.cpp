#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <MapleFreeRTOS821.h>
#include "config.h"
#include "ModbusMaster.h"
#include "EmonLib.h"

EnergyMonitor SCT013;
#define pinSCT PA0

uint16_t Irms = 0;
// ModbusMaster node[6];
// uint8_t deviceId[] = {1, 2, 3, 4, 5, 6};
// uint8_t result;
void os_getArtEui(u1_t *buf)
{
    memcpy_P(buf, APPEUI, 8);
}
void os_getDevEui(u1_t *buf)
{
    memcpy_P(buf, DEVEUI, 8);
}
void os_getDevKey(u1_t *buf)
{
    memcpy_P(buf, APPKEY, 16);
}

static osjob_t sendjob;
// uint8_t mydata[2];
unsigned char mydata[18];
// uint8_t data;
uint8_t data = 0;

// unsigned char mydata1[]= "Hello NamNam201";

bool sensor_SCT013 = false;
bool previousValue = false;

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

template <typename T>
void data2Bytes(T value, uint8_t *data)
{
    // Serial.println(sizeof(T));
    for (size_t i = 0; i < sizeof(T); i++)
    {
        data[i] = (uint8_t)(value >> (8 * (sizeof(T) - 1 - i)));
    }
}

void do_send(osjob_t *j)
{
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
        }

        Irms = SCT013.calcIrms(1480) * 100;
        data2Bytes(Irms, mydata);
        Serial.print("Dong dien = ");
        Serial.println(Irms * 0.01);
        // checkParameters_PH(4, 1, node[0]);
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
    SCT013.current(pinSCT, 60.606);

    // pinMode(PB15, OUTPUT);
    // pinMode(PB14, INPUT);
    // SWSerial0.begin(9600);
    // Serial_Canopus.begin(19200, SERIAL_8N1);
    // Serial3.begin(9600);
    // for (int i = 0; i < sizeof(deviceId); ++i)
    // {
    //     node[i].begin(deviceId[i], Serial3);
    // }
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
    // delay(1000);
}