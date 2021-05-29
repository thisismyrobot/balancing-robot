/* 
 *  Based on: https://gist.github.com/bonsi/9298780eefc6a0b1057f19779a901001
 *  
 *  Board: Arduino Nano.
 *  Extra Libraries: PID
 *
 *  Pin D2 connected to RX1 on drone micro controller.
 *  Pin D3 connected to RX on drone micro controller.
 *
  */
#include <SoftwareSerial.h>
#include <PID_v1.h>

#define MSP_ATTITUDE 108
#define MOTOR_FORWARD 5
#define MOTOR_REVERSE 6
#define MIN_MOTOR 30  //3s

double PidSetpoint;
double PidInput;
double PidOutput;
PID myPID(&PidInput, &PidOutput, &PidSetpoint, 4, 10, 0, DIRECT);

SoftwareSerial mspSerial(3, 2); // RX TX

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(MOTOR_FORWARD, OUTPUT);
    pinMode(MOTOR_REVERSE, OUTPUT);

    mspSerial.begin(9600);
    Serial.begin(115200);

    myPID.SetOutputLimits(-255, 255);
    PidSetpoint = -1;
    myPID.SetMode(AUTOMATIC);

    delay(3000);

    Serial.println("Awake!");
}

void loop() {
    uint8_t datad = 0;
    uint8_t *data = &datad;

    sendMSP(MSP_ATTITUDE, data, 0);
    double pitch = readPitch();

    if (pitch > PidSetpoint + 70 || pitch < PidSetpoint - 70) {
      stop();
    }

    PidInput = pitch;
    
    myPID.Compute();
    updateMotion(PidOutput);

    //Serial.println("PidInput: " + String(PidInput) + ", PidOutput: " + String(PidOutput));
    //Serial.println(String(PidInput) + "," + String(PidOutput) + "," + (millis() - lastLoopStartTime));
}

// + -> - 255
void updateMotion(double value)
{
  value = min(255, max(-255, value));

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

void sendMSP(uint8_t cmd, uint8_t *data, uint8_t n_bytes) {

    uint8_t checksum = 0;

    mspSerial.write((byte *) "$M<", 3);
    mspSerial.write(n_bytes);
    checksum ^= n_bytes;

    mspSerial.write(cmd);
    checksum ^= cmd;

    mspSerial.write(checksum);
}

double readPitch() {
    byte count = 0;

    int16_t roll = 0;
    int16_t pitch = 0;
    int16_t yaw = 0;

    // Descard everything until next '$'
    while (mspSerial.peek() != '$') {
      mspSerial.read();
      delay(1);
    }

    // Full altitude message is 12 bytes.
    while(mspSerial.available() < 12) {
    }

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

    return pitch / 10.0;
}

void stop() {
    analogWrite(MOTOR_FORWARD, 0);
    analogWrite(MOTOR_REVERSE, 0);
    while(1);
}

