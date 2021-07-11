/*
 * Add https://dl.espressif.com/dl/package_esp32_index.json as an additional board manager URL.
 * Install esp32 board via the board manager (1.0.6).
 * Select Heltec_Wifi_Kit_32 as board.
 * Install the MsgPack Library (0.3.8).
 * 
 */
#include <Arduino.h>
#include <HardwareSerial.h>
#include "src/telemetry.h"

#define ENABLE_GPIO 13
#define LED_GPIO 25

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

HardwareSerial F3Serial(1);

void setup() {
    setupPins();

    while(!enabled()) {
        digitalWrite(LED_GPIO, LOW);
        delay(200);
        digitalWrite(LED_GPIO, HIGH);
        delay(200);
    }

    setupF3();

    startTelemetryTask((TelemetryData_t *)&telemetryData);
}

void loop() {
    pitch = readPitch();

    updateStats();

    if(!enabled()) {
        ESP.restart();
    }
}

void setupPins() {
    pinMode(ENABLE_GPIO, INPUT_PULLDOWN);
    pinMode(LED_GPIO, OUTPUT);

    digitalWrite(LED_GPIO, LOW);

    // TODO SET MOTORS TO ZERO.
}

void setupF3() {
    delay(5000);

    // using UART2 pins.
    F3Serial.begin(115200, SERIAL_8N1, 16, 17);
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

    F3Serial.write((byte *) "$M<", 3);
    F3Serial.write(n_bytes);
    checksum ^= n_bytes;

    F3Serial.write(cmd);
    checksum ^= cmd;

    F3Serial.write(checksum);
    F3Serial.flush(true);  // true = TX only.
    delay(2);

    // Read response    
    byte count = 0;

    int16_t roll = 0;
    int16_t pitch = 0;
    int16_t yaw = 0;

    // Descard everything until next '$'
    while (F3Serial.peek() != '$') {
      F3Serial.read();
    }

    // Full altitude message is 12 bytes.
    while(F3Serial.available() < 9);

    // Read the rest.
    while (F3Serial.available()) {
        count += 1;
        byte c = F3Serial.read();
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

bool enabled()
{
    return digitalRead(ENABLE_GPIO);
}
