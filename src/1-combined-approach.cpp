#include <Arduino.h>
#include <CAN.h>
#include <settings.h>

void ReadCANWriteMotors(byte address);
byte readDipSwitch();

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    pinMode(ADDRESS_DIP_1_PIN, INPUT);
    pinMode(ADDRESS_DIP_2_PIN, INPUT);
    pinMode(ADDRESS_DIP_3_PIN, INPUT);
    pinMode(ADDRESS_DIP_4_PIN, INPUT);

    pinMode(PWM_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);

    while (!Serial) {
        delay(100);
    }

    // set initial address
    byte address = readDipSwitch();

    CAN.setPins(CRX_PIN, CTX_PIN);

    while (!CAN.begin(CAN_BAUD_RATE)) {
        Serial.println("Starting CAN failed!");
        delay(100);
    }

    Serial.println("CAN Initialized successfully.");
}

void loop() {
    ReadCANWriteMotors(readDipSwitch());
}

void ReadCANWriteMotors(byte address) {
    static int motorSpeed = 0;

    if (CAN.parsePacket()) {
        if (CAN.packetId() == address) {
            Serial.println("Received packet for this node.");

            while (CAN.available()) {
                motorSpeed = CAN.read();
                Serial.print("Motor speed: ");
                Serial.println(motorSpeed);

                analogWrite(PWM_PIN, motorSpeed);
            }
        }
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

    Serial.print("Address: ");
    Serial.println(address, HEX);

    return address;
}