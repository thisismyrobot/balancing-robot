#include <Arduino.h>
#include "telemetry.h"


// The UI task runs on the non-default core 0.
TaskHandle_t _TelemetryCoreZeroTask;


void startTelemetryTask(TelemetryData_t *telemetryData) {
  xTaskCreatePinnedToCore(
                    _telemetry,               /* Task function. */
                    "Telemetry",              /* name of task. */
                    10000,                    /* Stack size of task */
                    (void *)telemetryData,    /* parameter of the task */
                    1,                        /* priority of the task */
                    &_TelemetryCoreZeroTask,  /* Task handle to keep track of created task */
                    0);                       /* pin task to core 0 */
}


void _telemetry(void *parameters) {

  TelemetryData_t telemetryData = *((TelemetryData_t*)parameters);

  // Connect.
  Serial.begin(115200);

  for(;;){

    int count = *telemetryData.count;

    // Stream.
    Serial.println("Count:" + String(count));

    // At least 1, otherwise the watchdog kills the task.
    delay(500);
  }
}
