#ifndef telemetry_h
#define telemetry_h

typedef struct Data_t
{
    int *count;
} TelemetryData_t;

void startTelemetryTask(TelemetryData_t *telemetryData);
void _telemetry(void *parameters);

#endif
