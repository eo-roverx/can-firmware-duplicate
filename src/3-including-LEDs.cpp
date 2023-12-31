#include <Arduino.h>
#include <CAN.h>
#include <settings.h>

#define DEBUG false

char* read20BytesFromSerial();
void runMotor(char* SerialData);
void sendDataToOtherNodes(char* SerialData);
byte readDipSwitch();

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

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

    // set initial address
    byte address = readDipSwitch();

    CAN.setPins(CRX_PIN, CTX_PIN);

    while (!CAN.begin(CAN_BAUD_RATE)) {
        #if DEBUG
            Serial.println("Starting CAN failed!");
        #endif

        delay(100);
    }
}

void loop() {
    byte address = readDipSwitch();
    char* SerialData;

    // justification to declare inside loop
    // https://stackoverflow.com/a/7959658/10907280
    
    while (Serial.available()) {
        SerialData = read20BytesFromSerial();
        runMotor(SerialData);
    }
}

char* read20BytesFromSerial() {
    int SerialStringIndex = 0;
    char SerialString[SERIAL_DATA_LENGTH + 1];
    char receiveBuffer = Serial.read();

    while (receiveBuffer != ',') {
        SerialString[SerialStringIndex] = receiveBuffer;
        SerialStringIndex++;
        receiveBuffer = Serial.read();
    }

    SerialString[SerialStringIndex] = '\0';
    SerialStringIndex = 0;

    return SerialString;
}

void runMotor(char* SerialData) {
    int speedPWM = SerialData[0];
    int direction = SerialData[1];

    digitalWrite(DIR_PIN, direction);
    analogWrite(PWM_PIN, speedPWM);
}

void sendDataToOtherNodes(char* SerialData) {
    bool LEDState = false;

    for (
        int nodeAddress = 0x02;
        nodeAddress < (TOTAL_NODE_COUNT - 1); 
        nodeAddress++
    ) {
        CAN.beginPacket(nodeAddress);
        
        for (
            int byteIndex = 0;
            byteIndex < BYTES_PER_NODE;
            byteIndex++
        ) {
            CAN.write(SerialData[nodeAddress * BYTES_PER_NODE + byteIndex]);
        }

        CAN.endPacket();

        #if DEBUG
            Serial.print("Sent to node: ");
            Serial.println(nodeAddress, HEX);
        #endif
    }

    #if DEBUG
        Serial.println("Sent to all nodes");
    #endif
    
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