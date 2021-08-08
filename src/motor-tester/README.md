# Experiments in measuring comparable motor performance

##  stall_torque_calculator.py - measuring stall torque per volt

The idea is to ramp voltage until a weight starts to lift, then slow down
until it stops lifting, measure the voltage and use the mass and lifting wheel
diameter to determine the applied torque when it stalled.

Stall torque increases with voltage, fairly linearly, so we can apply the
ratio of the stall torque and voltage to estimate a stall torque per a spec
sheet volt value to compare motors by even if the voltage at which their stall
torque is specified at differs.

### DG01D-A130GEARMOTOR

pipenv run python stall_torque_calculator.py --qik

    Mass of weight in kilograms (0.05): 0.052
    Wheel diameter in metres (0.0479):
    Press [Enter] to begin...
    Press [Enter] when the motor starts!
    Press [Enter] when the motor stops!
    Current motor voltage: 0.69
    0.01Nm (0.12kgf.cm) @ 0.69V => 0.01770Nm/V (0.1805kgf.cm/V)

Which is pretty accurate, because 0.1805 * 4.5V = 0.81225kgf.cm and the spec
says the stall torque is 0.8kgf.cm at 4.5V for this motor.

## tester.py - measuring motor power

![](DG01D-E.gif)

### DG01D-A130GEARMOTOR

py tester.py

    Mass of weight in kilograms (0.05):
    Wheel diameter in metres (0.0665):
    Wheel rotations (2):
    Duration of lift in seconds: 0.65
    Battery voltage (5):
    0.10Nm (1.04kgf.cm) & 0.32W @ 5.0V & 184.62RPM

### DG01D-E

py tester.py

    Mass of weight in kilograms (0.05):
    Wheel diameter in metres (0.0665):
    Wheel rotations (2): 1
    Duration of lift in seconds: 0.9666666666666667
    Battery voltage (5):
    0.10Nm (1.04kgf.cm) & 0.11W @ 5.0V & 62.07RPM
