#ifndef MODBUSRS485_H
#define MODBUSRS485_H

#include <ModbusMaster.h>
void checkParameters_PH(int num_InputRegisters, int device_ID);
void checkParameters_PH6000V(int num_InputRegisters, int device_ID);
void checkParameters_PD6000(int num_InputRegisters, int device_ID);
extern uint8_t mydata[10]; // Dữ liệu truyền LoRa

#endif
