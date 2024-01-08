#include <Arduino.h>
#include <CAN.h>
#include <settings.h>

#define DEBUG false

char* readBytesFromSerial();
void sendDataToArmNodes(char* SerialData);
bool* checkNodesRTR();

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    CAN.setPins(CRX_PIN, CTX_PIN);

    while (!CAN.begin(CAN_BAUD_RATE)) {
    #if DEBUG
            Serial.println("Starting CAN failed!");
    #endif

        delay(100);
    }
}

void loop() {
    char SerialData[SERIAL_DATA_LENGTH];
    bool* nodesRTR = checkNodesRTR();
    int SerialBytesRead = 0;

    // justification to declare inside loop
    // https://stackoverflow.com/a/7959658/10907280

    while (Serial.available()) {
        SerialBytesRead = Serial.readBytesUntil('\n', SerialData, SERIAL_DATA_LENGTH);

        // if serial data is not the correct length, do not do anything with it
        if (SerialBytesRead != SERIAL_DATA_LENGTH) {
            #if DEBUG
                Serial.println("Serial data is not the correct length");
            #endif

            continue;
        }

        // if serial data is the check string, send RTR to all nodes and read responses
        if (SerialData == RTR_CHECK_STRING) {
            nodesRTR = checkNodesRTR();

            for (size_t node = 0; node < ARM_NODES_COUNT; node++) {
                Serial.write(nodesRTR[node]);
            }
            Serial.println();

        // if serial data is not the check string, parse and send to other nodes
        } else {
            sendDataToArmNodes(SerialData);
        }
    }
}

void sendDataToArmNodes(char* SerialData) {
    byte data[BYTES_PER_NODE];

    for (
        size_t node = 0;
        node < ARM_NODES_COUNT;
        node++) {
        digitalWrite(CAN_TX_LED_PIN, HIGH);

        CAN.beginPacket(MOTOR_DATA_ADDRESS + FIRST_NODE_ADDRESS + node);

        data[0] = SerialData[node * 2];
        data[1] = SerialData[node * 2 + 1];

        CAN.write(data, BYTES_PER_NODE);

        CAN.endPacket();

        digitalWrite(CAN_TX_LED_PIN, LOW);

    #if DEBUG
            Serial.print("Sent to node: ");
            Serial.println(MOTOR_DATA_ADDRESS + FIRST_NODE_ADDRESS + node, HEX);
    #endif
    }

#if DEBUG
    Serial.println("Sent to all nodes");
#endif
}

bool* checkNodesRTR() {
    bool* nodesRTR = new bool[ARM_NODES_COUNT];  // true ==> OK, false ==> not OK

    for (size_t node = 0; node < ARM_NODES_COUNT; node++) {
        nodesRTR[node] = true;
    }

    for (size_t node = 0; node < ARM_NODES_COUNT; node++) {
        CAN.beginPacket(RTR_REQUEST_ADDRESS + FIRST_NODE_ADDRESS + node, 1, true);
        CAN.endPacket();

        // start a timeout timer
        long startTime = millis();

        while (!CAN.available()) {
            // if timeout expired, set node to false (not OK) and break out of loop
            if (millis() - startTime > RTR_TIMEOUT) {
                nodesRTR[node] = false;
                break;
            }
        }

        // if node is OK, read packet
        if (nodesRTR[node] && CAN.packetId() == (RTR_RESPONSE_ADDRESS + FIRST_NODE_ADDRESS + node)) {
            CAN.parsePacket();

            // if packet is 0, set node to true (OK)
            nodesRTR[node] = (CAN.read() == 0);
        }
    }

    return nodesRTR;
}