#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "config.h"
#include <lorawanConfig.h>
#include <ModbusMaster.h>
#include <EEPROM.h>

// #define LENGTH_CONFIG 66
lorawanConfig myLora;
uint16_t AddressWrite = 0x10;
uint8_t mode = 0; // setup mode: 0 , run lora: 1

void os_getArtEui(u1_t *buf) { memcpy_P(buf, myLora.APPEUI, 8); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, myLora.DEVEUI, 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, myLora.APPKEY, 16); }

static uint8_t mydata[] = "Hello, world!";
ModbusMaster node;
// uint8_t mydata[10];
static osjob_t sendjob;
uint8_t result;

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
        LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
        Serial.println(F("Packet queued"));
    }
}

void checkParameters_PD6000(int num_InputRegisters, int device_ID, ModbusMaster node)
{
    node.begin(device_ID, Serial3 /*SWSerial0*/);
    Serial.println("*******STM32-Modbus-R485_by_NamNam201*********");
    Serial.println("=====readSensor1======");
    result = node.readInputRegisters(0, num_InputRegisters);
    Serial.println(result);
    if (result == node.ku8MBSuccess)
    {
        // for (uint8_t i = 0; i < num_InputRegisters; i++)
        // {
        //     int values = node.getResponseBuffer(i);
        //     Serial.println(values);
        // }
        mydata[4] = node.getResponseBuffer(0) % 256;
        mydata[5] = node.getResponseBuffer(0) / 256;

        mydata[6] = node.getResponseBuffer(4) % 256;
        mydata[7] = node.getResponseBuffer(4) / 256;

        mydata[8] = int((1.25 * node.getResponseBuffer(3) - 500)) % 256;
        mydata[9] = int(1.25 * node.getResponseBuffer(3) - 500) / 256;
        Serial.println(mydata[5] * 256 + mydata[4]);
        Serial.println(mydata[7] * 256 + mydata[6]);
        Serial.println(mydata[9] * 256 + mydata[8]);
    }
    else
    {
        // for (int i = 4; i <= 7; ++i)
        // {
        //     mydata[i] = 0;
        // }
        Serial.println("Read Fail node ID=1");
    }

    // delay(1000);
    // return values;
}

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

        // checkParameters_PH(4, 5, node);
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

void convertToMSB(const char *hexString, uint8_t *output, int length)
{
    for (int i = 0; i < length; i++)
    {
        sscanf(hexString + i * 2, "%2hhX", &output[i]);
    }
}

void convertToLSB(const char *hexString, uint8_t *output, int length)
{
    for (int i = 0; i < length; i++)
    {
        sscanf(hexString + (length - i - 1) * 2, "%2hhX", &output[i]);
    }
}

bool checkInputValid(bool &isReceived)
{
    if (Serial.available() > 0)
    {
        delay(10);
        isReceived = true;
        if (Serial.available() == LENGTH_CONFIG)
        {
            Serial.println("Okeeeeee");
            return true;
        }
        else
        {
            Serial.println("Serial.available(): ");
            Serial.println(Serial.available());
            Serial.println("Input data is invalid");
            return false;
        }
    }
    isReceived = false;
    return false;
}

bool handleReceivedDataInput(bool &isReceived, bool &isSet, bool &isValidData, char *receivedChar)
{
    isValidData = checkInputValid(isReceived);
    if (isReceived && isValidData) // nhận được data và data hợp lệ
    {
        while (Serial.available() > 0)
        {
            uint indexChar = Serial.available();
            char cmd = (char)Serial.read();

            receivedChar[LENGTH_CONFIG - indexChar] = cmd;
        }
        isReceived = false;
        return true;
    }
    else if (isReceived && !isValidData) // nhận được data nhưng data ko hợp lệ
    {
        while (Serial.available() > 0)
        {
            Serial.read();
        }
        isSet = true;
        isReceived = false;
    }
    return false;
}

// In kết quả dưới dạng hex
void printHex(uint8_t *data, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (i > 0)
        {
            Serial.print(", ");
        }
        Serial.print("0x");
        if (data[i] < 0x10)
            Serial.print("0");
        Serial.print(data[i], HEX);
    }
    Serial.println();
}

void saveConfig(char *receivedChar)
{
    char *appUI = strtok(receivedChar, "-");
    char *devUI = strtok(NULL, "-");
    char *appKey = strtok(NULL, "-");
    Serial.println(appUI);
    Serial.println(devUI);
    Serial.println(appKey);
    convertToLSB(appUI, myLora.APPEUI, LENGTH_APPUI);
    convertToLSB(devUI, myLora.DEVEUI, LENGTH_DEVEUI);
    convertToMSB(appKey, myLora.APPKEY, LENGTH_APPKEY);
    Serial.println("Save config done");
    // In kết quả
    Serial.print("APPEUI: ");
    for (int i = 0; i < 8; i++)
    {
        Serial.print(myLora.APPEUI[i], HEX);
    }
    Serial.println();
    Serial.print("DEVEUI: ");
    for (int i = 0; i < 8; i++)
    {
        Serial.print(myLora.DEVEUI[i], HEX);
    }
    Serial.println();
    Serial.print("APPKEY: ");
    for (int i = 0; i < 16; i++)
    {
        Serial.print(myLora.APPKEY[i], HEX);
    }
    EEPROM_writeStruct(AddressWrite, myLora);
}
void setUpLora()
{
    static bool isSet = true;
    static bool isReceived = false;
    static bool isValidData = false;
    char receivedChar[LENGTH_CONFIG] = {0}; // 1 byte for null string
    // String receivedConfig = "";
    while (!Serial)
    {
        digitalWrite(PC13, digitalRead(PC13) ^ 1);
        delay(500);
        isSet = true;
    };
    if (isSet)
    {
        Serial.println("*******Wecome to setup LoRaWan by NamNam201*********");
        Serial.print("Please enter your code here: ");
        isSet = false;
    }

    bool statusHandleInput = handleReceivedDataInput(isReceived, isSet, isValidData, receivedChar);
    if (statusHandleInput)
    {
        saveConfig(receivedChar);
        nvic_sys_reset();
    }
}

bool isNewEEPROM(const uint8_t *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (data[i] != 0xFF)
        {
            return false; // EEPROM is not empty if any byte is not 0xFF
        }
    }
    return true; // EEPROM is empty if all bytes are 0xFF
}

void runSystem()
{
    switch (mode)
    {
    case 1:
        os_runloop_once();
        break;
    default:
        setUpLora();
        break;
    }
}
void setup()
{
    Serial.begin(9600);
    pinMode(PC13, OUTPUT);
    Serial3.begin(9600);
    while (!Serial)
        ;
    Serial.println("\r\n*******STM32-LORAWAN_by_NamNam201*********");
    // memset(&myLora, 0, sizeof(lorawanConfig));
    // EEPROM.format();
    EEPROM_readStruct(AddressWrite, myLora);

    if (isNewEEPROM(myLora.APPEUI, 8) &&
        isNewEEPROM(myLora.DEVEUI, 8) &&
        isNewEEPROM(myLora.APPKEY, 16))
    {
        Serial.println("EEPROM is empty");
        mode = 0;
    }
    else
    {
        // LMIC init
        os_init();
        // Reset the MAC state. Session and pending data transfers will be discarded.
        LMIC_reset();
        // Start job (sending automatically starts OTAA too)
        do_send(&sendjob);
        mode = 1;
        Serial.println("EEPROM read ok");
    }
    // Serial.print("APPEUI: ");
    // for (int i = 0; i < 8; i++)
    // {
    //     Serial.print(myLora.APPEUI[i], HEX);
    // }
    // Serial.println();
    // Serial.print("DEVEUI: ");
    // for (int i = 0; i < 8; i++)
    // {
    //     Serial.print(myLora.DEVEUI[i], HEX);
    // }
    // Serial.println();
    // Serial.print("APPKEY: ");
    // for (int i = 0; i < 16; i++)
    // {
    //     Serial.print(myLora.APPKEY[i], HEX);
    // }
    // Serial.println("End");
    // mode = 0;
}

void loop()
{
    // os_runloop_once();
    // Serial.println("======start======");
    // checkParameters_PH(4, 1, node);
    // checkParameters_PD6000(5, 1, node);
    // delay(1000);
    runSystem();
}
