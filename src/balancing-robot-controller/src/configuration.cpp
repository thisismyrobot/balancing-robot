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

    WiFiUDP udp;
    udp.begin(target_port);

    Serial.begin(115200);
    Serial.println("config started");

    for(;;){

        // At least 1, otherwise the watchdog kills the task.
        delay(100);
    }
}
