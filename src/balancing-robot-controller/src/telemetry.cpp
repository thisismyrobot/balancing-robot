#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <MsgPack.h>
#include "telemetry.h"
#include "network.h"


// The telemetry task runs on the non-default core 0.
TaskHandle_t _TelemetryCoreZeroTask;
MsgPack::Packer _packer;


void startTelemetryTask(TelemetryData_t *telemetryData) {
    xTaskCreatePinnedToCore(
        _telemetry,                             /* Task function. */
        "Telemetry",                            /* name of task. */
        10000,                                        /* Stack size of task */
        (void *)telemetryData,        /* parameter of the task */
        1,                                                /* priority of the task */
        &_TelemetryCoreZeroTask,    /* Task handle to keep track of created task */
        0);                                             /* pin task to core 0 */
}

void _telemetry(void *parameters) {

    TelemetryData_t telemetryData = *((TelemetryData_t*)parameters);

    // Connect.
    Serial.begin(115200);
    Serial.println("Telemetry task awake!");

    WiFi.begin(wifi_ssid, wifi_pass);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("Connecting to WiFi...");
        delay(500);
    }

    Serial.println("Connected to WiFi!");

    WiFiUDP udp;

    for(;;){

        _packer.clear();

        // This order *must* match the order of the telemetry listener.
        _packer.to_array(
            *telemetryData.count,
            *telemetryData.rate,
            *telemetryData.pitch
        );

        udp.beginPacket(target_ip, target_port);
        udp.write(_packer.data(), _packer.size());
        udp.endPacket();

        // At least 1, otherwise the watchdog kills the task.
        delay(100);
    }
}
