#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "config.h"
#include <lorawanConfig.h>
#include <ModbusMaster.h>
#include <EEPROM.h>

void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

// static uint8_t mydata[] = "Hello, world!";
ModbusMaster node;
// unsigned char mydata[18];
static osjob_t sendjob;
uint8_t result;

#define NUM_VARIABLES 29
// enum Variables = {VoltageA, VoltageB, VoltageC, CurrentA, CurrentB, CurrentC, PhaseA, PhaseB, PhaseC, PowerFactorA, PowerFactorB, PowerFactorC, ActivePowerA, ActivePowerB, ActivePowerC, TotalActivePower, ReactivePowerA, ReactivePowerB, ReactivePowerC, TotalReactivePower, ApparentPowerA, ApparentPowerB, ApparentPowerC, TotalApparentPower, Temperature, Frequency, ImportEnergy, ImportReactiveEnergy, ImportApparentEnergy};
// uint16_t dataReceived[NUM_VARIABLES];  // data received from power meter
uint8_t mydata[NUM_VARIABLES * 2 + 4]; // data to send (convert to bytes) + 4 bytes for TimeOn

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

template <typename T>
void data2Bytes(T value, uint8_t *data)
{
    // Serial.println(sizeof(T));
    for (size_t i = 0; i < sizeof(T); i++)
    {
        data[i] = (uint8_t)(value >> (8 * (sizeof(T) - 1 - i)));
    }
}

void readParametersPowerMeter(int num_InputRegisters, int device_ID, ModbusMaster node)
{
    node.begin(device_ID, Serial3 /*SWSerial0*/);
    Serial.println("*******STM32-Modbus-R485_by_NamNam201*********");
    Serial.println("=====readParametersPowerMeter======");
    uint8_t result = node.readHoldingRegisters(0, num_InputRegisters);
    Serial.println(result);
    if (result == node.ku8MBSuccess)
    {
        for (int i = 0; i < num_InputRegisters; i += 2)
        {
            uint16_t value[2] = {node.getResponseBuffer(i), node.getResponseBuffer(i + 1)};
            uint32_t valueReceived = (value[0] << 0 | value[1] << 16);
            float valueReceivedFloat;
            memcpy(&valueReceivedFloat, &valueReceived, sizeof(float));
            // dataReceived[i / 2] = (uint16_t)(valueReceivedFloat * 100);
            data2Bytes((uint16_t)(valueReceivedFloat * 10), &mydata[i]);
            // Serial.print("Data received: ");
            // Serial.print(i);
            // Serial.print(" - ");
            // Serial.println(valueReceivedFloat);
            // Serial.print(": ");
            // Serial.print(mydata[i]);
            // Serial.print(" ; ");
            // Serial.println(mydata[i + 1]);
        }
    }
    else
    {
        Serial.println("Read Fail node ID=1");
    }

    // delay(1000);
    // return values;
}

void readTimeOn(int start_Register, int num_InputRegisters, int device_ID, ModbusMaster node)
{
    node.begin(device_ID, Serial3 /*SWSerial0*/);
    Serial.println("*******STM32-Modbus-R485_by_NamNam201*********");
    Serial.println("=====readTimeOn======");
    uint8_t result = node.readHoldingRegisters(start_Register, num_InputRegisters);
    Serial.println(result);
    if (result == node.ku8MBSuccess)
    {
        for (int i = 0; i < num_InputRegisters; i += 2)
        {
            uint16_t value[2] = {node.getResponseBuffer(i), node.getResponseBuffer(i + 1)};
            uint32_t valueReceived = (value[0] << 0 | value[1] << 16);
            data2Bytes(valueReceived, &mydata[start_Register + 4]);
            Serial.print("Data received Time ON: ");
            Serial.print(i + start_Register);
        }
    }
    else
    {
        Serial.println("Read Fail node ID=1");
    }
}

void runReadModbus()
{
    readParametersPowerMeter(NUM_VARIABLES * 2, 1, node);
    // readTimeOn(58, 2, 1, node);
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

        readParametersPowerMeter(NUM_VARIABLES * 2, 1, node);
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
    // delay(3000);
    Serial.begin(9600);
    Serial.println("\r\n*******STM32-Modbus-R485_by_NamNam201*********");
    pinMode(PC13, OUTPUT);
    Serial3.begin(19200);
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop()
{
    os_runloop_once();
    // runReadModbus();
    // delay(1000);
}
