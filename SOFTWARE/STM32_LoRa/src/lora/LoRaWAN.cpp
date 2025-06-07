#include "LoRaWANConfig.h"
#include "eeprom/EEPROMConfig.h"
#include "modbus/ModbusRS485.h"
#include "led/led.h"
#include "config.h"
#include "button/button.h"
extern lorawanConfig myLora;
extern uint16_t AddressWrite;
extern osjob_t sendjob;

const uint8_t numConfigs = sizeof(predefinedConfigs) / sizeof(predefinedConfigs[0]);
void os_getArtEui(u1_t *buf) { memcpy_P(buf, myLora.APPEUI, LENGTH_APPUI); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, myLora.DEVEUI, LENGTH_DEVEUI); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, myLora.APPKEY, LENGTH_APPKEY); }

// const lmic_pinmap lmic_pins = {
//     .nss = PA4,
//     .rxtx = LMIC_UNUSED_PIN,
//     .rst = PB0,
//     .dio = {PB8, PB9, LMIC_UNUSED_PIN},
// };

void do_send(osjob_t *j)
{
    if (LMIC.opmode & OP_TXRXPEND)
    {
        // Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        // static uint8_t mydata[] = "Hello, world!";
        LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
        // Serial.println(F("Packet queued"));
    }
}

void initLoRa()
{
    // Serial.println("LoRa init");
    os_init();
    // Serial.println("LoRa init 1");
    LMIC_reset();
    // Serial.println("LoRa reset");
    do_send(&sendjob);
    // Serial.println("LoRa init done");
}

void printHex2(unsigned v)
{
    v &= 0xff;
    // if (v < 16)
    // Serial.print('0');
    // Serial.print(v, HEX);
}

void onEvent(ev_t ev)
{
    // Serial.print(os_getTime());
    // Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        // Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        // Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        // Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        // Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        // Serial.println(F("EV_JOINING"));
        break;
    case EV_JOINED:
        // Serial.println(F("EV_JOINED"));
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            // Serial.print("netid: ");
            // Serial.println(netid, DEC);
            // Serial.print("devaddr: ");
            // Serial.println(devaddr, HEX);
            // Serial.print("AppSKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i)
            {
                if (i != 0)
                    // Serial.print("-");
                    printHex2(artKey[i]);
            }
            // Serial.println("");
            // Serial.print("NwkSKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i)
            {
                if (i != 0)
                    // Serial.print("-");
                    printHex2(nwkKey[i]);
            }
            // Serial.println();
        }
        isJoined = true;
        LMIC_setLinkCheckMode(0);
        break;

    case EV_JOIN_FAILED:
        // Serial.println(F("EV_JOIN_FAILED"));
        isJoined = false;
        break;
    case EV_REJOIN_FAILED:
        // Serial.println(F("EV_REJOIN_FAILED"));
        isJoined = false;
        break;
    case EV_TXCOMPLETE:
        // Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
        if (LMIC.txrxFlags & TXRX_ACK)
            // Serial.println(F("Received ack"));
            if (LMIC.dataLen)
            {
                uint8_t result = LMIC.frame[LMIC.dataBeg + 0];
                // Serial.print(F("Received "));
                // Serial.print(LMIC.dataLen);
                // Serial.println(F(" bytes of payload"));
                // Serial.println(result);
                for (int i = 0; i < LMIC.dataLen; i++)
                {
                    // Serial.print(LMIC.frame[LMIC.dataBeg + i]);
                    // Serial.print(" ");
                }
                // Serial.println();
            }

        // checkParameters_PH(4, 5, node);
        if (myLora.mode == READ_CHINA)
        {
            checkParameters_PH(4, 5);
        }
        else if (myLora.mode == READ_LEADTED)
        {
            checkParameters_PH6000V(5, 1);
        }
        else if (myLora.mode == READ_LEADTED_DO)
        {
            checkParameters_PD6000(5, 6);
        }
        os_setCallback(&sendjob, do_send);
        break;
    case EV_LOST_TSYNC:
        // Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        // Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        // Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        // Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        // Serial.println(F("EV_LINK_ALIVE"));
        break;
    case EV_TXSTART:
        // Serial.println(F("EV_TXSTART"));
        break;
    case EV_TXCANCELED:
        // Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        // Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;

    default:
        // Serial.print(F("Unknown event: "));
        // Serial.println((unsigned)ev);
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
            // Serial.println("Data input is valid");
            return true;
        }
        else
        {
            // Serial.print(Serial.available());
            // Serial.println(" character");
            // Serial.println("Input data is invalid");
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
        };
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
    // Serial.println("Save config done");
    // In kết quả
    // Serial.print("APPEUI: ");
    for (int i = 0; i < 8; i++)
    {
        // Serial.print(myLora.APPEUI[i], HEX);
    }
    // Serial.println();
    // Serial.print("DEVEUI: ");
    for (int i = 0; i < 8; i++)
    {
        // Serial.print(myLora.DEVEUI[i], HEX);
    }
    // Serial.println();
    // Serial.print("APPKEY: ");
    for (int i = 0; i < 16; i++)
    {
        // Serial.print(myLora.APPKEY[i], HEX);
    }
    // Serial.println();
    EEPROM_writeStruct(AddressWrite, myLora);
}

bool checkInputDataDfu(String data, String &appui, String &deveui, String &appkey)
{
    int firstDash = data.indexOf('-');
    int secondDash = data.indexOf('-', firstDash + 1);

    // Kiểm tra xem có đủ 2 dấu '-' không
    if (firstDash == -1 || secondDash == -1)
    {
        // Serial.println("Chuoi khong hop le: Khong tim thay 2 dau '-'!");
        return false;
    }

    // division to 3 parts
    String part1 = data.substring(0, firstDash);
    String part2 = data.substring(firstDash + 1, secondDash);
    String part3 = data.substring(secondDash + 1);

    // Kiểm tra độ dài của từng phần
    if (part1.length() != 16)
    {
        // Serial.println("Chuoi khong hop le: Phan 1 khong dung 16 ky tu!");
        return false;
    }

    if (part2.length() != 16)
    {
        // Serial.println("Chuoi khong hop le: Phan 2 khong dung 16 ky tu!");
        return false;
    }

    if (part3.length() != 32)
    {
        // Serial.println("Chuoi khong hop le: Phan 3 khong dung 32 ky tu!");
        return false;
    }
    // Serial.println("Chuoi hop le!");
    appui = part1;
    deveui = part2;
    appkey = part3;
    return true;
}

void convertToLSB1(String hexString, uint8_t *outputArray, int length)
{
    if (hexString.length() != length * 2)
    {
        // Serial.println("Error: Incorrect length");
        return;
    }

    for (int i = 0; i < length; i++)
    {
        String byteString = hexString.substring((length - 1 - i) * 2, (length - i) * 2);
        outputArray[i] = (uint8_t)strtoul(byteString.c_str(), NULL, 16);
    }
}

void convertToMSB1(String hexString, uint8_t *outputArray, int length)
{
    if (hexString.length() != length * 2)
    {
        // Serial.println("Error: Incorrect length");
        return;
    }

    for (int i = 0; i < length; i++)
    {
        String byteString = hexString.substring(i * 2, (i + 1) * 2);
        outputArray[i] = (uint8_t)strtoul(byteString.c_str(), NULL, 16);
    }
}

void processDataDfu()
{
    if (Serial.available() > 0)
    {
        String data = Serial.readStringUntil('\n');
        String appui, deveui, appkey;
        // Serial.println("Received: " + data);
        if (checkInputDataDfu(data, appui, deveui, appkey))
        {
            // Serial.println("Data is valid");
            convertToLSB1(appui, myLora.APPEUI, LENGTH_APPUI);
            convertToLSB1(deveui, myLora.DEVEUI, LENGTH_DEVEUI);
            convertToMSB1(appkey, myLora.APPKEY, LENGTH_APPKEY);
            for (int i = 0; i < numConfigs; i++)
            {
                if (predefinedConfigs[i] == data)
                {
                    myLora.index = i + 1;
                }
                else
                {
                    // Serial.println("Config not found");
                    myLora.index = 0;
                }
            }
            // Serial.println("Save config done");
            EEPROM.format();
            EEPROM_writeStruct(AddressWrite, myLora);
            delay(1000);
            nvic_sys_reset();
        };
    }
}

void setUpLora()
{
    if (typeSetup == DFU_SETUP)
    {
        static bool isSet = true;
        static unsigned long startTime = 0;
        if (millis() - startTime >= 250)
        {
            static bool stateLed = false;
            (stateLed = !stateLed) ? led_off(15) : led_on(15);
            startTime = millis();
        }
        if (!Serial)
        {
            isSet = true;
        };
        if (isSet)
        {
            // Serial.println("*******Wecome to setup LoRaWan by NamNam201*********");
            // Serial.print("Please enter your code here: ");
            isSet = false;
        }
        processDataDfu();
    }

    else if (typeSetup == BUTTON_SETUP)
    {

        static bool lastButtonState = HIGH;
        static bool currentButtonState = HIGH;
        static bool isSet = false;

        if (!isSet)
        {
            led_on(indexPond);
        }
        currentButtonState = digitalRead(BUTTON_RESET_EEPROM);

        // Xử lý nhấn nút
        if (currentButtonState == HIGH && lastButtonState == LOW)
        {
            isSet = true;
            if (isConfirmConfig)
            {
                // Serial.println("Confirm config2");
                String appui, deveui, appkey;
                // Serial.println("Change Config: " + predefinedConfigs[indexPond - 1]);
                if (checkInputDataDfu(predefinedConfigs[indexPond - 1], appui, deveui, appkey))
                {
                    // Serial.println("Data button is valid");
                    convertToLSB1(appui, myLora.APPEUI, LENGTH_APPUI);
                    convertToLSB1(deveui, myLora.DEVEUI, LENGTH_DEVEUI);
                    convertToMSB1(appkey, myLora.APPKEY, LENGTH_APPKEY);
                    myLora.index = indexPond;
                    // Serial.println("Save config button done");
                    // EEPROM.format();
                    EEPROM_writeStruct(AddressWrite, myLora);
                    isSet = false;
                    // blink led 3 times
                    for (int i = 0; i < 3; i++)
                    {
                        led_on(indexPond);
                        delay(200);
                        led_off(indexPond);
                        delay(200);
                    }
                    typeSetup = BUTTON_MODE;
                    // delay(1000);
                    // nvic_sys_reset();
                };
            }
            else
            {
                // Serial.println("Button released");
                indexPond++;
                if (indexPond > numConfigs || indexPond >= 16)
                {
                    indexPond = 1;
                }
                led_on(indexPond);
                // Serial.println(indexPond);
            }
        }
        lastButtonState = currentButtonState;
    }

    else if (typeSetup == BUTTON_MODE)
    {
        static bool lastButtonState = HIGH;
        static bool currentButtonState = HIGH;
        static bool isSet = false;

        if (!isSet)
        {
            led_on(modeLora);
        }
        currentButtonState = digitalRead(BUTTON_RESET_EEPROM);

        // Xử lý nhấn nút
        if (currentButtonState == HIGH && lastButtonState == LOW)
        {
            if (isConfirmConfigMode)
            {
                // Serial.println("Confirm config mode");
                myLora.mode = modeLora;
                // Serial.println("Change Mode: " + String(myLora.mode));
                EEPROM.format();
                EEPROM_writeStruct(AddressWrite, myLora);
                for (int i = 0; i < 3; i++)
                {
                    led_on(modeLora);
                    delay(200);
                    led_off(modeLora);
                    delay(200);
                }
                delay(1000);
                nvic_sys_reset();
            }
            else
            {
                // Serial.println("Button released mode");
                modeLora++;
                if (modeLora > 3)
                {
                    modeLora = 1;
                }
                led_on(modeLora);
                // Serial.println(modeLora);
            }
        }
        lastButtonState = currentButtonState;
    }
}
