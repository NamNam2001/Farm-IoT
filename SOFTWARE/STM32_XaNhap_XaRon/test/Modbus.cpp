#include <ModbusMaster.h>
#include <SPI.h>
#include <Wire.h>

// instantiate ModbusMaster object
ModbusMaster node;

void setup()
{
    // use Serial (port 0); initialize Modbus communication baud rate
    Serial3.begin(9600);
    Serial.begin(9600);

    // communicate with Modbus slave ID 1 over Serial3
    node.begin(1, Serial3);
}

void loop()
{
    uint8_t result;
    uint8_t numRegisters = 5;
    uint16_t data[numRegisters];

    // Request to read 6 input registers starting from address 0
    result = node.readHoldingRegisters(1008, numRegisters);

    // Check if the request was successful
    if (result == node.ku8MBSuccess)
    {
        // Read the values into the data array
        for (uint8_t j = 0; j < numRegisters; j++)
        {
            data[j] = node.getResponseBuffer(j);
            Serial.print(F("Register "));
            Serial.print(j);
            Serial.print(F(": "));
            Serial.println(data[j]);
        }
    }
    else
    {
        // Print error message if request failed
        Serial.print(F("readInputRegisters() failed, error code: "));
        Serial.println(result);
    }

    delay(1000); // Delay for readability of output
}
