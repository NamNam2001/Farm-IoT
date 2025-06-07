#include "ModbusRS485.h"
// #define INTERVAL_RESET 60000 // Thời gian reset dữ liệu nếu không đọc được từ Modbus
ModbusMaster node;
uint8_t mydata[10]; // Dữ liệu truyền LoRa
// static unsigned long lastTimeCheckMyData = millis();
void checkParameters_PH6000V(int num_InputRegisters, int device_ID)
{
    node.begin(device_ID, Serial3);
    // Serial.println("Read Sensor PH6000V...");
    uint8_t result = node.readInputRegisters(0, num_InputRegisters);
    // Serial.println(result);
    if (result == node.ku8MBSuccess)
    {
        mydata[4] = node.getResponseBuffer(0) % 256;
        mydata[5] = node.getResponseBuffer(0) / 256;

        mydata[6] = node.getResponseBuffer(2) % 256;
        mydata[7] = node.getResponseBuffer(2) / 256;
        // lastTimeCheckMyData = millis();
    }
    else
    {
        // if (millis() - lastTimeCheckMyData >= INTERVAL_RESET)
        // {
        //     memset(mydata, 0, sizeof(mydata));
        //     // lastTimeCheckMyData = millis();
        // }
        memset(mydata, 0, sizeof(mydata));
    }
}
void checkParameters_PD6000(int num_InputRegisters, int device_ID)
{
    node.begin(device_ID, Serial3);
    // Serial.println("Read Sensor PD6000...");
    uint8_t result = node.readInputRegisters(0, num_InputRegisters);
    if (result == node.ku8MBSuccess)
    {
        mydata[4] = node.getResponseBuffer(0) % 256;
        mydata[5] = node.getResponseBuffer(0) / 256;

        mydata[6] = node.getResponseBuffer(4) % 256;
        mydata[7] = node.getResponseBuffer(4) / 256;

        mydata[8] = int((1.25 * node.getResponseBuffer(3) - 500)) % 256;
        mydata[9] = int(1.25 * node.getResponseBuffer(3) - 500) / 256;
        // lastTimeCheckMyData = millis();
    }
    else
    {
        // if (millis() - lastTimeCheckMyData >= INTERVAL_RESET)
        // {
        // lastTimeCheckMyData = millis();
        // }
        memset(mydata, 0, sizeof(mydata));
    }
}

void checkParameters_PH(int num_InputRegisters, int device_ID)
{
    node.begin(device_ID, Serial3);
    // Serial.println("Read Sensor PH...");
    uint8_t result = node.readHoldingRegisters(1008, num_InputRegisters);
    if (result == node.ku8MBSuccess)
    {
        uint16_t hregs[num_InputRegisters];
        for (uint8_t i = 0; i < num_InputRegisters; i++)
        {
            hregs[i] = node.getResponseBuffer(i);
        }
        // convert 2x int16 to 32-bit
        uint32_t pH = (hregs[0] << 0 | hregs[1] << 16);
        uint32_t temp = (hregs[2] << 0 | hregs[3] << 16);

        float pH_val, temp_val;
        memcpy(&pH_val, &pH, sizeof(float));
        memcpy(&temp_val, &temp, sizeof(float));
        // Serial.print("Float Value: ");
        // Serial.println(pH_val);
        // Serial.println(temp_val);

        mydata[4] = int(pH_val * 100) % 256;
        mydata[5] = int(pH_val * 100) / 256;

        mydata[6] = int(temp_val * 10) % 256;
        mydata[7] = int(temp_val * 10) / 256;
        // lastTimeCheckMyData = millis();
    }
    else
    {
        // set all data to 0 if read fails
        // if (millis() - lastTimeCheckMyData >= INTERVAL_RESET)
        // {
        //     memset(mydata, 0, sizeof(mydata));
        //     // lastTimeCheckMyData = millis();
        // }
        memset(mydata, 0, sizeof(mydata));
    }
}
