/* 
 *  Based on: https://gist.github.com/bonsi/9298780eefc6a0b1057f19779a901001
 *  
 *  Board: Arduino Nano.
 *
 *  Pin D2 connected to RX1 on drone micro controller.
 *  Pin D3 connected to RX on drone micro controller.
 *
  */
#include <SoftwareSerial.h>

#define MSP_ATTITUDE 108

SoftwareSerial mspSerial(3, 2); // RX TX

int ledError = 0;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    mspSerial.begin(9600);
    Serial.begin(115200);
}

void loop() {
    uint8_t datad = 0;
    uint8_t *data = &datad;

    sendMSP(MSP_ATTITUDE, data, 0);
    int pitch = readData();

    Serial.println("Pitch: " + String(pitch/10.0));

    ledError = pitch / 10;
    if (ledError < 0) {
      ledError = -ledError;
    }
    if (ledError > 100) {
      ledError = 100;
    }
}

void sendMSP(uint8_t cmd, uint8_t *data, uint8_t n_bytes) {

    uint8_t checksum = 0;

    mspSerial.write((byte *) "$M<", 3);
    mspSerial.write(n_bytes);
    checksum ^= n_bytes;

    mspSerial.write(cmd);
    checksum ^= cmd;

    mspSerial.write(checksum);
}

int readData() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(ledError);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100 - ledError);

    byte count = 0;

    int16_t roll;
    int16_t pitch;
    int16_t yaw;

    while (mspSerial.available()) {
        count += 1;
        byte c = mspSerial.read();
        switch (count) {
            case 6:
                roll = c;
                break;
            case 7:
                roll <<= 8;
                roll += c;
                roll = (roll & 0xFF00) >> 8 | (roll & 0x00FF) << 8; // Reverse the order of bytes
                break;
            case 8:
                pitch = c;
                break;
            case 9:
                pitch <<= 8;
                pitch += c;
                pitch = (pitch & 0xFF00) >> 8 | (pitch & 0x00FF) << 8; // Reverse the order of bytes
                break;
            case 10:
                yaw = c;
                break;
            case 11:
                yaw <<= 8;
                yaw += c;
                yaw = (yaw & 0xFF00) >> 8 | (yaw & 0x00FF) << 8; // Reverse the order of bytes
                break;
        }
    }

    return pitch;
}
