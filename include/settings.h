#if !defined(SETTINGS_H)
#define SETTINGS_H


#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200  // bps
#define CAN_BAUD_RATE 500E3      // Kbps

const uint8_t CTX_PIN = 15;
const uint8_t CRX_PIN = 2;

const uint8_t ADDRESS_DIP_1_PIN = 4;
const uint8_t ADDRESS_DIP_2_PIN = 16;  // RX2
const uint8_t ADDRESS_DIP_3_PIN = 17;  // TX2
const uint8_t ADDRESS_DIP_4_PIN = 5;

const uint8_t PWM_PIN = 26;
const uint8_t DIR_PIN = 25;


#endif // SETTINGS_H
