#ifdef ESP8266
#include <ESP8266WiFi.h>
#define SDA D3
#define SCL D4
#else
#include <WiFi.h>
#define SDA 5
#define SCL 4
#endif

#include "SH1106.h"             // See https://github.com/squix78/esp8266-oled-ssd1306 or via Sketch/Include Library/Manage Libraries
SH1106 display(0x3c, SDA, SCL); // OLED display object definition (address, SDA, SCL)

// #include "SSD1306.h"             // See https://github.com/squix78/esp8266-oled-ssd1306 or via Sketch/Include Library/Manage Libraries
// SSD1306 display(0x3c, SDA, SCL); // OLED display object definition (address, SDA, SCL)

struct networks
{
  String Name;
  int Signal;
};

networks networks_received[20];
networks networks_sorted[20];

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

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  // Wire.begin(SDA, SCL);
  display.init();
  display.flipScreenVertically();    // rotation 180
  display.setContrast(255);          // If you want turn the display contrast down, 255 is maxium and 0 in minimum, in practice about 128 is OK
  display.setFont(ArialMT_Plain_10); // Set the Font size
}

void loop()
{
  byte available_networks = WiFi.scanNetworks(); // Count the number of networks being received
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
    for (int i = 0; i < available_networks; i++)
    {
      if (i < 6)
      {
        display.drawString(0, (i + 1) * 8 + 3, networks_received[i].Name + " ");       // Display SSID name
        display.drawString(110, (i + 1) * 8 + 3, String(networks_received[i].Signal)); // Display RSSI for that network
        Serial.println(networks_received[i].Name);
        Serial.println(networks_received[i].Signal);
        display.display();
      }
      else
      {
        if (i == 6)
        {
          delay(10000);
          display.clear();
          display.drawString(0, 0, "SSID");
          display.drawString(102, 0, "RSSI");
          display.drawLine(0, 11, 127, 11);
        }
        display.drawString(0, (i + 1 - 6) * 8 + 3, networks_received[i].Name + " ");       // Display SSID name
        display.drawString(110, (i + 1 - 6) * 8 + 3, String(networks_received[i].Signal)); // Display RSSI for that network
        Serial.println(networks_received[i].Name);
        Serial.println(networks_received[i].Signal);
        display.display();
      }
    }
    delay(3000);
  }
  delay(200);
}
