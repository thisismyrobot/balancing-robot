# Balancing robot

Using a BetaFlight-equipped flight controller to give the orientation.

## Hardware - V1

![Robot](build 1.jpg?raw=true "Robot")

### HappyModel SP Racing F3 EVO Brushed Micro FC

 * Running BetaFlight 3.1.7 with minimal configuration.

### Solarbotics L293D Secret Motor Driver

### Arduino Nano

# What makes a good balancing robot

## Height/centre of mass/moment of inertia

The Moment of Inertia of an object is its resistance to acceleration around a
pivot, based on its mass and the distribution of its mass compared to the
pivot. Adding mass or moving mass away from a pivot will both increase the
Moment of Inertia.

The greater the Moment of Inertia a mass has around a pivot, the more
resistant it will be to forces trying to accelerate its rotation around around
that pivot.

When the robot is stationary and starting to tip, it is because gravity is
applying a vertical force that is resulting in a torque around a pivot (its
wheels). The higher the Moment of Inertia is around the wheels pivot, the
more slowly it will accelerate in rotation towards the ground.

When the robot moves laterally to correct its balance, it is applying a
sideways force at the wheels to rotate the robot around a pivot at its top.
This rotation takes less force (and thus less motor power) if the Moment of
Inertia is low around that pivot.

Moving mass away from the wheels pivot and towards the top pivot will increase
the moment of inertia for tipping over whilst reducing it for the corrective
actions of the motors.

## Tuning approach

The usual, tune P, the I, then D. Some people say for balancing robots to go
with zero I, some go with I = 10x P. YMMV. D is nearly always a 10th or 100th
of P.

## Fast PID control loop time

I got now meaningful balancing performance before hitting over 150Hz loop
time. At 10Hz it was hopeless.

I don't have a good external reference but 1ms loop times (so 1000Hz) are
successful in https://www.youtube.com/watch?v=TN6u-D992sw

## Constant PID control loop time

Using the standard Arduino PID library, variances in PID control loop time are
not taken into account which can impact D term. Keep it as constant as
possible.

## Battery voltage vs. P

Consider raising P by the drop (%) in battery voltage, and vice versa. Motor
voltage is proportional to motor speed so this will help maintain level
performance as a battery sags.
