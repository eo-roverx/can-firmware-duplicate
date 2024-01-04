#include <Arduino.h>
#include <CAN.h>
#include <settings.h>

#define DEBUG false

char* readBytesFromSerial();
void sendDataToOtherNodes(char* SerialData);

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

    // justification to declare inside loop
    // https://stackoverflow.com/a/7959658/10907280
    
    while (Serial.available()) {
        Serial.readBytesUntil('\n', SerialData, SERIAL_DATA_LENGTH);
        sendDataToOtherNodes(SerialData);
    }
}

void sendDataToOtherNodes(char* SerialData) {
    byte data[BYTES_PER_NODE];

    for (
        size_t node = 0;
        node < TOTAL_NODE_COUNT; 
        node++
    ) {
        digitalWrite(CAN_TX_LED_PIN, HIGH);

        CAN.beginPacket(FIRST_NODE_ADDRESS + node);
        
        data[0] = SerialData[node * 2];
        data[1] = SerialData[node * 2 + 1];

        CAN.write(data, BYTES_PER_NODE);

        CAN.endPacket();

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