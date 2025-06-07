#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "config.h"

#define TEMP_SENSOR_CHANNEL 16 // Cảm biến nhiệt độ là kênh 16
#define VREFINT_CHANNEL 17     // VREFINT là kênh 17

void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

// static uint8_t mydata[] = "Hello, world!";
uint8_t mydata[10];
static osjob_t sendjob;

// enum PinControl
// {
//     PIN_0 = PA8,
//     PIN_1 = PA9,
//     PIN_2 = PA15,
//     PIN_3 = PB3,
//     PIN_4 = PB4,
//     PIN_5 = PB5,
//     PIN_6 = PB5
//     // PIN_7 = PB6
// };

enum RELAY
{
    RELAY_0,
    RELAY_1,
    RELAY_2,
    RELAY_3,
    RELAY_4,
    RELAY_5,
    RELAY_6
};

int pinRelay[] = {PB15, PA8, PA15, PB3, PB4, PB5, PB6};

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
        Serial.println(F("Packet queued"));
    }
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
            uint8_t receivedData[LMIC.dataLen];
            Serial.println("Received Data: ");
            for (int i = 0; i < LMIC.dataLen; i++)
            {
                receivedData[i] = LMIC.frame[LMIC.dataBeg + i];
                Serial.println(receivedData[i]);
            }
            // control relay
            uint8_t data_control = receivedData[0]; // server index from 1
            for (int i = 0; i < 7; i++)
            {
                if (data_control & (1 << i))
                {
                    digitalWrite(pinRelay[i], HIGH);
                }
                else
                {
                    digitalWrite(pinRelay[i], LOW);
                }
            }
            // if (digitalRead(pinRelay[relay_index]) != relay_command)
            // {
            //     digitalWrite(pinRelay[relay_index], relay_command);
            // }
        }
        mydata[0] = random(20, 40);  // Temperature
        mydata[1] = random(50, 100); // Humidity
        mydata[2] = random(0, 255);  // Light intensity
        for (int i = RELAY_0; i <= RELAY_6; i++)
        {
            mydata[i + 3] = digitalRead(pinRelay[i]);
        }
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
    Serial.println(F("Starting"));
    for (int i = RELAY_0; i <= RELAY_6; i++)
    {
        pinMode(pinRelay[i], OUTPUT);
        digitalWrite(pinRelay[i], LOW);
    }
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
    // analogRead(ADC_SQR3_SQ1_4)
}

void loop()
{
    os_runloop_once();
}
