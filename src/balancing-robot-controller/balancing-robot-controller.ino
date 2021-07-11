/*
 * Add https://dl.espressif.com/dl/package_esp32_index.json as an additional board manager URL.
 * Install esp32 board via the board manager.
 * Select Heltec_Wifi_Kit_32 as board.
 * Install the MsgPack Library (0.3.8).
 * 
 */
#include <Arduino.h>
#include "src/telemetry.h"

int watchdog = 0;
int rate = 0;
double pitch = 0;

const int statsUpdateMillis = 1000;
unsigned long lastStatsUpdateMillis = 0;
int rateLoops = 0;

// Pointers to the shared data.
TelemetryData_t telemetryData = {
    &watchdog,
    &rate,
    &pitch
};

void setup() {
    // F3 Board, using UART2 pins.
    Serial2.begin(38400);

    startTelemetryTask((TelemetryData_t *)&telemetryData);
}

void loop() {
    pitch = readPitch();

    updateStats();
}

void updateStats() {
    unsigned long nowMillis = millis();
    unsigned long elapsedMillis = nowMillis - lastStatsUpdateMillis;
    if (elapsedMillis > statsUpdateMillis) {
      watchdog++;
      rate = rateLoops / (elapsedMillis / 1000.0);
      
      rateLoops = 0;
      lastStatsUpdateMillis = nowMillis;      
    }

    rateLoops++;
}

// Return a relative pitch, accounting for the configured balance point offset.
double readPitch() {

    // Request Attitude
    uint8_t cmd = 108;  // MSP_ATTITUDE
    uint8_t n_bytes = 0;
    uint8_t checksum = 0;

    Serial2.write((byte *) "$M<", 3);
    Serial2.write(n_bytes);
    checksum ^= n_bytes;

    Serial2.write(cmd);
    checksum ^= cmd;

    Serial2.write(checksum);

    // Read response    
    byte count = 0;

    int16_t roll = 0;
    int16_t pitch = 0;
    int16_t yaw = 0;

    // Wait for anything.
    while (!Serial2.available());

    // Descard everything until next '$'
    while (Serial2.peek() != '$') {
      Serial2.read();
    }

    // Full altitude message is 12 bytes.
    while(Serial2.available() < 9);

    // Read the rest.
    while (Serial2.available()) {
        count += 1;
        byte c = Serial2.read();
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

    return (pitch / 10.0);
}
