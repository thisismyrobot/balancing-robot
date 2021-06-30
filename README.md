# Balancing robot

Using a BetaFlight-equipped flight controller to give the orientation.

## Hardware - Verson 2

WIP

 * HappyModel SP Racing F3 EVO Brushed Micro FC - running BetaFlight 3.1.7 with minimal configuration except for fast serial response.
 * 2 x Solarbotics L293D Secret Motor Driver
 * Arduino Nano
 * 2 x 48.75:1 DC motor w/wheel, 3-12V (search for "Arduino DC Motor with Wheel and Rubber Tyres")
 * Pololu 5V, 2.5A Step-Down Voltage Regulator D24V22F5

## Hardware - Verson 1

See Tag 1.0

![](build%201.jpg)

 * HappyModel SP Racing F3 EVO Brushed Micro FC - running BetaFlight 3.1.7 with minimal configuration.
 * Solarbotics L293D Secret Motor Driver
 * Arduino Nano
 * 48.75:1 DC motor w/wheel, 3-12V (search for "Arduino DC Motor with Wheel and Rubber Tyres")
 * Pololu 5V, 2.5A Step-Down Voltage Regulator D24V22F5
 * 2S and 3S LiPos to test (3S better)

# What makes a good balancing robot

## Height/centre of mass/moment of inertia

Moving and concentrating mass towards the top of the robot will give the
control system more time to react to tipping and reduce the power requirements
of the motors to regain balance.

## Centre of Mass

The Centre of Mass of an object is the average position of the items within
it, weighted by their mass.

For the purposes of approximation, the Centre of Mass of our robot is it's
balance point along its length when laying on it's side.

## Torque Arm

When a solid object has a force put upon it, and that force is not directly
pointing at the Centre of Mass, the object will both move and be torqued
around the Centre of Mass. The line from the Centre of Mass to where the force
is acting is the Torque Arm. The longer the Torque Arm the greater the torque.
The greater the angle between the Torque Arm and the force, the more the force
will contribute to rotation and less to moving. 100% of the force will
contribute to rotation when the force is acting at 90 degrees to the Torque
Arm.

### Moment of Inertia

The Moment of Inertia of an object is its resistance to acceleration around a
pivot, based on its mass and the distribution of its mass compared to the
pivot. Adding mass to an object or moving an object's mass away from a pivot
will both increase the Moment of Inertia.

The greater the Moment of Inertia a mass has around a pivot, the more
resistant it will be to forces trying to accelerate its rotation around around
that pivot.

### Tipping

When the robot is stationary and starting to tip, it is because gravity is
applying a vertical force that is resulting in a torque around a pivot (its
wheel contact patches). The higher the Moment of Inertia is around the pivot,
the more slowly it will accelerate in rotation towards the ground.

The benefit of it more slowly accelerating is your control system has more
time to correct.

### Recovering

When a robot moves laterally to correct its balance, two forces are in play.

Firstly, the rotation of the wheels by the motor creates an opposite torque on
the robot that attempts to straighten it. The lighter the robot the more
impact this torque will have. The greater the Moment of Inertia of the robot
around the axle pivot, the less impact the torque will have.

This torque is not generally accounted for in your control system so the less
impact it has the better.

Secondly, the sideways motion of the base of the robot as it attempts to move
the wheels directly under the top is a lateral force on the bottom of the
robot that is attempting to torque the robot to vertical. The Torque Arm is
between the Centre of Mass and the wheel contact patches. Inhibiting this
torque will be the Moment of Inertia of that rotation around the Centre of
Mass. The higher up the robot the centre of mass is and/or the more
concentrated it is at the Centre of Mass (and thus the lower the Moment of
Inertia around the Centre of Mass pivot), the the less lateral force required
to torque the robot back to vertical.

This reduction in required lateral force reduces the power requirements of the
motors.

## Tuning approach

The usual, tune P, the I, then D. Some people say for balancing robots to go
with zero I, some go with I = 10x P. YMMV. D is nearly always a 100th or
1000th of P.

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
