#ifndef drive_h
#define drive_h

typedef struct Drive_t
{
    // Metres/second.
    double *leftSpeedCommand;
    double *rightSpeedCommand;
    double *leftSpeedActual;
    double *rightSpeedActual;
} DriveCommands_t;

void setDriveParams(
    int forwardPwmChannel,
    int reversePwmChannel,
    int encLeftAGpio,
    int encLeftBGpio,
    int encRightAGpio,
    int encRightBGpio,
    double wheelDistanceM,
    double pulsesPerRadian,
    double p,
    double i,
    double d);

void startDriveTask(DriveCommands_t *driveCommands);
void _drive(void *parameters);
double _toDistanceMetres(double count);
void _setPwm(double leftPwmVector, double rightPwmVector);
double _map(double x, double in_min, double in_max, double out_min, double out_max);

#endif
