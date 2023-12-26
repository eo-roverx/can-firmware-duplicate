#include <Arduino.h>
#include <CAN.h>
#include <settings.h>

#define DEBUG false

void ReadCANWriteMotors(byte address);
byte readDipSwitch();

void setup() {

    #if DEBUG
        Serial.begin(SERIAL_BAUD_RATE);
        
        while (!Serial) {
            delay(100);
        }
    #endif

    pinMode(ADDRESS_DIP_1_PIN, INPUT);
    pinMode(ADDRESS_DIP_2_PIN, INPUT);
    pinMode(ADDRESS_DIP_3_PIN, INPUT);
    pinMode(ADDRESS_DIP_4_PIN, INPUT);

    pinMode(PWM_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);


    // set initial address
    byte address = readDipSwitch();

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
}

void loop() {
    ReadCANWriteMotors(readDipSwitch());
}

void ReadCANWriteMotors(byte address) {
    static int motorSpeed = 0;

    if (CAN.parsePacket()) {
        if (CAN.packetId() == address) {

            #if DEBUG
                Serial.println("Received packet for this node.");
            #endif

            while (CAN.available()) {
                motorSpeed = CAN.read();

                #if DEBUG
                    Serial.print("Motor speed: ");
                    Serial.println(motorSpeed);
                #endif

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

    #if DEBUG
        Serial.print("Address: ");
        Serial.println(address, HEX);
    #endif

    return address;
}