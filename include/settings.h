#if !defined(SETTINGS_H)
#define SETTINGS_H

#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200  // bps
#define CAN_BAUD_RATE 500E3      // Kbps
#define RTR_TIMEOUT 50           // ms
#define SERIAL_DATA_LENGTH 15    // bytes
#define RTR_CHECK_STRING "GET CAN STATUS"

#define ARM_NODES_COUNT 7        // only arm, not including protocol converter
#define BYTES_PER_NODE 2

// CAN addresses are formed by the following formula:
// PACKET_TYPE + FIRST_NODE_ADDRESS + SPECIFIC_NODE_NUMBER
// Bus arbitration is done by the CAN protocol, to decide priority
// LOWER ADDRESS = HIGHER PRIORITY
#define FIRST_NODE_ADDRESS 0x01  // only arm, not including protocol converter
#define TRANSLATOR_NODE_ADDRESS 0x00
#define RTR_REQUEST_ADDRESS 0x00
#define RTR_RESPONSE_ADDRESS 0x10
#define MOTOR_DATA_ADDRESS 0x20

const uint8_t CTX_PIN = 15;
const uint8_t CRX_PIN = 2;

const uint8_t ADDRESS_DIP_1_PIN = 4;
const uint8_t ADDRESS_DIP_2_PIN = 16;  // RX2
const uint8_t ADDRESS_DIP_3_PIN = 17;  // TX2
const uint8_t ADDRESS_DIP_4_PIN = 5;

const uint8_t PWM_PIN = 26;
const uint8_t DIR_PIN = 25;

const uint8_t MAIN_STATUS_LED_PIN = 32;
const uint8_t CAN_SYNC_LED_PIN = 13;
const uint8_t CAN_RX_LED_PIN = 12;
const uint8_t CAN_TX_LED_PIN = 14;
const uint8_t IMU_CALIBRATION_LED_PIN = 27;

#endif  // SETTINGS_H
