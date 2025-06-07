#include "modbus/ModbusRS485.h"
#include "lora/LoRaWAN.h"
#include "eeprom/EEPROMConfig.h"
#include "button/button.h"
#include "led/led.h"
#include "config.h"

uint8_t indexPond = 1;
uint8_t modeLora = 1;
lorawanConfig myLora;
uint16_t AddressWrite = 0x10;
uint8_t mode = 0;
bool isJoined = false;
TYPE_SETUP typeSetup = DFU_SETUP;
osjob_t sendjob;

const lmic_pinmap lmic_pins = {
    .nss = PA4,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PB0,
    .dio = {PB8, PB9, LMIC_UNUSED_PIN},
};

void setup()
{
    // delay(3000);
    Serial.begin(9600);
    Serial3.begin(9600);
    led_init();
    pinMode(BUTTON_RESET_EEPROM, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_RESET_EEPROM), handleInterruptButtonReset, CHANGE);

    // Serial.println("STM32 LoRaWAN Start");
    EEPROM_readStruct(AddressWrite, myLora);
    indexPond = myLora.index;
    modeLora = myLora.mode;
    indexPond = (indexPond == 255) ? 0 : indexPond;
    modeLora = (myLora.mode > 3) ? READ_CHINA : myLora.mode;

    if (isNewEEPROM(myLora.APPEUI, LENGTH_APPUI) &&
        isNewEEPROM(myLora.DEVEUI, LENGTH_DEVEUI) &&
        isNewEEPROM(myLora.APPKEY, LENGTH_APPKEY))
    {
        // Serial.println("EEPROM is empty");
        mode = 0;
        typeSetup = DFU_SETUP;
    }
    else
    {
        // Serial.println(indexPond);
        // Serial.println(modeLora);
        led_on(indexPond);
        initLoRa();
        mode = 1;
    }
}

void loop()
{
    if (mode == 1)
    {
        static unsigned long lastTime = 0;
        static bool isLedOn = false;
        if (millis() - lastTime >= 1000)
        {
            lastTime = millis();
            if (isLedOn)
            {
                led_on(indexPond);
                isLedOn = false;
            }
            else
            {
                if (isJoined)
                {
                    led_off(15);
                    isLedOn = true;
                }
            }
        }
        // Enter Run Mode...
        os_runloop_once();
    }
    else
    {
        // Setup Mode
        setUpLora();
    }
}
