#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "configuration.h"
#include "network.h"


// The configuration task runs on the non-default core 0.
TaskHandle_t _ConfigurationCoreZeroTask;


void startConfigurationTask(Configuration_t *configuration) {
    xTaskCreatePinnedToCore(
        _configuration,                             /* Task function. */
        "Configuration",                            /* name of task. */
        10000,                                        /* Stack size of task */
        (void *)configuration,        /* parameter of the task */
        1,                                                /* priority of the task */
        &_ConfigurationCoreZeroTask,    /* Task handle to keep track of created task */
        0);                                             /* pin task to core 0 */
}

void _configuration(void *parameters) {

    Configuration_t configuration = *((Configuration_t*)parameters);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    WiFiUDP udp;
    udp.begin(target_port);

    char packetBuffer[2];
    for(;;){
        int packetSize = udp.parsePacket();

        if (packetSize) {
            int len = udp.read(packetBuffer, 2);
            if (len > 0) {
                packetBuffer[len] = 0;
                if (String(packetBuffer).equals("A+"))
                {
                    *configuration.pitchCorrection += 0.1;
                }
                else if (String(packetBuffer).equals("A-"))
                {
                    *configuration.pitchCorrection -= 0.1;
                }
            }
        }

        // At least 1, otherwise the watchdog kills the task.
        delay(100);
    }
}
