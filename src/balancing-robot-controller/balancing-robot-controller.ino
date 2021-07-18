/*
 * Add https://dl.espressif.com/dl/package_esp32_index.json as an additional board manager URL.
 * Install esp32 board via the board manager (1.0.6).
 * Select Heltec_Wifi_Kit_32 as board.
 * 
 * Extra libraries:
 *  - MsgPack (0.3.8)
 *  - PID (1.2.0)
 * 
 */
#include <Arduino.h>
#include <HardwareSerial.h>
#include "src/telemetry.h"
#include "src/drive.h"
#include <PID_v1.h>

// Robot configuration and characteristics.
#define BATTERY_VOLTAGE 12.0  // TODO: Read live.
#define MIN_MOTOR_VOLTS 1.5  // Tune per your DC motor.
#define ANGLE_DEADZONE 0.1 // +/- this pitch value is considered zero.
#define ANGLE_FALLEN 40

// Arduino wiring configuration.
#define ENABLE_GPIO 13
#define LED_GPIO 25
// Both motors bound together for now.
#define MOTOR_FORWARD_GPIO 14
#define MOTOR_REVERSE_GPIO 27

// PWM
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8
#define PWM_MOTOR_FORWARD_CHANNEL 0
#define PWM_MOTOR_REVERSE_CHANNEL 1

// Multi-Wii commands.
#define MSP_ATTITUDE 108

// PID configuration.
#define P 8.0
#define I 38.0
#define D 1.0
#define MIN_MOTOR (255.0 / BATTERY_VOLTAGE) * MIN_MOTOR_VOLTS

// PID instances.
double pitchPidSetpoint = 0;
double pitchPidInput;
double pitchPidOutput;
PID pitchPid(&pitchPidInput, &pitchPidOutput, &pitchPidSetpoint, P, I, D, DIRECT);

// Statistics/telemetry
int watchdog = 0;
int rate = 0;
const int statsUpdateMillis = 1000;
unsigned long lastStatsUpdateMillis = 0;
int rateLoops = 0;

// Subtracted from sensor reading to get robot balanced = 0.
double pitchCorrection = 0;

// Pointers to the shared data.
TelemetryData_t telemetryData = {
    &watchdog,
    &rate,
    &pitchCorrection,
    &pitchPidInput,
    &pitchPidOutput
};

HardwareSerial F3Serial(1);

void setup() {
    setupPins();
    setDriveParams(PWM_MOTOR_FORWARD_CHANNEL, PWM_MOTOR_REVERSE_CHANNEL, MIN_MOTOR);

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
    double pitch = readPitch();

    if (pitch > ANGLE_FALLEN || pitch < -ANGLE_FALLEN) {
        reset();
    }

    if (abs(pitch) <= ANGLE_DEADZONE) {
        pitch = 0;
    }

    pitchPidInput = pitch;

    pitchPid.Compute();
    setSpeed(pitchPidOutput);

    updateStats();

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
