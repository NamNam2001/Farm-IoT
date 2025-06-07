#include <Wire.h>
#include <LiquidCrystal_I2C_STM32.h>
#include <SPI.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C_STM32 lcd(0x27, 16, 2);

void setup()
{
    // initialize the LCD
    lcd.begin();

    // Turn on the blacklight and print a message.
    lcd.backlight();
    lcd.print("Hello, world!");
}
int i = 0;
void loop()
{
    lcd.setCursor(0, 1);
    lcd.print(i++);
    delay(1000);
}
