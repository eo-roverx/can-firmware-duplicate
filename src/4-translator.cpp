#include <Arduino.h>
#include <CAN.h>
#include <settings.h>

#define DEBUG true

char* readBytesFromSerial();
void sendDataToOtherNodes(char* SerialData);

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

#if DEBUG
  Serial.println("Translator node");
#endif

  CAN.setPins(CRX_PIN, CTX_PIN);

  while (!CAN.begin(CAN_BAUD_RATE)) {
#if DEBUG
    Serial.println("Starting CAN failed!");
#endif

    delay(100);
  }

#if DEBUG
  Serial.println("can init success");
#endif
}

void loop() {
  char SerialData[SERIAL_DATA_LENGTH];

  // justification to declare inside loop
  // https://stackoverflow.com/a/7959658/10907280

  while (Serial.available()) {
#if DEBUG
    Serial.println("sa i");
#endif

    Serial.readBytes(SerialData, SERIAL_DATA_LENGTH);
    sendDataToOtherNodes(SerialData);
  }
}

void sendDataToOtherNodes(char* SerialData) {
#if DEBUG
  Serial.print("read: ");
  Serial.println(SerialData);
#endif

  byte data[BYTES_PER_NODE];

  for (
    size_t node = 0;
    node < TOTAL_NODE_COUNT;
    node++) {

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