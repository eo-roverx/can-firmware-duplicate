#include <Arduino.h>
#include <CAN.h>

#include "settings.h"

#define DEBUG true

char* readBytesFromSerial();
void sendDataToOtherNodes(char* SerialData);

void setup() {
    bool statusLEDState = LOW;
    Serial.begin(SERIAL_BAUD_RATE);

    pinMode(MAIN_STATUS_LED_PIN, OUTPUT);
    pinMode(CAN_TX_LED_PIN, OUTPUT);
    digitalWrite(CAN_TX_LED_PIN, LOW);

#if DEBUG
    Serial.println("Translator node");
#endif

    CAN.setPins(CRX_PIN, CTX_PIN);

    while (!CAN.begin(CAN_BAUD_RATE)) {
#if DEBUG
        Serial.println("Starting CAN failed!");
#endif

        delay(100);
        digitalWrite(MAIN_STATUS_LED_PIN, statusLEDState);
        statusLEDState = !statusLEDState;
    }

#if DEBUG
    Serial.println("can init success");
#endif

    digitalWrite(MAIN_STATUS_LED_PIN, HIGH);
    delay(100);
    digitalWrite(MAIN_STATUS_LED_PIN, LOW);
}

void loop() {
    char SerialData[SERIAL_DATA_LENGTH];

    // justification to declare inside loop
    // https://stackoverflow.com/a/7959658/10907280
    digitalWrite(CAN_TX_LED_PIN, HIGH);

    testNode();

    while (Serial.available()) {
#if DEBUG
        Serial.println("sa i");
#endif

        Serial.readBytes(SerialData, SERIAL_DATA_LENGTH);

        digitalWrite(CAN_TX_LED_PIN, HIGH);
        sendDataToOtherNodes(SerialData);
        digitalWrite(CAN_TX_LED_PIN, LOW);
    }
}

void testNode() {
    byte data[2];

    for (size_t node = 0; node < TOTAL_NODE_COUNT; node++) {
#if DEBUG
        Serial.print("Sending to node: ");
        Serial.println(FIRST_NODE_ADDRESS + node, HEX);
#endif

        for (int direction = 0; direction < 2; direction++) {
            for (uint8_t speed = 0; speed < 255; speed++) {
                digitalWrite(CAN_TX_LED_PIN, HIGH);
                CAN.beginPacket(FIRST_NODE_ADDRESS + node);

                data[0] = speed;
                data[1] = direction;

                CAN.write(data, BYTES_PER_NODE);

                CAN.endPacket();

                digitalWrite(CAN_TX_LED_PIN, LOW);

                Serial.println(speed);

                delay(25);
            }

            for (uint8_t speed = 255; speed > 0; speed--) {
                digitalWrite(CAN_TX_LED_PIN, HIGH);
                CAN.beginPacket(FIRST_NODE_ADDRESS + node);

                data[0] = speed;
                data[1] = direction;

                CAN.write(data, BYTES_PER_NODE);

                CAN.endPacket();

                digitalWrite(CAN_TX_LED_PIN, LOW);

                Serial.println(speed);

                delay(25);
            }
        }

#if DEBUG
        Serial.print("Sent to node: ");
        Serial.println(FIRST_NODE_ADDRESS + node, HEX);
#endif
    }

#if DEBUG
    Serial.println("Sent to all nodes");
#endif
}

void sendDataToOtherNodes(char* SerialData) {
#if DEBUG
    Serial.print("read: ");
    Serial.println(SerialData);
#endif

    byte data[BYTES_PER_NODE];

    for (size_t node = 0; node < TOTAL_NODE_COUNT; node++) {
#if DEBUG
        Serial.println("ctx i");
        digitalWrite(CAN_TX_LED_PIN, HIGH);
#endif

        CAN.beginPacket(FIRST_NODE_ADDRESS + node);
#if DEBUG
        Serial.println("packet begun");
#endif

        data[0] = SerialData[node * 2];
        data[1] = SerialData[node * 2 + 1];

        CAN.write(data, BYTES_PER_NODE);
#if DEBUG
        Serial.println("written");
#endif

        CAN.endPacket();
#if DEBUG
        Serial.println("packet ended");
#endif

        digitalWrite(CAN_TX_LED_PIN, LOW);

#if DEBUG
        Serial.print("Sent to node: ");
        Serial.println(FIRST_NODE_ADDRESS + node, HEX);
#endif
    }

#if DEBUG
    Serial.println("Sent to all nodes");
#endif
}