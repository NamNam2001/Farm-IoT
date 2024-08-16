// have Config Things

#include "thingProperties.h"
#include "ModbusRTU.h"
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "settings.h"
#include "EEPROM.h"
#include <WebServer.h>
#include "ota_update.h"
#include "ArduinoJson.h"
#include "list_device.h"

// #define REG 0       // thanh ghi bắt đầu
// #define REG_NUM 9   // số lượng thanh ghi cần lấy
// #define SLAVE_ID1 3 // địa chỉ ID slave 1
// #define SLAVE_ID2 2 // địa chỉ ID slave 2
#define BAUDRATE 9600
#define MBUS_HW_SERIAL Serial2

#define MBUS_TXD_PIN 16
#define MBUS_RXD_PIN 17

#define touchPin 15 // Clear EEPROM
#define clearPin 0  // Clear EEPROM
#define delayMillis(t)                            \
  for (static unsigned long _lasttime = millis(); \
       (millis() - _lasttime) >= (t);             \
       _lasttime += (t))

ModbusRTU mb;

// WIFI default
String ssid[4] = {"HEDOTUDONG", "THIEN AN PHU", "RAK7240_AB22", "HTR"};
String password[4] = {"1234567890", "1234567890", "12345678", "huetronics2023"};

// String FirmwareVer = {"0.0"};
byte tries = 0;
// #define URL_fw_Version "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/Version.txt"
// #define URL_fw_Bin "https://raw.githubusercontent.com/NamNam2001/IoT_Farm/master/ArduinoIoTCloud_ConfigWeb.ino.bin"

xSemaphoreHandle xMutex;
Modbus::ResultCode err;
Modbus::ResultCode readSync(uint8_t address, uint16_t start, uint16_t num, uint16_t *buf)
{
  xSemaphoreTake(xMutex, portMAX_DELAY); // đồng bộ hoá sự kiện giữa các task -> bắt đầu
  if (mb.slave())
  {
    xSemaphoreGive(xMutex); // đồng bộ hoá sự kiện giữa các task-> kết thúc
    return Modbus::EX_GENERAL_FAILURE;
  }
  // Serial.printf("SlaveID: %d Hreg %d\r\n", address, start);
  mb.readHreg(address, start, buf, num, [](Modbus::ResultCode event, uint16_t, void *) // thực hiện funtion 4 theo tiêu chuẩn giao thức modbusRTU
              {
    err = event;
    return true; });
  while (mb.slave())
  {
    vTaskDelay(100); // thời gian đợi kết thúc gói truyền của từng ID *2 để ra thời gian của toàn bộ 1 gói tin
    mb.task();
  }
  Modbus::ResultCode res = err;
  xSemaphoreGive(xMutex);
  return res;
}

Modbus::ResultCode readSync1(uint8_t address, uint16_t start, uint16_t num, uint16_t *buf)
{
  xSemaphoreTake(xMutex, portMAX_DELAY); // đồng bộ hoá sự kiện giữa các task -> bắt đầu
  if (mb.slave())
  {
    xSemaphoreGive(xMutex); // đồng bộ hoá sự kiện giữa các task-> kết thúc
    return Modbus::EX_GENERAL_FAILURE;
  }
  // Serial.printf("SlaveID: %d Hreg %d\r\n", address, start);
  mb.readIreg(address, start, buf, num, [](Modbus::ResultCode event, uint16_t, void *) // thực hiện funtion 4 theo tiêu chuẩn giao thức modbusRTU
              {
    err = event;
    return true; });
  while (mb.slave())
  {
    vTaskDelay(100);
    mb.task();
  }
  Modbus::ResultCode res = err;
  xSemaphoreGive(xMutex);
  return res;
}

WebServer server(80);
settings user = {};

WiFiClientSecure client;

void handleScan(JsonObject obj)
// {
//   String networks = "";
//   int8_t numNetworks;
//   {

//     numNetworks = WiFi.scanNetworks();
//     if (numNetworks == 0)
//     {
//       networks += "<p>No networks found</p>";
//     }
//     else
//     {
//       networks += "<table style='width: " + String("100%") + ";'><tr><th>SSID</th><th>(RSSI)</th></tr>";
//       Serial.println(numNetworks);
//       for (int8_t i = 0; i < numNetworks; i++)
//       {
//         networks += "<tr><td>" + WiFi.SSID(i) + "</td><td>" + String(WiFi.RSSI(i)) + " dBm</td></tr>";
//       }
//       networks += "</table>";
//       server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center} h3{text-align: center} th{text-align: left}</style> </head> <body><main class='form-signin'> <form action='/' method='post'> <h1 class=''>Wifi Manager</h1> <div> <h3>Scan Wifi</h3>" + networks + " </div> <h3>Configure Wifi</h3> <div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'></div><br/><br/><button type='submit'>Save</button><p style='text-align: right'><a href='https://huetronics.vn/' style='color: #32C5FF'>huetronics.vn</a></p></form></main> </body></html>");
//     }
//   }
// }
{
  String networks = "";
  int8_t numNetworks;

  String options_device = "";

  {
    // if (user.ssid[0] == '\0')
    // {
    //   numNetworks = scanWiFiNetworks();
    // }
    // else
    // {
    //   numNetworks = wifi_scanned.num_netWorks;
    // }
    // numNetworks = scanWiFiNetworks();
    numNetworks = WiFi.scanNetworks();
    if (numNetworks == 0)
    {
      networks += "<p>No networks found</p>";
    }
    else
    {
      networks += "<table style='width: " + String("100%") + ";'><tr><th>SSID</th><th>(RSSI)</th></tr>";
      Serial.println(numNetworks);
      for (int8_t i = 0; i < numNetworks; i++)
      {
        // if (user.ssid[0] == '\0')
        // {
        //   networks += "<tr><td>" + WiFi.SSID(i) + "</td><td>" + String(WiFi.RSSI(i)) + " dBm</td></tr>";
        // }
        // else
        // {
        //   networks += "<tr><td>" + wifi_scanned.ssid[i] + "</td><td>" + String(wifi_scanned.rssi[i]) + " dBm</td></tr>";
        // }
        networks += "<tr><td>" + WiFi.SSID(i) + "</td><td>" + String(WiFi.RSSI(i)) + " dBm</td></tr>";
        // networks += "<tr><td>" + wifi_scanned.ssid[i] + "</td><td>" + String(wifi_scanned.rssi[i]) + " dBm</td></tr>";
      }
      networks += "</table>";

      options_device += "<div><br><label>Things </label><select id=\"things\" name=\"things\">";

      for (JsonPair kv : obj)
      {
        options_device += "<option value=\"";
        options_device += kv.key().c_str();
        options_device += "\">";
        options_device += kv.key().c_str();
        options_device += "</option>";
      }
      options_device += "</select></div></body></html>";

      server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title> <style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{cursor: pointer;border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1{text-align: center} h3{text-align: center} th{text-align: left}</style> </head> <body><main class='form-signin'> <form action='/' method='post'> <h1 class=''>Wifi Manager</h1> <div> <h3>Scan Wifi</h3>" + networks + " </div> <h3>Configure Wifi</h3> <div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br/><label>Password</label><input type='password' class='form-control' name='password'>" + options_device + "</div><br/><br/><button type='submit'>Save</button><p style='text-align: right'><a href='https://huetronics.vn/' style='color: #32C5FF'>huetronics.vn</a></p></form></main> </body></html>");
    }
  }
}

void handlePortal()
// {
//   if (server.method() == HTTP_POST)
//   {

//     strncpy(user.ssid, server.arg("ssid").c_str(), sizeof(user.ssid));
//     strncpy(user.password, server.arg("password").c_str(), sizeof(user.password));
//     user.ssid[server.arg("ssid").length()] = user.password[server.arg("password").length()] = '\0';
//     user.mode = 1;
//     EEPROM.put(0, user);
//     EEPROM.commit();
//     Serial.println(user.ssid);
//     Serial.println(user.password);
//     Serial.println(user.mode);

//     server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Manager</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!<br />Please restart the device.</p></main></body></html>");
//     ESP.restart();
//   }
//   else
//   {
//     handleScan();
//   }
// }
{
  char things[15];
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, list_device);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  // Get each device and create option tags
  JsonObject obj = doc.as<JsonObject>();
  if (server.method() == HTTP_POST)
  {

    strncpy(user.ssid, server.arg("ssid").c_str(), sizeof(user.ssid));
    strncpy(user.password, server.arg("password").c_str(), sizeof(user.password));
    strncpy(things, server.arg("things").c_str(), sizeof(things));
    user.ssid[server.arg("ssid").length()] = user.password[server.arg("password").length()] = things[server.arg("things").length()] = '\0';
    Serial.print("board_id: ");
    Serial.println(obj[things]["board_id"].as<String>());
    Serial.print("device_key: ");
    Serial.println(obj[things]["device_key"].as<String>());
    strncpy(user.board_id, obj[things]["board_id"].as<const char *>(), sizeof(user.board_id) - 1);
    user.board_id[sizeof(user.board_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
    strncpy(user.device_key, obj[things]["device_key"].as<const char *>(), sizeof(user.device_key) - 1);
    user.device_key[sizeof(user.device_key) - 1] = '\0'; // Đảm bảo kết thúc chuỗi

    // user.board_id = obj[things]["board_id"].as<String>();
    // user.device_key = obj[things]["board_id"].as<String>();
    strncpy(user.things, things, sizeof(user.things) - 1);
    user.things[sizeof(user.things) - 1] = '\0';
    user.mode = 1;
    EEPROM.put(0, user);
    EEPROM.commit();
    Serial.println(user.ssid);
    Serial.println(user.password);
    Serial.println(user.mode);

    server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Manager</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!<br />Please restart the device.</p></main></body></html>");
    ESP.restart();
  }
  else
  {
    handleScan(obj);
  }
}

void clearEEPROM(u16_t val)
{
  EEPROM.begin(val);
  for (u16_t i = 0; i < val; i++)
  {
    EEPROM.write(i, 0);
    Serial.println(i);
  }

  EEPROM.end();
  Serial.println("EEPROM Clear");
}

void SendModbusID1(void *pvParameters);
void ReadModbusID1(void *pvParameters);
// void ReadModbusID2( void * pvParameters );
void ConfigWifi(void *pvParameters);
void RetryWifi(void *pvParameters);

void OTA_Update(void *pvParameters);
// khai báo mảng chứa dữ liệu trên thanh ghi
uint16_t hregs1[5]; // thanh ghi ID1 PH
uint16_t hregs2[2]; // thanh ghi ID2 temp & humidity
uint16_t hregs3[2]; // thanh ghi ID3 Light Sensor
uint16_t hregs4[1]; // thanh ghi ID4 RainFall
uint16_t hregs5[5]; // thanh ghi ID4 RainFall
uint16_t hregs6[5]; // thanh ghi ID4 RainFall

String dataprint = "";
void SendModbusID1(void *pvParameters)
{
  while (true)
  {
    vTaskDelay(1);
    // if (user.status == 1){
    // Serial.println("tao o day");
    ArduinoCloud.update();
  }
}

// Read Modbus485
void ReadModbusID1(void *pvParameters)
{
  float temp, ph, DO, DO1; // DO1 (mg/L), DO(%)
  unsigned long currentTime_checkFW = 0;
  unsigned long previous_rainTime = 0;
  float previous_rainFall = 0;
  u8_t cout_warning = 0;

  while (true)
  {
    vTaskDelay(1);
    // if (user_wifi.status == 1){
    // Serial.println("nam");
    // Read PH1
    ssid_val = user.ssid;
    if (readSync1(1, 0, 5, hregs5) == Modbus::EX_SUCCESS)
    {
      // hiển thị 5 giá trị data của mỗi ID ra cổng serial mỗi data = 2byte
      // dataprint = "Data ID1: " + String(hregs1[0]) + " : " + String(hregs1[1]) + " : " + String(hregs1[2]) + " : " + String(hregs1[3]) + " : " + String(hregs1[4]); //+" Data ID2: " + String(hregs2[0]) + " : " + String(hregs2[1]) + " : " + String(hregs2[2])+ " : " + String(hregs2[3]) + " : " + String(hregs2[4]);
      // Serial.println(dataprint);
      temp = hregs5[2] * 0.1;
      ph = hregs5[0] * 0.01;
      if ((temp > 0) & (temp < 100))
        temp_val = temp;
      else
        temp_val = 0;
      if ((ph > 0) & (ph < 14))
        ph_val = ph;
      else
        ph_val = 0;
      // Serial.println(dataprint);
      // oRP  = hregs2[0]; // muon do pH tren bo ID2 phai bo sung lai cam bien pH. Moi thoi diem chi co the do 1 thong so pH hoac ORP
      // dO   = hregs1[2] * 0.01; //%x0.1 - ppm*0.01 DO
      // doam = 87;
      //  ArduinoCloud.update();
    }
    else
    {
      temp_val = 0;
      ph_val = 0;
      // Serial.println("here========");
    }
    // // Ph Warning
    // delayMillis(60000)
    // {
    //   if (ph_val < 7.7 || ph_val > 8.0)
    //   {
    //     cout_warning += 1;
    //   }
    //   ph_warning = "";
    //   // Serial.println("cout_warning: " + String(cout_warning));
    // };

    // delayMillis(600000)
    // {
    //   if (cout_warning > 8)
    //   {
    //     ph_warning = String(ph_val);
    //   }
    //   else
    //   {
    //     ph_warning = "";
    //   }
    //   // Serial.println("ph_warning: " + ph_warning);
    //   cout_warning = 0;
    // }

    // SOSS temp
    if (strcmp(user.things, "PH4") == 0)
    {
      Serial.println("dang ph4");
      temp_val2 = temp_val;
      ph_val2 = ph_val;
    }

    // read ph6000v have DO
    if (readSync1(6, 0, 5, hregs6) == Modbus::EX_SUCCESS)
    {
      temp = hregs6[4] * 0.1;
      ph = hregs6[0] * 0.01;
      DO = hregs6[2] * 0.01;
      DO1 = (1.25 * hregs6[3] - 500) * 0.01;
      temp_val3 = (temp > 0 && temp < 100) ? temp : 0;
      ph_val3 = (ph > 0 && ph < 14) ? ph : 0;
      do_val3 = (DO > 0 && DO < 200) ? DO : 0;
      do_val3_1 = (DO1 > 0 && DO1 < 20) ? DO1 : 0;
    }
    else
    {
      temp_val3 = 0;
      ph_val3 = 0;
      do_val3 = 0;
      do_val3_1 = 0;
    }
    // function hello

    if (strcmp(user.things, "PH1") == 0)
    {
      Serial.println("dang ph1");
      temp_val = temp_val3;
      ph_val = ph_val3;
      do_val = do_val3;
    }

    // Read PH CHINA
    // SOSS temp
    // have ph_china then

    if (readSync(5, 1008, 4, hregs1) == Modbus::EX_SUCCESS)
    {
      // hiển thị 5 giá trị data của mỗi ID ra cổng serial mỗi data = 2byte
      // dataprint = "Data ID1: " + String(hregs1[0]) + " : " + String(hregs1[1]) + " : " + String(hregs1[2]) + " : " + String(hregs1[3]) + " : " + String(hregs1[4]); //+" Data ID2: " + String(hregs2[0]) + " : " + String(hregs2[1]) + " : " + String(hregs2[2])+ " : " + String(hregs2[3]) + " : " + String(hregs2[4]);
      // Serial.println(dataprint);
      // temp = hregs1[2] * 0.1;
      // ph = hregs1[0] * 0.01;

      // convert 2x int16 to 32-bit
      uint32_t pH = (hregs1[0] << 0 | hregs1[1] << 16);
      uint32_t temp = (hregs1[2] << 0 | hregs1[3] << 16);

      float pH_convert, temp_convert;
      memcpy(&pH_convert, &pH, sizeof(float));
      memcpy(&temp_convert, &temp, sizeof(float));
      Serial.println(pH_convert);
      Serial.println(temp_convert);

      temp_val2 = (temp_convert > 0 && temp_convert < 100) ? temp_convert : 0;
      ph_val2 = (pH_convert > 0 && pH_convert < 14) ? pH_convert : 0;

      // Serial.println(dataprint);
      // oRP  = hregs2[0]; // muon do pH tren bo ID2 phai bo sung lai cam bien pH. Moi thoi diem chi co the do 1 thong so pH hoac ORP
      // dO   = hregs1[2] * 0.01; //%x0.1 - ppm*0.01 DO
      // doam = 87;
      //  ArduinoCloud.update();
    }

    // else
    // {
    //   temp_val2 = 0;
    //   ph_val2 = 0;
    //   // temp_val = random(1, 11); // random() includes the lower bound but excludes the upper bound
    //   // ph_val = random(1, 11);
    //   // do_val2 = 0;
    // }

    if (strcmp(user.things, "PH3") == 0)
    {
      Serial.println("dang PH3");
      temp_val = temp_val2;
      ph_val = ph_val2;
    }

    // Read ES35
    if (readSync(2, 0, 2, hregs2) == Modbus::EX_SUCCESS)
    {
      // hiển thị 5 giá trị data của mỗi ID ra cổng serial mỗi data = 2byte
      // dataprint = "Data ID1: " + String(hregs1[0]) + " : " + String(hregs1[1]); //+" Data ID2: " + String(hregs2[0]) + " : " + String(hregs2[1]) + " : " + String(hregs2[2])+ " : " + String(hregs2[3]) + " : " + String(hregs2[4]);
      temperature = (hregs2[0] * 0.1);
      humidity = (hregs2[1] * 0.1);

      Serial.println(dataprint);
    }
    else
    {
      temperature = 0;
      humidity = 0;
    }

    // Read data light sensor
    if (readSync(3, 7, 2, hregs3) == Modbus::EX_SUCCESS)
    {
      // hiển thị 5 giá trị data của mỗi ID ra cổng serial mỗi data = 2byte
      // dataprint2 = "Data ID2: " + String(hregs3[7]) + String(hregs3[8]); //+" Data ID2: " + String(hregs2[0]) + " : " + String(hregs2[1]) + " : " + String(hregs2[2])+ " : " + String(hregs2[3]) + " : " + String(hregs2[4]);
      // rainFall = (humidity <0 && humidity > 100)? (hregs1[1] * 0.1) : 0;
      light_val = hregs3[0] * 65536 + hregs3[1];
      // Serial.println(dataprint2);
      Serial.println(light_val);
    }
    else
    {
      light_val = 0;
    }

    // Read data rainFall
    if (readSync(4, 0, 2, hregs4) == Modbus::EX_SUCCESS)
    {
      // hiển thị 5 giá trị data của mỗi ID ra cổng serial mỗi data = 2byte
      // dataprint1 = "Data ID2: " + String(hregs2[0]); //+" Data ID2: " + String(hregs2[0]) + " : " + String(hregs2[1]) + " : " + String(hregs2[2])+ " : " + String(hregs2[3]) + " : " + String(hregs2[4]);
      // rainFall = (humidity <0 && humidity > 100)? (hregs1[1] * 0.1) : 0;
      sum_rainFall = hregs4[0] * 0.1; // sum mưa

      if (sum_rainFall - previous_rainFall > 0)
      {
        if (rainFall == sum_rainFall - previous_rainFall)
        {
          if ((unsigned long)millis() - previous_rainTime >= 7200000)
          {
            rainFall = 0;
            previous_rainFall = sum_rainFall;
          }
        }
        else
        {
          rainFall = sum_rainFall - previous_rainFall;
          previous_rainTime = millis();
        }
      }
      else
      {
        rainFall = 0;
      }

      status_rain = rainFall == 1 ? true : false;   // light rain
      status_rain1 = rainFall == 5 ? true : false;  // moderate rain
      status_rain2 = rainFall == 10 ? true : false; // heavy rain
      Serial.println(rainFall);
    }
    else
    {
      rainFall = 0;
      sum_rainFall = 0;
    }
    firmware_Ver = VersionFirware.toFloat();

    Serial.println("PH");
    Serial.println(temp_val2);
    Serial.println(ph_val2);
    // Serial.println(temp_val2);
    // Serial.println(ph_val2);
    // Serial.println(do_val);
    // Serial.println("ES35");
    // Serial.println(temperature);
    // Serial.println(humidity);
    // Serial.println("light");
    // Serial.println(light_val);
    // Serial.println("rainFall");
    // Serial.println(rainFall);
    // Serial.println("sum_rainFall");
    // Serial.println(sum_rainFall);
    // Serial.println(firmware_Ver);
    // Serial.println("Namdeptrai");
    // Serial.println("Sent Arduino Cloud data");
    // Serial.println("Update OTA Pro");
    // Serial.println("Update ota Prox2");

    if (touchRead(touchPin) < 20)
    {
      user.mode = 2;
      EEPROM.put(0, user);
      EEPROM.commit();
      ESP.restart();
    }

    if (digitalRead(clearPin) == 0)
    {
      Serial.println("Deleting EEPROM");
      clearEEPROM(4096);
      ESP.restart();
    }

    // time check auto update cycle == 30 minutes (1800000)
    if ((unsigned long)(millis() - currentTime_checkFW) >= 1800000)
    {
      currentTime_checkFW = millis();
      if (FirmwareVersionCheck())
      {
        user.mode = 2;
        EEPROM.put(0, user);
        EEPROM.commit();
        ESP.restart();
      }
    }
  }
}

void ConfigWifi(void *pvParameters)
{
  String nameWifi = "ESP32";
  if (user.things[0] != '\0')
  {
    nameWifi += "_" + String(user.things);
  }
  WiFi.mode(WIFI_AP);
  WiFi.softAP(nameWifi, "12345678");
  Serial.println(WiFi.softAPIP());
  tries = 1;
  server.on("/", handlePortal);
  server.begin();

  while (tries == 1)
  {
    server.handleClient();
  }
}

uint8_t changeWifi = 0;
void RetryWifi(void *pvParameters)
{
  uint8_t timesRetry = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Retry connect to '");
    Serial.print(user.ssid);
    Serial.println("'");
    WiFi.begin(user.ssid, user.password);
    // scanWiFiNetworks();
    // delay(1000);
    vTaskDelay(5000);
    timesRetry++;
    if (timesRetry == 12)
    {
      strncpy(user.ssid, ssid[changeWifi].c_str(), sizeof(user.ssid));
      strncpy(user.password, password[changeWifi].c_str(), sizeof(user.password));
      EEPROM.put(0, user);
      EEPROM.commit();
      changeWifi = (changeWifi < 3) ? (changeWifi + 1) : 0;
      Serial.println(changeWifi);
      timesRetry = 0;
    }
  }
  user.mode = 1;
  EEPROM.put(0, user);
  EEPROM.commit();
  ESP.restart();
}

void OTA_Update(void *PvParameters)
{
  UpdateOTA();
}

void setup()
{
  // Initialize serial and wait for port to open
  Serial.begin(115200);
  Serial.println("HELLO IoT Farm");

  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, user);
  Serial.println(user.things);
  Serial.println(user.board_id);
  Serial.println(user.device_key);
  delay(1000);
  // Serial.println(user.ssid);
  VersionFirware = user.FirmwareVer;
  Serial.println(firmware_Ver);
  WiFi.mode(WIFI_STA);
  WiFi.begin(user.ssid, user.password);
  Serial.println(user.mode);

  while (WiFi.status() != WL_CONNECTED)
  {
    {
      delay(1000);
      Serial.println(tries);
      if (tries++ > 10)
      {
        user.mode = 0;
        EEPROM.put(0, user);
        EEPROM.commit();
        break;
      }
    }
  }

  if (user.things[0] == '\0' || user.board_id[0] == '\0' || user.device_key[0] == '\0')
  {
    if (user.ssid[0] != '\0')
    {
      clearEEPROM(4096);
      Serial.println("here====");
      ESP.restart();
    }
  }

  if (digitalRead(clearPin) == 0)
  {
    Serial.println("Deleting EEPROM");
    clearEEPROM(4096);
    ESP.restart();
  }

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  MBUS_HW_SERIAL.begin(BAUDRATE, SERIAL_8N1, MBUS_RXD_PIN, MBUS_TXD_PIN);
  mb.begin(&MBUS_HW_SERIAL);
  mb.master();
  xMutex = xSemaphoreCreateMutex();
  // phân chia CPU cho các task và mức độ ưu tiên của task trong mỗi core - đang chạy core 0 và 1

  if (user.mode == 1)
  {
    // Only edit here
    xTaskCreatePinnedToCore(ReadModbusID1, "Task1", 5000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(SendModbusID1, "Task2", 10000, NULL, 3, NULL, 1);
  }
  else if (user.mode == 2)
  {
    xTaskCreatePinnedToCore(OTA_Update, "Task4", 5000, NULL, 4, NULL, 0);

    // xTaskCreate(UpdateOTA, "Task4", 5000, NULL, 4, NULL);
  }
  else
  {
    xTaskCreatePinnedToCore(ConfigWifi, "Task3", 5000, NULL, 1, NULL, 1);
    if (user.ssid[0] != '\0')
    {
      xTaskCreatePinnedToCore(RetryWifi, "Task4", 5000, NULL, 2, NULL, 0);
    }
  }
}

void loop() {}
