/*
 * Add https://dl.espressif.com/dl/package_esp32_index.json as an additional board manager URL.
 * Install esp32 board via the board manager (1.0.6).
 * Select Heltec_Wifi_Kit_32 as board.
 * 
 * Extra libraries:
 *  - MsgPack (0.3.8)
 *  - PID (1.2.0)
 *  - ESP32Encoder (0.5.1)
 * 
 */
#include <Arduino.h>
#include <HardwareSerial.h>
#include <PID_v1.h>
#include "src/telemetry.h"
#include "src/configuration.h"
#include "src/drive.h"

// Robot configuration and characteristics.
#define ANGLE_DEADZONE 0.1 // +/- this pitch value is considered zero.
#define ANGLE_FALLEN 40

// Closed loop motor control.
#define WHEEL_DISTANCE_M 0.208916  // 66.5mm diameter.
#define ENC_PULSES_PER_ROTATION 543.0  // 48:1 gearbox and ?!?! 11.3125 divisor. Odd.
#define MOTOR_CONTROL_P 50
#define MOTOR_CONTROL_I 1000
#define MOTOR_CONTROL_D 0

// Arduino wiring configuration.
#define ENABLE_GPIO 13
#define LED_GPIO 25

// Both motors bound together for now.
#define MOTOR_FORWARD_GPIO 27
#define MOTOR_REVERSE_GPIO 14

#define ENC_LEFT_A_GPIO 39
#define ENC_LEFT_B_GPIO 38
#define ENC_RIGHT_A_GPIO 36
#define ENC_RIGHT_B_GPIO 37

// PWM
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8
#define PWM_MOTOR_FORWARD_CHANNEL 0
#define PWM_MOTOR_REVERSE_CHANNEL 1

// Multi-Wii commands.
#define MSP_ATTITUDE 108

// PID configuration.
#define P 30.0
#define I 100.0
#define D 1.0
#define MIN_MOTOR (255.0 / BATTERY_VOLTAGE) * MIN_MOTOR_VOLTS

// PID instances.
double pitchPidSetpoint = 0;
double pitchPidInput;
double pitchPidOutput;
PID pitchPid(&pitchPidInput, &pitchPidOutput, &pitchPidSetpoint, P, I, D, DIRECT);

// Motor commands and tracking
double leftSpeedCommand = 0;
double rightSpeedCommand = 0;
double leftSpeedActual = 0;
double rightSpeedActual = 0;

// Statistics/telemetry
int watchdog = 0;
int rate = 0;
const int statsUpdateMillis = 250;
unsigned long lastStatsUpdateMillis = 0;
int rateLoops = 0;

// Subtracted from sensor reading to get robot balanced = 0.
double pitchCorrection = 0;

// Pointers to the shared data for telemetry and configuration.
TelemetryData_t telemetryData = {
    &watchdog,
    &rate,
    &pitchCorrection,
    &pitchPidInput,
    &pitchPidOutput,
    &leftSpeedCommand,
    &rightSpeedCommand,
    &leftSpeedActual,
    &rightSpeedActual    
};

Configuration_t configuration = {
    &pitchCorrection
};

DriveCommands_t driveCommands = {
    &leftSpeedCommand,
    &rightSpeedCommand,
    &leftSpeedActual,
    &rightSpeedActual
};

HardwareSerial F3Serial(1);

void setup() {
    setupPins();
    setDriveParams(
        PWM_MOTOR_FORWARD_CHANNEL,
        PWM_MOTOR_REVERSE_CHANNEL,
        ENC_LEFT_A_GPIO,
        ENC_LEFT_B_GPIO,
        ENC_RIGHT_A_GPIO,
        ENC_RIGHT_B_GPIO,
        WHEEL_DISTANCE_M,
        ENC_PULSES_PER_ROTATION,
        MOTOR_CONTROL_P,
        MOTOR_CONTROL_I,
        MOTOR_CONTROL_D);

/*

    pitchPid.SetOutputLimits(-255, 255);
    pitchPid.SetMode(AUTOMATIC);

    while(!enabled()) {
        digitalWrite(LED_GPIO, LOW);
        delay(250);
        digitalWrite(LED_GPIO, HIGH);
        delay(250);
    }

    setupF3();

    pitchCorrection = getAveragePitch(10);

    startConfigurationTask((Configuration_t *)&configuration);

*/
    startDriveTask((DriveCommands_t *)&driveCommands);
    startTelemetryTask((TelemetryData_t *)&telemetryData);
}

double getAveragePitch(int loops)
{
    double average = 0;
    for(int i = 0; i < loops; i++)
    {
        average += readPitch();
        digitalWrite(LED_GPIO, LOW);
        delay(150);
        digitalWrite(LED_GPIO, HIGH);
        delay(150);
    }
    return average / loops;
}

void loop() {

    leftSpeedCommand = 0.1;

    delay(5000);

    leftSpeedCommand = 0;

    delay(2000);

    leftSpeedCommand = -0.2;

    delay(2500);

    leftSpeedCommand = 0;

    delay(2000);

    leftSpeedCommand = 0.5;

    delay(1000);

    leftSpeedCommand = -0.2;

    delay(2500);

    /*
    double pitch = readPitch();

    if (pitch > ANGLE_FALLEN || pitch < -ANGLE_FALLEN) {
        reset();
    }

    if (abs(pitch) <= ANGLE_DEADZONE) {
        pitch = 0;
    }

    pitchPidInput = pitch;

    pitchPid.Compute();

    // Until steering implemented.
    leftSpeedCommand = pitchPidOutput;
    rightSpeedCommand = pitchPidOutput;

    updateStats();
*/
    if(!enabled()) {
        reset();
    }
}

void setupPins() {
    pinMode(ENABLE_GPIO, INPUT_PULLDOWN);

    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LOW);

    ledcSetup(PWM_MOTOR_FORWARD_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_MOTOR_REVERSE_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(MOTOR_FORWARD_GPIO, PWM_MOTOR_FORWARD_CHANNEL);
    ledcAttachPin(MOTOR_REVERSE_GPIO, PWM_MOTOR_REVERSE_CHANNEL);

    ledcWrite(PWM_MOTOR_FORWARD_CHANNEL, 0);
    ledcWrite(PWM_MOTOR_REVERSE_CHANNEL, 0);
}

void setupF3() {
    delay(5000);

    // using UART2 pins.
    F3Serial.begin(115200, SERIAL_8N1, 16, 17);
}

void updateStats() {
    unsigned long nowMillis = millis();
    unsigned long elapsedMillis = nowMillis - lastStatsUpdateMillis;
    if (elapsedMillis > statsUpdateMillis) {
        watchdog++;
        rate = rateLoops / (elapsedMillis / 1000.0);
        rateLoops = 0;
        lastStatsUpdateMillis = nowMillis;
    }

    rateLoops++;
}

// Return a relative pitch, accounting for the configured balance point offset.
double readPitch() {

    // Request Attitude
    uint8_t cmd = MSP_ATTITUDE;
    uint8_t n_bytes = 0;
    uint8_t checksum = 0;

    F3Serial.write((byte *) "$M<", 3);
    F3Serial.write(n_bytes);
    checksum ^= n_bytes;

    F3Serial.write(cmd);
    checksum ^= cmd;

    F3Serial.write(checksum);
    F3Serial.flush(true);  // true = TX only.
    delay(2);

    // Read response    
    byte count = 0;

    int16_t roll = 0;
    int16_t pitch = 0;
    int16_t yaw = 0;

    // Descard everything until next '$'
    while (F3Serial.peek() != '$') {
        F3Serial.read();
    }

    // Full altitude message is 12 bytes.
    while(F3Serial.available() < 9);

    // Read the rest.
    while (F3Serial.available()) {
        count += 1;
        byte c = F3Serial.read();
        switch (count) {
            case 6:
                roll = c;
                break;
            case 7:
                roll <<= 8;
                roll += c;
                roll = (roll & 0xFF00) >> 8 | (roll & 0x00FF) << 8; // Reverse the order of bytes
                break;
            case 8:
                pitch = c;
                break;
            case 9:
                pitch <<= 8;
                pitch += c;
                pitch = (pitch & 0xFF00) >> 8 | (pitch & 0x00FF) << 8; // Reverse the order of bytes
                break;
            case 10:
                yaw = c;
                break;
            case 11:
                yaw <<= 8;
                yaw += c;
                yaw = (yaw & 0xFF00) >> 8 | (yaw & 0x00FF) << 8; // Reverse the order of bytes
                break;
        }
    }

    return (pitch / 10.0) - pitchCorrection;
}

void reset()
{
    ESP.restart();
}

bool enabled()
{
    return digitalRead(ENABLE_GPIO);
}
