# Balancing robot

Using a BetaFlight-equipped flight controller (and later on an MPU6050) to
give the orientation and an Arduino Nano for control.

End goal is the Wheelie Boy from The Peripheral.

This is an educational (for myself) project but I want to share my steps
towards achieving (or not) this goal.

If you want to try out some of the code, use the tags below, anything else is
probably WIP and may not do anything useful :)

## Version 3

Currently WIP. Most recent work has completed closed loop motor control but
that's not working well enough - I think I need more pulses per rotation on
the encoder to achieve the faster cycle times which I seem to need for it to
perform at all well. Or bigger wheels, that'll help.

![](doc/build%203.gif) ![](doc/build%203.jpg)

### Thoughts

 * Will add encoders for (preferably) closed loop motor control or
   (worst-case) PID feedback and creep detection.
 * Most likely will use ESP32 to handle extra interrupts etc. Alternatively an
   M5Stack-C that I have with built-in IMU.
 * Should be able to read from F3 board - if using that - at a higher baud
   with ESP32. Backup is MPU6050.
 * Stream UDP telemetry and maybe even take commands - using second ESP32
   core? UDP for reduced cycle time.
 * I'll need to build a PID loop for the motor control, something new and
   probably where I'll start.
 * Also starting to think about what components would fit in a "Wheelie Boy"
   chassis. Might measure the performance of these motors if they work to have
   something to compare to. Also consider 11-12cm diameter wheels as that will
   be the end-goal

### Hardware (WIP)

 * ~~HappyModel SP Racing F3 EVO Brushed Micro FC - running BetaFlight 3.1.7 with minimal configuration except for fast serial response.~~ Serial interface (or my code) locks up after > 30s.
 * MPU6050
 * 2 x Solarbotics L293D Secret Motor Driver
 * Heltec Automation WiFi Kit 32 (an ESP32 board) for control
 * ~~2 x DG01D-E 48:1 DC motor w/wheel, 3-9V~~ Not powerful enough
 * 2 x 48.75:1 DC motor w/wheel, 3-12V (search for "Arduino DC Motor with Wheel and Rubber Tyres") + bodged-on encoders from DG01D-E.
 * Pololu 5V, 2.5A Step-Down Voltage Regulator D24V22F5
 * 3S LiPo (450mAh 45C)

## Version 2

See [Tag 2.0](https://github.com/thisismyrobot/balancing-robot/releases/tag/2.0)

![](doc/build%202.gif) ![](doc/build%202.jpg)

### Hardware

 * HappyModel SP Racing F3 EVO Brushed Micro FC - running BetaFlight 3.1.7
  with minimal configuration except for fast serial response.
 * 2 x Solarbotics L293D Secret Motor Driver
 * Arduino Nano
 * 2 x 48.75:1 DC motor w/wheel, 3-12V (search for "Arduino DC Motor with
   Wheel and Rubber Tyres")
 * Pololu 5V, 2.5A Step-Down Voltage Regulator D24V22F5
 * 3S LiPo (450mAh 45C)

### Thoughts

Pretty much balances. Next version will use encoders to get better motor speed
control and also distance measurement so I can start correcting for it
creeping.

## Version 1

See [Tag 1.0](https://github.com/thisismyrobot/balancing-robot/releases/tag/1.0)

![](doc/build%201.jpg)

### Hardware

 * HappyModel SP Racing F3 EVO Brushed Micro FC - running BetaFlight 3.1.7
   with minimal configuration.
 * Solarbotics L293D Secret Motor Driver
 * Arduino Nano
 * 48.75:1 DC motor w/wheel, 3-12V (search for "Arduino DC Motor with Wheel
   and Rubber Tyres")
 * Pololu 5V, 2.5A Step-Down Voltage Regulator D24V22F5
 * 2S and 3S LiPos to test (3S better)

### Thoughts

Too short to even start tuning PID, but it was a good proof of concept of
using the drone controller for orientation.

# Some notes on motors

I am trying different motors as I progress and I'll log what I know about them
here, performance-wise.

| Motor model | Robot version(s) | Stall Torque | No Load Speed |
| --- | --- | --- | --- |
| DG01D-A130GEARMOTOR | 1 + 2 | 0.8kg/cm @ 4.5V | 180 RPM @ 4.5V |
| DG01D-E | 3? | 0.8kg/cm @ 4.5V | 180 RPM @ 4.5V |
| FS90R | 4? Wheelie Boy? | 1.3kg/cm @ 4.8V | 100 RPM @ 4.8V |
