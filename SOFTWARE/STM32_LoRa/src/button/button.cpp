#include "button.h"
#include "EEPROM/EEPROMConfig.h"
#include "config.h"
bool isConfirmConfig = false;
bool isConfirmConfigMode = false;

void handleInterruptButtonReset()
{
    if (mode == 0)
    {
        if (typeSetup == DFU_SETUP && digitalRead(BUTTON_RESET_EEPROM) == HIGH)
        {
            Serial.println("Button released");
            typeSetup = BUTTON_SETUP;
        }
        else if (typeSetup == BUTTON_SETUP || typeSetup == BUTTON_MODE)
        {
            volatile static unsigned long startTime = 0;
            volatile static bool buttonPressed = false;

            if (digitalRead(BUTTON_RESET_EEPROM) == LOW)
            {
                // Serial.println("Button pressed");
                startTime = millis();
                buttonPressed = true;
            }
            else
            {
                if (buttonPressed && millis() - startTime >= HOLD_TIME)
                {
                    Serial.println("Confirm config");
                    Serial.println(millis() - startTime);
                    typeSetup == BUTTON_MODE ? isConfirmConfigMode = true : isConfirmConfig = true;

                    // mode = 1;
                }
                // buttonPressed = false;
            }
        }
    }
    else if (mode == 1)
    {
        volatile static unsigned long startTime = 0;
        volatile static bool buttonPressed = false;

        if (digitalRead(BUTTON_RESET_EEPROM) == LOW)
        {
            // Serial.println("Button pressed");
            startTime = millis();
            buttonPressed = true;
        }
        else
        {
            if (buttonPressed && millis() - startTime >= HOLD_TIME)
            {
                Serial.println("Reset EEPROM");
                Serial.println(millis() - startTime);
                // EEPROM.format();
                mode = 0;
            }
            // buttonPressed = false;
        }
    }
}
