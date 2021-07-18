#ifndef configuration_h
#define configuration_h

typedef struct Config_t
{
    double *pitchCorrection;
} Configuration_t;

void startConfigurationTask(Configuration_t *configuration);
void _configuration(void *parameters);

#endif
