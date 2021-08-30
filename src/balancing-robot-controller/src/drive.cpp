#include <Arduino.h>
#include <ESP32Encoder.h>
#include <PID_v1.h>
#include "drive.h"

double _driveP;
double _driveI;
double _driveD;

ESP32Encoder _encoderLeft;
ESP32Encoder _encoderRight;

int _forwardLeftPwmChannel = 0;
int _reverseLeftPwmChannel = 1;
int _forwardRightPwmChannel = 2;
int _reverseRightPwmChannel = 3;
double _wheelDistanceM;
double _pulsesPerRevolution;

// The drive task runs on the non-default core 0.
TaskHandle_t _DriveCoreZeroTask;

void setDriveParams(
    int forwardLeftPwmChannel,
    int reverseLeftPwmChannel,
    int forwardRightPwmChannel,
    int reverseRightPwmChannel,
    int encLeftAGpio,
    int encLeftBGpio,
    int encRightAGpio,
    int encRightBGpio,
    double wheelDistanceM,
    double pulsesPerRevolution,
    double p,
    double i,
    double d)
{
    _forwardLeftPwmChannel = forwardLeftPwmChannel;
    _reverseLeftPwmChannel = reverseLeftPwmChannel;
    _forwardRightPwmChannel = forwardRightPwmChannel;
    _reverseRightPwmChannel = reverseRightPwmChannel;
    _wheelDistanceM = wheelDistanceM;
    _pulsesPerRevolution = pulsesPerRevolution;
    _driveP = p;
    _driveI = i;
    _driveD = d;

    _encoderLeft.attachFullQuad(encLeftAGpio, encLeftBGpio);
    _encoderRight.attachFullQuad(encRightAGpio, encRightBGpio);
}


void startDriveTask(DriveCommands_t *driveCommands) {
    xTaskCreatePinnedToCore(
        _drive,                             /* Task function. */
        "Drive",                            /* name of task. */
        10000,                                        /* Stack size of task */
        (void *)driveCommands,        /* parameter of the task */
        1,                                                /* priority of the task */
        &_DriveCoreZeroTask,    /* Task handle to keep track of created task */
        0);                                             /* pin task to core 0 */
}


void _drive(void *parameters) {

    DriveCommands_t drive = *((DriveCommands_t*)parameters);

    double leftPwmOutput = 0;
    double rightPwmOutput = 0;

    PID leftPid(drive.leftSpeedActual, &leftPwmOutput, drive.leftSpeedCommand, _driveP, _driveI, _driveD, DIRECT);
    PID rightPid(drive.rightSpeedActual, &rightPwmOutput, drive.rightSpeedCommand, _driveP, _driveI, _driveD, DIRECT);

    leftPid.SetOutputLimits(-255, 255);
    rightPid.SetOutputLimits(-255, 255);
    leftPid.SetMode(AUTOMATIC);
    rightPid.SetMode(AUTOMATIC);

    unsigned long lastPidUpdateMillis = 0;
    int lastLeftCount = 0;
    int lastRightCount = 0;

    for(;;){

        unsigned long nowMillis = millis();
        double elapsedSeconds = (nowMillis - lastPidUpdateMillis) / 1000.0;

        int leftCount = (int32_t)_encoderLeft.getCount();
        int rightCount = (int32_t)_encoderRight.getCount();

        *drive.leftSpeedActual = _toDistanceMetres(leftCount - lastLeftCount) / elapsedSeconds;
        *drive.rightSpeedActual = _toDistanceMetres(rightCount - lastRightCount) / elapsedSeconds;

        leftPid.Compute();
        rightPid.Compute();

        _setPwm(leftPwmOutput, rightPwmOutput);

        lastPidUpdateMillis = nowMillis;
        lastLeftCount = leftCount;
        lastRightCount = rightCount;

        delay(200);  // 5Hz okay
    }
}


void _setPwm(double leftPwmVector, double rightPwmVector)
{
    // TODO: USE RIGHT SIDE TOO.
    double leftPwm = constrain(leftPwmVector, -255, 255);
    double rightPwm = constrain(rightPwmVector, -255, 255);

    int forwardLeftValue = 0;
    int reverseLeftValue = 0;
    int forwardRightValue = 0;
    int reverseRightValue = 0;

    if (leftPwm > 0) {
        forwardLeftValue = _map(leftPwm, 0, 255, 40, 255);
    } 
    else if (leftPwm < 0){
        leftPwm = -leftPwm;
        reverseLeftValue = _map(leftPwm, 0, 255, 40, 255);
    }

    if (rightPwm > 0) {
        forwardRightValue = _map(rightPwm, 0, 255, 40, 255);
    } 
    else if (rightPwm < 0){
        rightPwm = -rightPwm;
        reverseRightValue = _map(rightPwm, 0, 255, 40, 255);
    }

    ledcWrite(_forwardLeftPwmChannel, forwardLeftValue);
    ledcWrite(_reverseLeftPwmChannel, reverseLeftValue);
    ledcWrite(_forwardRightPwmChannel, forwardRightValue);
    ledcWrite(_reverseRightPwmChannel, reverseRightValue);
}

double _toDistanceMetres(double count)
{
    return (count / _pulsesPerRevolution) * _wheelDistanceM;
}

// Arduino built-in map is integer only.
double _map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}