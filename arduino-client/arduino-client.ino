/* 
 *  Based on: https://gist.github.com/bonsi/9298780eefc6a0b1057f19779a901001
 *  
 *  Board: Arduino Nano.
 *  Extra Libraries: PID
 *
*/
#include <SoftwareSerial.h>
#include <PID_v1.h>

// Robot configuration and characteristics
#define BATTERY_VOLTAGE 7.4  // TODO: Read live.
#define MIN_MOTOR_VOLTS 1.5  // Tune per your DC motor.
#define ANGLE_DEADZONE 0.1 // +/- this pitch value is considered zero.
#define ANGLE_FALLEN 30

// Arduino wiring configuration.
#define MOTOR_FORWARD 5
#define MOTOR_REVERSE 6
#define SERIAL_RX 3
#define SERIAL_TX 2
#define TUNING_POT A7

// Multi-Wii commands.
#define MSP_ATTITUDE 108

// PID configuration.
#define P 50
#define I 0
#define D 0.2
#define MIN_MOTOR (255.0 / BATTERY_VOLTAGE) * MIN_MOTOR_VOLTS

// Timing.
#define DEBUG_LOOPS 1000
unsigned long timingStart;
int loops = 0;

double PidSetpoint;
double PidInput;
double PidOutput;
PID myPID(&PidInput, &PidOutput, &PidSetpoint, P, I, D, DIRECT);

SoftwareSerial mspSerial(SERIAL_RX, SERIAL_TX);

// Subtracted from read pitch angle to get zero = stable robot.
double balanceZeroAngle;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(MOTOR_FORWARD, OUTPUT);
    pinMode(MOTOR_REVERSE, OUTPUT);
    pinMode(TUNING_POT, INPUT);

    myPID.SetOutputLimits(-255, 255);
    myPID.SetMode(AUTOMATIC);

    Serial.begin(115200);
    Serial.println("Setting up...");

    // Grab the Pot value for tuning.
    balanceZeroAngle = tuningValue(-3, 3);
    Serial.println("Zero angle = " + String(balanceZeroAngle));

    // Let the F3 board settle before attempting to connect.
    delay(5000);
    mspSerial.begin(38400);

    timingStart = millis();
    Serial.println("Awake!");
}

void loop() {
    sendMSP(MSP_ATTITUDE, 0);
    double pitch = readPitch();

    if (pitch > ANGLE_FALLEN || pitch < -ANGLE_FALLEN) {
      stop();
    }

    if (pitch > -ANGLE_DEADZONE && pitch < ANGLE_DEADZONE) {
      pitch = 0;
    }

    PidInput = pitch;

    myPID.Compute();
    updateMotion(PidOutput);

    loops = loops + 1;
    if (loops > DEBUG_LOOPS) {
      unsigned long duration = millis() - timingStart;
      if (duration > 0) {
        unsigned long rate = (DEBUG_LOOPS * 1000L) / duration;
        Serial.println("Rate: " + String(rate) + "Hz, Pitch: " + String(pitch));
      }
      loops = 0;
      timingStart = millis();
    }
}

// + -> - 255
void updateMotion(double value)
{
  value = constrain(value, -255, 255);

  if (value == 0) {
      analogWrite(MOTOR_FORWARD, 128);
      analogWrite(MOTOR_REVERSE, 128);
  }
  else if (value > 0) {
      value = map(value, 0, 255, MIN_MOTOR, 255);
      analogWrite(MOTOR_FORWARD, value);
      analogWrite(MOTOR_REVERSE, 0);
  } 
  else {
      value = -value;

      value = map(value, 0, 255, MIN_MOTOR, 255);
      analogWrite(MOTOR_FORWARD, 0);
      analogWrite(MOTOR_REVERSE, value);
  }
}

void sendMSP(uint8_t cmd, uint8_t n_bytes) {

    uint8_t checksum = 0;

    mspSerial.write((byte *) "$M<", 3);
    mspSerial.write(n_bytes);
    checksum ^= n_bytes;

    mspSerial.write(cmd);
    checksum ^= cmd;

    mspSerial.write(checksum);
}

// Return a relative pitch, accounting for the configured balance point offset.
double readPitch() {
    byte count = 0;

    int16_t roll = 0;
    int16_t pitch = 0;
    int16_t yaw = 0;

    // Wait for anything.
    while (!mspSerial.available());

    // Descard everything until next '$'
    while (mspSerial.peek() != '$') {
      mspSerial.read();
    }

    // Full altitude message is 12 bytes.
    while(mspSerial.available() < 9);

    // Read the rest.
    while (mspSerial.available()) {
        count += 1;
        byte c = mspSerial.read();
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

    return (pitch / 10.0) - balanceZeroAngle;
}

void stop() {
    analogWrite(MOTOR_FORWARD, 0);
    analogWrite(MOTOR_REVERSE, 0);
    while(1) {
      delay(1);
    }
}

double tuningValue(double lowBound, double highBound) {
  double rawValue = analogRead(TUNING_POT);
  return map(rawValue, 1023, 0, lowBound, highBound);
}

double map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

