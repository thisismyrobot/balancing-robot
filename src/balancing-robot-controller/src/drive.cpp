#include <Arduino.h>
#include <ESP32Encoder.h>
#include <PID_v1.h>
#include "drive.h"

double _driveP;
double _driveI;
double _driveD;

ESP32Encoder _encoderLeft;
ESP32Encoder _encoderRight;

int _forwardPwmChannel = 0;
int _reversePwmChannel = 1;
double _wheelDistanceM;
double _pulsesPerRadian;

// The drive task runs on the non-default core 0.
TaskHandle_t _DriveCoreZeroTask;

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
    double d)
{
    _forwardPwmChannel = forwardPwmChannel;
    _reversePwmChannel = reversePwmChannel;
    _wheelDistanceM = wheelDistanceM;
    _pulsesPerRadian = pulsesPerRadian;
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

        if (*drive.leftSpeedCommand == 0) {
            leftPwmOutput = 0;
        }
        else if (*drive.leftSpeedCommand > 0) {
            leftPwmOutput = max((double)0.0, leftPwmOutput);
        }
        else if (*drive.leftSpeedCommand < 0) {
            leftPwmOutput = min((double)0.0, leftPwmOutput);
        }

        if (*drive.rightSpeedCommand == 0) {
            rightPwmOutput = 0;
        }
        else if (*drive.rightSpeedCommand > 0) {
            rightPwmOutput = max((double)0.0, leftPwmOutput);
        }
        else if (*drive.rightSpeedCommand < 0) {
            rightPwmOutput = min((double)0.0, leftPwmOutput);
        }

        _setPwm(leftPwmOutput, rightPwmOutput);

        lastPidUpdateMillis = nowMillis;
        lastLeftCount = leftCount;
        lastRightCount = rightCount;

        delay(5);
    }
}


void _setPwm(double leftPwmVector, double rightPwmVector)
{
    // TODO: USE RIGHT SIDE TOO.
    double leftPwm = constrain(leftPwmVector, -255, 255);

    int forwardValue = 0;
    int reverseValue = 0;

    if (leftPwm > 0) {
        forwardValue = _map(leftPwm, 0, 255, 100, 255);
    } 
    else if (leftPwm < 0){
        leftPwm = -leftPwm;
        reverseValue = _map(leftPwm, 0, 255, 100, 255);
    }

    ledcWrite(_forwardPwmChannel, forwardValue);
    ledcWrite(_reversePwmChannel, reverseValue);
}

double _toDistanceMetres(double count)
{
    return (count / _pulsesPerRadian) * _wheelDistanceM;
}

// Arduino built-in map is integer only.
double _map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}