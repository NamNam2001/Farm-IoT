#include <WiFi.h>
#include <SHT3x.h>
#include "SH1106.h" // See https://github.com/squix78/esp8266-oled-ssd1306 or via Sketch/Include Library/Manage Libraries
// SH1106 display(0x3c, SDA, SCL); // OLED display object definition (address, SDA, SCL)

// #include "SSD1306.h"     // See https://github.com/squix78/esp8266-oled-ssd1306 or via Sketch/Include Library/Manage Libraries
// #include "SSD1306Wire.h"
#include "SH1106Wire.h"
#define SDA 21
#define SCL 22
#include <ModbusRTU.h>
#include "images.h"

SH1106Wire display(0x3c, SDA, SCL); // OLED display object definition (address, SDA, SCL)
SHT3x Sensor;

const int buttonMode = 34;
const int buttonUp = 32;
const int buttonDown = 35;
const int buttonEnter = 33;

#define MBUS_HW_SERIAL Serial2
#define MBUS_TXD_PIN 16
#define MBUS_RXD_PIN 17
ModbusRTU mb;
uint16_t hregs[20];

bool cb(Modbus::ResultCode event, uint16_t transactionId, void *data)
{ // Callback to monitor errors
   if (event != Modbus::EX_SUCCESS)
   {
      Serial.print("Request result: 0x");
      Serial.print(event, HEX);
   }
   return true;
}

void runRS485(uint8_t address, uint16_t start, uint8_t num, uint16_t *hregs, uint8_t functionCode, uint32_t baudRate)
{
   MBUS_HW_SERIAL.begin(baudRate, SERIAL_8N1, MBUS_RXD_PIN, MBUS_TXD_PIN);
   mb.begin(&MBUS_HW_SERIAL);
   mb.master();
   // while (digitalRead(buttonMode) == LOW)
   // {
   if (!mb.slave())
   {
      if (functionCode == 3)
      {
         mb.readHreg(address, start, hregs, num, cb); // Send Read Hreg from Modbus Server
         Serial.println("3");
      }
      else if (functionCode == 4)
      {
         mb.readIreg(address, start, hregs, num, cb);
         Serial.println("4");
      }
      while (mb.slave())
      { // Check if tran saction is active  mb.task();
         Serial.println("HERE++++++++++");
         mb.task();
         delay(10);
      }
   }
}

enum Mode
{
   MAIN_SCREEN,
   SELECT_MODE,
   READ_RS485,
   SURVEY_WIFI,
   READ_SHT30,
   RUN_RS485,
   CONFIG_RS485
};

Mode currentMode = MAIN_SCREEN;
int selectedMode = 0;
int selectedMode_RS485 = 0;
int selectedParameter = 0;
bool adjustValue = false; // edit config RS485

// parameter RS485
const uint baudRates[] = {9600, 19200, 115200};
int baudRateIndex = -1;
uint baudRate = 9600;
u8_t slaveAddress = 1;
u16_t startRegister = 0;
u8_t numRegisters = 1;
u8_t functionCode = 3;

struct networks
{
   String Name;
   int Signal;
};

networks networks_received[20];
networks networks_sorted[20];

int centerTextX(const String &text)
{
   int screenWidth = 128;
   int textWidth = display.getStringWidth(text);
   return (screenWidth - textWidth) / 2;
}

void get_and_sort_networks(byte available_networks)
{
   // First get all network names and their signal strength
   for (int i = 0; i < available_networks; i++)
   {
      networks_received[i].Name = String(WiFi.SSID(i)); // Display SSID name
      networks_received[i].Signal = WiFi.RSSI(i);       // Display RSSI for that network
   }
   // Now sort in order of signal strength
   String temp_name;
   int temp_signal, iteration = 0;
   while (iteration < available_networks)
   { // Now bubblesort results by RSSI
      for (int i = 1; i < available_networks; i++)
      {
         if (networks_received[i].Signal > networks_received[i - 1].Signal)
         {
            temp_name = networks_received[i - 1].Name;
            temp_signal = networks_received[i - 1].Signal;
            networks_received[i - 1].Name = networks_received[i].Name; // Swap around values
            networks_received[i - 1].Signal = networks_received[i].Signal;
            networks_received[i].Name = temp_name;
            networks_received[i].Signal = temp_signal;
         }
      }
      iteration++;
   }
}

void scan_and_show()
{
   unsigned long previousMillis = 0;
   byte available_networks;
   while (digitalRead(buttonMode) == HIGH)
   {
      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= 15000)
      {
         available_networks = WiFi.scanNetworks(); // Count the number of networks being received
         previousMillis = currentMillis;
      }
      if (available_networks == 0)
      {
         Serial.println("no networks found");
      }
      else
      {
         get_and_sort_networks(available_networks);
         display.clear();
         display.drawString(0, 0, "SSID");
         display.drawString(102, 0, "RSSI");
         display.drawLine(0, 11, 127, 11);
         if (currentMillis - previousMillis <= 14000)
         {
            for (int i = 0; i < available_networks; i++)
            {
               if (i < 6)
               {
                  display.drawString(0, (i + 1) * 8 + 3, networks_received[i].Name + " ");       // Display SSID name
                  display.drawString(110, (i + 1) * 8 + 3, String(networks_received[i].Signal)); // Display RSSI for that network
                  Serial.println(networks_received[i].Name);
                  Serial.println(networks_received[i].Signal);
               }
            }
            display.display();
            // previousMillis = currentMillis;
         }
         else
         {
            if (currentMillis - previousMillis <= 15000 && available_networks > 6)
            {
               for (int i = 0; i < available_networks; i++)
               {
                  if (i == 6)
                  {
                     display.clear();
                     display.drawString(0, 0, "SSID");
                     display.drawString(102, 0, "RSSI");
                     display.drawLine(0, 11, 127, 11);
                  }
                  display.drawString(0, (i + 1 - 6) * 8 + 3, networks_received[i].Name + " ");       // Display SSID name
                  display.drawString(110, (i + 1 - 6) * 8 + 3, String(networks_received[i].Signal)); // Display RSSI for that network
                  Serial.println(networks_received[i].Name);
                  Serial.println(networks_received[i].Signal);
               }
               display.display();
            }
            // else
            // {
            //    previousMillis = currentMillis;
            // }
         }
      }
   }
}

void displayMainScreen()
{
   display.clear();
   display.drawString(centerTextX("Main Screen"), 0, "Main Screen");
   display.drawXbm(0, 0, 128, 64, pozadina);
   display.display();
}

void displaySelectMode()
{
   display.clear();
   display.drawString(centerTextX("Select Mode"), 0, "Select Mode");
   display.drawString(0, 16, (selectedMode == 0 ? "> " : "  ") + String("Read RS485"));
   display.drawString(0, 32, (selectedMode == 1 ? "> " : "  ") + String("Survey WiFi"));
   display.drawString(0, 48, (selectedMode == 2 ? "> " : "  ") + String("Read SHT30"));
   display.display();
}

void displayReadRS485()
{
   display.clear();
   display.drawString(centerTextX("Read RS485"), 0, "Read RS485");
   // Add RS485 reading logic here
   display.drawString(0, 16, selectedMode_RS485 == 0 ? "> Run RS485" : "  Run RS485");
   display.drawString(0, 32, selectedMode_RS485 == 1 ? "> Config RS485" : "  Config RS485");
   display.display();
}

unsigned long previousMillis = 0;
const long interval = 1000;

void displayRunRS485()
{
   unsigned long currentMillis = millis();

   if (currentMillis - previousMillis >= interval)
   {
      previousMillis = currentMillis;
      display.clear();
      display.drawString(centerTextX("Run RS485"), 0, "Run RS485");
      runRS485(slaveAddress, startRegister, numRegisters, hregs, functionCode, baudRate);
      for (u8_t i = 0; i < numRegisters; i++)
      {
         if (i < 5)
         {
            display.drawString(0, (i + 1) * 10, String(i) + ": " + String(hregs[i]));
         }
         else if (5 < i < 10)
         {
            display.drawString(64, (i + 1 - 5) * 10, String(i) + ": " + String(hregs[i]));
         }
         else
         {
            display.drawString(84, (i + 1 - 10) * 10, String(i) + ": " + String(hregs[i]));
         }
      }
      display.display();
   }
}

void displaySurveyWiFi()
{
   // display.clear();
   // display.drawString(centerTextX("Survey WiFi"), 0, "Survey WiFi");
   // // Add WiFi survey logic here
   // display.display();
   while (digitalRead(buttonMode) == HIGH)
   {
      scan_and_show();
   }
}

void displayConfigRS485()
{
   display.clear();
   display.drawString(centerTextX("Config RS485"), 0, "Config RS485");
   display.drawString(0, 10, (selectedParameter == 0 ? "> " : "  ") + String("Baud Rate: ") + (adjustValue == 1 && selectedParameter == 0 ? ">" : "") + String(baudRate));
   display.drawString(0, 20, (selectedParameter == 1 ? "> " : "  ") + String("Slave Address: ") + (adjustValue == 1 && selectedParameter == 1 ? ">" : "") + String(slaveAddress));
   display.drawString(0, 30, (selectedParameter == 2 ? "> " : "  ") + String("Start Register: ") + (adjustValue == 1 && selectedParameter == 2 ? ">" : "") + String(startRegister));
   display.drawString(0, 40, (selectedParameter == 3 ? "> " : "  ") + String("Num Registers: ") + (adjustValue == 1 && selectedParameter == 3 ? ">" : "") + String(numRegisters));
   display.drawString(0, 50, (selectedParameter == 4 ? "> " : "  ") + String("Function Code: ") + (adjustValue == 1 && selectedParameter == 4 ? ">" : "") + String(functionCode));
   display.display();
}

void displayReadSHT30()
{
   display.clear();
   display.drawString(centerTextX("Read SHT30"), 0, "Read SHT30");
   // Add SHT30 reading logic here
   Sensor.UpdateData();
   Serial.print("Temperature: ");
   Serial.print(Sensor.GetTemperature());
   Serial.println("°C");
   Serial.print("Humidity: ");
   Serial.print(Sensor.GetRelHumidity());
   Serial.println("%");
   display.drawString(0, 20, "Temperature: " + String(Sensor.GetTemperature()) + "°C");
   display.drawString(0, 40, "Humidity: " + String(Sensor.GetRelHumidity()) + "%");
   display.display();
}

// const uint8_t theImage[] PROGMEM = {
//   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//   0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00,
//   0x00, 0x00, 0x00, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//   0xF8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x03, 0x00, 0x00,
//   0x00, 0x00, 0x00, 0x00, 0xFE, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//   0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00,
//   0x00, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
//   0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x00,
//   0x00, 0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0,
//   0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x00,
//   0x00, 0x00, 0x7E, 0x00, 0xE0, 0x1F, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x03,
//   0xFC, 0xFF, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0xBF, 0xFF, 0xFF, 0x01, 0x00,
//   0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x0F, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00,
//   0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00,
//   0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00,
//   0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF,
//   0xFF, 0x3F, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00,
//   0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF,
//   0xFF, 0x3F, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00,
//   0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF,
//   0xFF, 0x3F, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00,
//   0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
//   0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x80, 0xFF, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x03, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00,
//   0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x3F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x00,
//   0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0xFE, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x1F, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00,
//   0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x07, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00,
//   0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0xE0, 0xFF, 0xFF,
//   0xFF, 0xFF, 0x01, 0x00, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00,
//   0x00, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF,
//   0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x1F, 0xFE, 0x3F, 0x00, 0x00,
//   0x00, 0x00, 0xFC, 0x03, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
//   0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//   };

void setup()
{
   Serial.begin(115200);
   Sensor.Begin();
   // modbus RS485

   WiFi.mode(WIFI_STA);

   // Initialize buttons as input
   pinMode(buttonMode, INPUT_PULLUP);
   pinMode(buttonUp, INPUT_PULLUP);
   pinMode(buttonDown, INPUT_PULLUP);
   pinMode(buttonEnter, INPUT_PULLUP);

   // Initialize display
   display.init();
   display.flipScreenVertically();    // rotation 180
   display.setContrast(255);          // If you want turn the display contrast down, 255 is maxium and 0 in minimum, in practice about 128 is OK
   display.setFont(ArialMT_Plain_10); // Set the Font size

   // delay(10000);
}

void loop()
{
   // display.drawXbm(10, 10, 60, 60, WiFi_Logo_bits);

   switch (currentMode)
   {
   case MAIN_SCREEN:
      displayMainScreen();
      if (digitalRead(buttonMode) == LOW)
      {
         currentMode = SELECT_MODE;
         selectedMode = 0;
         selectedMode_RS485 = 0;
         selectedParameter = 0;
         while (digitalRead(buttonMode) == LOW)
            ;
      }
      break;
   case SELECT_MODE:
      displaySelectMode();
      if (digitalRead(buttonUp) == LOW)
      {
         selectedMode = (selectedMode < 2) ? (selectedMode + 1) : 0;
         Serial.println(selectedMode);
         delay(200);
      }
      if (digitalRead(buttonDown) == LOW)
      {
         selectedMode = (selectedMode > 0) ? (selectedMode - 1) : 2;
         Serial.println(selectedMode);
         delay(200);
      }
      if (digitalRead(buttonEnter) == LOW)
      {
         if (selectedMode == 0)
         {
            currentMode = READ_RS485;
         }
         else if (selectedMode == 1)
         {
            currentMode = SURVEY_WIFI;
         }
         else
         {
            currentMode = READ_SHT30;
         }
         while (digitalRead(buttonEnter) == LOW)
            ;
      }
      if (digitalRead(buttonMode) == LOW)
      {
         currentMode = MAIN_SCREEN;
         while (digitalRead(buttonMode) == LOW)
            ;
      }
      break;

   case READ_RS485:
      displayReadRS485();
      if (digitalRead(buttonMode) == LOW)
      {
         currentMode = SELECT_MODE;
         while (digitalRead(buttonMode) == LOW)
            ;
      }
      if (digitalRead(buttonUp) == LOW)
      {
         selectedMode_RS485 = (selectedMode_RS485 == 0) ? 1 : 0; // Toggle between 0 and 1
         delay(200);
      }
      if (digitalRead(buttonDown) == LOW)
      {
         selectedMode_RS485 = (selectedMode_RS485 == 0) ? 1 : 0; // Toggle between 0 and 1
         delay(200);
      }
      if (digitalRead(buttonEnter) == LOW)
      {
         currentMode = (selectedMode_RS485 == 0) ? RUN_RS485 : CONFIG_RS485;
         while (digitalRead(buttonEnter) == LOW)
            ;
      }
      break;

   case RUN_RS485:
      displayRunRS485();
      if (digitalRead(buttonMode) == LOW)
      {
         currentMode = READ_RS485;
         while (digitalRead(buttonMode) == LOW)
            ;
      }
      break;

   case CONFIG_RS485:
      displayConfigRS485();
      if (!adjustValue)
      {
         if (digitalRead(buttonUp) == LOW)
         {
            selectedParameter = (selectedParameter < 4) ? (selectedParameter + 1) : 0;
            Serial.println(selectedParameter);
            delay(200);
         }
         if (digitalRead(buttonDown) == LOW)
         {
            selectedParameter = (selectedParameter > 0) ? (selectedParameter - 1) : 4;
            Serial.println(selectedParameter);
            delay(200);
         }
         if (digitalRead(buttonEnter) == LOW)
         {
            adjustValue = true;
            Serial.println(adjustValue);
            while (digitalRead(buttonEnter) == LOW)
               ;
         }
         if (digitalRead(buttonMode) == LOW)
         {
            currentMode = READ_RS485;
            while (digitalRead(buttonMode) == LOW)
               ;
         }
      }
      else
      {
         if (digitalRead(buttonUp) == LOW)
         {
            switch (selectedParameter)
            {
            case 0:
               baudRateIndex = (baudRateIndex < 2) ? baudRateIndex + 1 : 0;
               baudRate = baudRates[baudRateIndex];
               break;
            case 1:
               slaveAddress++;
               break;
            case 2:
               startRegister++;
               break;
            case 3:
               numRegisters++;
               break;
            case 4:
               functionCode = (functionCode < 4) ? functionCode + 1 : 0;
               break;
            }
            delay(200);
         }
         if (digitalRead(buttonDown) == LOW)
         {
            switch (selectedParameter)
            {
            case 0:
               baudRateIndex = (baudRateIndex > 0) ? baudRateIndex - 1 : 2;
               baudRate = baudRates[baudRateIndex];
               break;
            case 1:
               slaveAddress--;
               break;
            case 2:
               startRegister--;
               break;
            case 3:
               numRegisters--;
               break;
            case 4:
               functionCode = (functionCode > 0) ? functionCode - 1 : 4;
               break;
            }
            delay(200);
         }
         if (digitalRead(buttonEnter) == LOW || digitalRead(buttonMode) == LOW)
         {
            adjustValue = false;
            while (digitalRead(buttonMode) == LOW || digitalRead(buttonEnter) == LOW)
               ;
         }
         if (startRegister > 65000)
         {
            startRegister = 1008;
         }
         else if (startRegister == 1009)
         {
            startRegister = 0;
         }
      }

      break;

   case SURVEY_WIFI:
      displaySurveyWiFi();
      if (digitalRead(buttonMode) == LOW)
      {
         currentMode = SELECT_MODE;
         while (digitalRead(buttonMode) == LOW)
            ;
      }
      break;
   case READ_SHT30:
      displayReadSHT30();
      if (digitalRead(buttonMode) == LOW)
      {
         currentMode = SELECT_MODE;
         while (digitalRead(buttonMode) == LOW)
            ;
      }
      delay(100); // Small delay for stability
   }
}

// #include <SHT3x.h>
// #include <SPI.h>
// #include <Wire.h>
// #include "SH1106.h" // See https://github.com/squix78/esp8266-oled-ssd1306 or via Sketch/Include Library/Manage Libraries
// // SH1106 display(0x3c, SDA, SCL); // OLED display object definition (address, SDA, SCL)

// // #include "SSD1306.h"     // See https://github.com/squix78/esp8266-oled-ssd1306 or via Sketch/Include Library/Manage Libraries
// // #include "SSD1306Wire.h"
// #include "SH1106Wire.h"
// #define SDA 21
// #define SCL 22
// SH1106Wire display(0x3c, SDA, SCL); // OLED display object definition (address, SDA, SCL)

// SHT3x Sensor;
// void setup()
// {
//    Serial.begin(9600);
//    Sensor.Begin();

//    display.init();
//    display.flipScreenVertically();    // rotation 180
//    display.setContrast(255);          // If you want turn the display contrast down, 255 is maxium and 0 in minimum, in practice about 128 is OK
//    display.setFont(ArialMT_Plain_10); // Set the Font size
// }

// void loop()
// {
//    display.clear();
//    Sensor.UpdateData();
//    Serial.print("Temperature: ");
//    Serial.print(Sensor.GetTemperature());
//    Serial.println("C");
//    Serial.print("Humidity: ");
//    Serial.print(Sensor.GetRelHumidity());
//    Serial.println("%");
//    display.drawString(0, 0, "Temperature: ");
//    display.drawString(0, 15, String(Sensor.GetTemperature()));
//    display.drawString(0, 30, "C");
//    display.drawString(0, 45, "Humidity: ");
//    display.drawString(0, 60, String(Sensor.GetRelHumidity()));
//    display.drawString(0, 75, "%");
//    display.display();
//    delay(333);
// }
