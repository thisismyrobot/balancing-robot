/*
 * Add https://dl.espressif.com/dl/package_esp32_index.json as an additional board manager URL.
 * Install esp32 board via the board manager.
 * Select Heltec_Wifi_Kit_32 as board.
 * Install the MsgPack Library (0.3.8).
 * 
 */
#include <Arduino.h>
#include "src/telemetry.h"

int count = 0;

// Pointers to the shared data.
TelemetryData_t telemetryData = {
    &count
};


void setup() {
    count = 100;

    startTelemetryTask((TelemetryData_t *)&telemetryData);
}

void loop() {
    count += 1;
    delay(1000);
}
