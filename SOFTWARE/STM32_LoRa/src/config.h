#include <Arduino.h>
#define BUTTON_RESET_EEPROM PB7
#define HOLD_TIME 3000
extern uint8_t mode;
// extern uint8_t typeSetup;
extern TYPE_SETUP typeSetup;
extern MODE_READ_RS485 modeReadRS485;
extern uint8_t indexPond;
extern uint8_t modeLora;
extern bool isJoined;