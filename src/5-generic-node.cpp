#include <Arduino.h>
#include <CAN.h>
#include <settings.h>

#define DEBUG true

void ReadCANWriteMotors(int packetSize);
byte readDipSwitch();

void setup() {

    #if DEBUG
        Serial.begin(SERIAL_BAUD_RATE);

        while (!Serial) {
            delay(100);
        }

        Serial.println("Arm node 1");
    #endif

    pinMode(ADDRESS_DIP_1_PIN, INPUT);
    pinMode(ADDRESS_DIP_2_PIN, INPUT);
    pinMode(ADDRESS_DIP_3_PIN, INPUT);
    pinMode(ADDRESS_DIP_4_PIN, INPUT);

    pinMode(PWM_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);

    pinMode(MAIN_STATUS_LED_PIN, OUTPUT);
    pinMode(CAN_SYNC_LED_PIN, OUTPUT);
    pinMode(CAN_RX_LED_PIN, OUTPUT);
    pinMode(CAN_TX_LED_PIN, OUTPUT);
    pinMode(IMU_CALIBRATION_LED_PIN, OUTPUT);

    digitalWrite(CAN_TX_LED_PIN, LOW);  // not implemented
    digitalWrite(IMU_CALIBRATION_LED_PIN, LOW);  // not implemented

    CAN.setPins(CRX_PIN, CTX_PIN);

    while (!CAN.begin(CAN_BAUD_RATE)) {

        #if DEBUG
            Serial.println("Starting CAN failed!");
        #endif

        delay(100);
    }

    #if DEBUG
        Serial.println("CAN Initialized successfully.");
    #endif

    #if DEBUG
        Serial.println(readDipSwitch());
    #endif
}

void loop() {
    int packetSize = CAN.parsePacket();
    
    ReadCANWriteMotors(packetSize);
}

void ReadCANWriteMotors(int packetSize) {
    /**
     * @brief Reads CAN packet and writes to motor controller
     * @param packetSize Size of CAN packet
     * @details direction: 0 ==> stop, 1 ==> forward, -1 ==> backward
     * @details motorSpeed: 0-255
    */

    int motorSpeed = 0;
    int direction = 0;
    static bool CAN_RX_LED_State = false;
    char data[BYTES_PER_NODE];

    if (packetSize && (CAN.packetId() == readDipSwitch())) {

        #if DEBUG
            Serial.println("Received packet for this node.");
        #endif

        while (CAN.available()) {
            digitalWrite(CAN_RX_LED_PIN, HIGH);
            CAN.readBytes(data, BYTES_PER_NODE);
            motorSpeed = data[0];
            direction = data[1];
        }

        #if DEBUG
            Serial.print("Motor speed: ");
            Serial.println(motorSpeed);
        #endif

        digitalWrite(DIR_PIN, direction > 0);
        analogWrite(PWM_PIN, motorSpeed * (direction != 0));
        digitalWrite(CAN_SYNC_LED_PIN, LOW);
    }
}

byte readDipSwitch() {
    byte address = 0;

    address = digitalRead(ADDRESS_DIP_1_PIN);
    address <<= 1;
    address += digitalRead(ADDRESS_DIP_2_PIN);
    address <<= 1;
    address += digitalRead(ADDRESS_DIP_3_PIN);
    address <<= 1;
    address += digitalRead(ADDRESS_DIP_4_PIN);

    #if DEBUG
        Serial.print("Address: ");
        Serial.println(address, HEX);
    #endif

    return address;
}