#include <Arduino.h>

int _forwardPwmChannel = 0;
int _reversePwmChannel = 1;
int _minMotorPulse = 0;

void setDriveParams(int forwardPwmChannel, int reversePwmChannel, int minMotorPulse)
{
    _forwardPwmChannel = forwardPwmChannel;
    _reversePwmChannel = reversePwmChannel;
    _minMotorPulse = minMotorPulse;
}

void setSpeed(double speed)
{
    speed = constrain(speed, -255, 255);

    int forwardValue = 0;
    int reverseValue = 0;
    
    if (speed == 0) {
        // Active braking.
        forwardValue = 255;
        reverseValue = 255;
    }
    else if (speed > 0) {
        forwardValue = map(speed, 0, 255, _minMotorPulse, 255);
    } 
    else {
        reverseValue = map(-speed, 0, 255, _minMotorPulse, 255);
    }

    ledcWrite(_forwardPwmChannel, forwardValue);
    ledcWrite(_reversePwmChannel, reverseValue);
}

// Arduino built-in map is integer only.
double map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
