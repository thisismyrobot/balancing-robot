# Balancing robot

Using a BetaFlight-equipped flight controller (and later on an MPU6050) to
give the orientation and an Arduino Nano (and later an ESP32) for control.

End goal is the Wheelie Boy from The Peripheral.

This is an educational (for myself) project but I want to share my steps
towards achieving (or not) this goal.

If you want to try out some of the code, use the tags below, anything else is
probably WIP and may not do anything useful :)

## Version 4 (TODO/WIP)

### Thoughts

 * Will build a new chassis, with new motors, and target wheel size.
 * If encoders are high enough resolution I will retry the closed-loop
   control, if not will instead use the behaviour (ie oscillating, running
   away) to self tune a little.
 * Will move control board to bottom to simplify wiring and to stop instances
   where I tap the "head" back and forward while tuning from saturating the
   accelerometers which caused it to lose an accurate "zero" inclination.

## Version 3

See [Tag 3.0](https://github.com/thisismyrobot/balancing-robot/releases/tag/3.0)

![](doc/build%203.gif) ![](doc/build%203.jpg)

### Hardware

 * HappyModel SP Racing F3 EVO Brushed Micro FC - running BetaFlight 3.1.7 with minimal configuration except for fast serial response.
 * 2 x Solarbotics L293D Secret Motor Driver
 * Heltec Automation WiFi Kit 32 (an ESP32 board) for control
 * ~~2 x DG01D-E 48:1 DC motor w/wheel, 3-9V~~ 2 x 48.75:1 DC motor w/wheel, 3-12V (search for "Arduino DC Motor with Wheel and Rubber Tyres") + bodged-on encoders from DG01D-E, as the DG01D-E is just not powerful enough. Measurements suggest 1/3 of the power.
 * Pololu 5V, 2.5A Step-Down Voltage Regulator D24V22F5
 * 3S LiPo (450mAh 45C)

### Thoughts

Not a great success. Switching to an ESP32 was a good, but implementing
closed-loop motor control didn't improve motor speed accuracy as the encoder
counts per revolution were too low. As a consequence a slower PID loop
frequency was needed to ensure enough counts were taken to correct the speed,
but that introduced lag in response.

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
