#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <MapleFreeRTOS821.h>
#include <LoRa.h>
#include <RTClock.h>
#include <EEPROM.h>
int counter = 0;
RTClock rtc(RTCSEL_LSE);
////test
static const u1_t PROGMEM APPEUI[8] = {0x83, 0x29, 0xEE, 0x83, 0x8E, 0xDF, 0xFA, 0xFD}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
void os_getArtEui(u1_t *buf)
{
  memcpy_P(buf, APPEUI, 8);
}

static const u1_t PROGMEM DEVEUI[8] = {0x49, 0x80, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getDevEui(u1_t *buf)
{
  memcpy_P(buf, DEVEUI, 8);
}

static const u1_t PROGMEM APPKEY[16] = {0x15, 0x13, 0x83, 0x2D, 0x6F, 0xDF, 0x02, 0x7B, 0x7B, 0xE0, 0x0D, 0xA0, 0xDA, 0x41, 0x48, 0xD7}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
void os_getDevKey(u1_t *buf)
{
  memcpy_P(buf, APPKEY, 16);
}

// ////test1
// static const u1_t PROGMEM APPEUI[8] = {0x83, 0x29, 0xEE, 0x83, 0x8E, 0xDF, 0xFA, 0xFD}; //{ 0x5B, 0x6A, 0x6F, 0x5E, 0x4D, 0x3C, 0x2B, 0x1A }; //A0B1C2B3D4E5F6A7
// void os_getArtEui(u1_t *buf)
// {
//   memcpy_P(buf, APPEUI, 8);
// }

// // This should also be in little endian format, see above.
// static const u1_t PROGMEM DEVEUI[8] = {0x49, 0x80, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70}; //{ 0x4E, 0x35, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
// void os_getDevEui(u1_t *buf)
// {
//   memcpy_P(buf, DEVEUI, 8);
// }

// // This key should be in big endian format (or, since it is not really a
// // number but a block of memory, endianness does not really apply). In
// // practice, a key taken from ttnctl can be copied as-is.
// // The key shown here is the semtech default key.
// static const u1_t PROGMEM APPKEY[16] = {0x15, 0x13, 0x83, 0x2D, 0x6F, 0xDF, 0x02, 0x7B, 0x7B, 0xE0, 0x0D, 0xA0, 0xDA, 0x41, 0x48, 0xD7}; //{ 0xA6, 0xD0, 0x43, 0x38, 0x69, 0xC8, 0x35, 0x19, 0x1A, 0xD4, 0x9A, 0x1C, 0x23, 0x7F, 0x55, 0xD6 };
// void os_getDevKey(u1_t *buf)
// {
//   memcpy_P(buf, APPKEY, 16);
// }

static osjob_t sendjob;
uint8_t mydata[4];

// Pin mapping for STM32
const lmic_pinmap lmic_pins = {
    .nss = PA4,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PB0,
    .dio = {PB8, PB9, LMIC_UNUSED_PIN},
};

void printHex2(unsigned v)
{
  v &= 0xff;
  if (v < 16)
    Serial.print('0');
  Serial.print(v, HEX);
}

void do_send(osjob_t *j)
{
  // Check if there is not a current TX/RX job running

  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  }
  else
  {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
    // LMIC_setTxData2(1, &data, sizeof(data), 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void timestamp2Bytes(uint32_t timestamp, uint8_t *data)
{
  data[0] = (uint8_t)(timestamp >> 24);
  data[1] = (uint8_t)(timestamp >> 16);
  data[2] = (uint8_t)(timestamp >> 8);
  data[3] = (uint8_t)(timestamp);
}

void onEvent(ev_t ev)
{
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev)
  {
  case EV_SCAN_TIMEOUT:
    Serial.println(F("EV_SCAN_TIMEOUT"));
    break;
  case EV_BEACON_FOUND:
    Serial.println(F("EV_BEACON_FOUND"));
    break;
  case EV_BEACON_MISSED:
    Serial.println(F("EV_BEACON_MISSED"));
    break;
  case EV_BEACON_TRACKED:
    Serial.println(F("EV_BEACON_TRACKED"));
    break;
  case EV_JOINING:
    Serial.println(F("EV_JOINING"));
    digitalWrite(PC13, HIGH);
    break;
  case EV_JOINED:
    digitalWrite(PC13, LOW);
    Serial.println(F("EV_JOINED"));
    {
      u4_t netid = 0;
      devaddr_t devaddr = 0;
      u1_t nwkKey[16];
      u1_t artKey[16];
      LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
      Serial.print("netid: ");
      Serial.println(netid, DEC);
      Serial.print("devaddr: ");
      Serial.println(devaddr, HEX);
      Serial.print("AppSKey: ");
      for (size_t i = 0; i < sizeof(artKey); ++i)
      {
        if (i != 0)
          Serial.print("-");
        printHex2(artKey[i]);
      }
      Serial.println("");
      Serial.print("NwkSKey: ");
      for (size_t i = 0; i < sizeof(nwkKey); ++i)
      {
        if (i != 0)
          Serial.print("-");
        printHex2(nwkKey[i]);
      }
      Serial.println();
    }
    LMIC_setLinkCheckMode(0);
    break;

  case EV_JOIN_FAILED:
    Serial.println(F("EV_JOIN_FAILED"));
    break;
  case EV_REJOIN_FAILED:
    Serial.println(F("EV_REJOIN_FAILED"));
    break;
  case EV_TXCOMPLETE:
    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    if (LMIC.txrxFlags & TXRX_ACK)
      Serial.println(F("Received ack"));
    if (LMIC.dataLen)
    {

      uint8_t receivedData[LMIC.dataLen];
      for (int i = 0; i < LMIC.dataLen; i++)
      {
        receivedData[i] = LMIC.frame[LMIC.dataBeg + i];
      }
      if (LMIC.dataLen == 4)
      {
        // timestamp xanhap setup
        uint32_t rtc_xanhap_setup = ((uint32_t)receivedData[0] << 24 |
                                     (uint32_t)receivedData[1] << 16 |
                                     (uint32_t)receivedData[2] << 8 |
                                     (uint32_t)receivedData[3]);
        if (rtc_xanhap_setup != 0)
        {
          rtc.setTime(rtc_xanhap_setup);
        }
      }
      else
      {
        uint32_t rtc_xanhap_config = ((uint32_t)receivedData[0] << 24 |
                                      (uint32_t)receivedData[1] << 16 |
                                      (uint32_t)receivedData[2] << 8 |
                                      (uint32_t)receivedData[3]);
        uint16_t interval = ((uint16_t)receivedData[4] << 8 |
                             (uint16_t)receivedData[5]);
      }
    }
    // data = counter;
    // Serial.print("Sending ");
    // Serial.println(rtc.getTime());
    timestamp2Bytes(rtc.getTime(), mydata);

    os_setCallback(&sendjob, do_send);
    break;
  case EV_LOST_TSYNC:
    Serial.println(F("EV_LOST_TSYNC"));
    break;
  case EV_RESET:
    Serial.println(F("EV_RESET"));
    break;
  case EV_RXCOMPLETE:
    // data received in ping slot
    Serial.println(F("EV_RXCOMPLETE"));
    break;
  case EV_LINK_DEAD:
    Serial.println(F("EV_LINK_DEAD"));
    break;
  case EV_LINK_ALIVE:
    Serial.println(F("EV_LINK_ALIVE"));
    break;
  case EV_TXSTART:
    Serial.println(F("EV_TXSTART"));
    break;
  case EV_TXCANCELED:
    Serial.println(F("EV_TXCANCELED"));
    break;
  case EV_RXSTART:
    /* do not print anything -- it wrecks timing */
    break;
  case EV_JOIN_TXCOMPLETE:
    Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
    digitalWrite(PC13, LOW);
    break;

  default:
    Serial.print(F("Unknown event: "));
    Serial.println((unsigned)ev);
    break;
  }
}

static void vHandleControllerTask(void *pvParameters)
{
  tm_t currentTime;
  while (1)
  {
    rtc.getTime(currentTime);
    Serial.print("Time: ");
    Serial.print(currentTime.hour);
    Serial.print(":");
    Serial.print(currentTime.minute);
    Serial.print(":");
    Serial.println(currentTime.second);

    Serial.print("Date: ");
    Serial.print(currentTime.day);
    Serial.print("-");
    Serial.print(currentTime.month);
    Serial.print("-");
    Serial.println(currentTime.year);
    vTaskDelay(1000);
  }
}
static void vSendTask(void *pvParameters)
{
  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  // Start job (sending automatically starts OTAA too)
  do_send(&sendjob);
  while (1)
  {
    os_runloop_once();
  }
}
void setup()
{
  Serial.begin(9600);
  Serial.print("\r\n*******STM32-IoTFarm-by_NamNam201*********");
  pinMode(PC13, OUTPUT);
  pinMode(PB15, OUTPUT);
  pinMode(PB14, INPUT);
  rtc.setTime(1727680030);

  xTaskCreate(vHandleControllerTask,
              "Task1",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 2,
              NULL);

  xTaskCreate(vSendTask,
              "Task2",
              configMINIMAL_STACK_SIZE,
              NULL,
              tskIDLE_PRIORITY + 1,
              NULL);
  vTaskStartScheduler();
}

void loop()
{
}
