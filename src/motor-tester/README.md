# Experiments in measuring comparable motor performance

##  stall_torque_calculator.py - measuring stall torque per volt

![](stall_torque_setup.jpg)

The idea is to ramp voltage until a weight starts to lift, then slow down
until it stops lifting, measure the voltage and use the mass and lifting wheel
diameter to determine the applied torque when it stalled.

Stall torque increases with voltage, fairly linearly, so we can apply the
ratio of the stall torque and voltage to estimate a stall torque per a spec
sheet volt value to compare motors by even if the voltage at which their stall
torque is specified at differs.

### DG01D-A130GEARMOTOR

This motor + gearbox is spec'd to be 0.8kgf.cm @ 4.5V. As a ratio that is
0.17778 kgf.cm/V.

pipenv run python stall_torque_calculator.py --qik

    Mass of weight in kilograms (0.052):
    Wheel diameter in metres (0.0479):
    Supply voltage (5): 5.1
    Press [Enter] to begin...
    Press [Enter] when the motor starts!
    Press [Enter] when the motor stops!
    0.01Nm (0.12kgf.cm) @ 0.92V => 0.01328Nm/V (0.1354kgf.cm/V)

This is quite different to spec, but I think that the voltage was too far away
from the nominal, and more weight seems to improve the accuracy:

pipenv run python stall_torque_calculator.py --qik

    Mass of weight in kilograms (0.052): 0.16
    Wheel diameter in metres (0.0479):
    Supply voltage (5): 5.1
    Press [Enter] to begin...
    Press [Enter] when the motor starts!
    Press [Enter] when the motor stops!
    0.04Nm (0.38kgf.cm) @ 2.00V => 0.01879Nm/V (0.1916kgf.cm/V)

pipenv run python stall_torque_calculator.py --qik

    Mass of weight in kilograms (0.052): 0.284
    Wheel diameter in metres (0.0479):
    Supply voltage (5): 5.1
    Press [Enter] to begin...
    Press [Enter] when the motor starts!
    Press [Enter] when the motor stops!
    0.07Nm (0.68kgf.cm) @ 3.44V => 0.01939Nm/V (0.1977kgf.cm/V)

These two results are very close, despite a (~75% increase in weight), and
their average (0.19465) is pretty accurate, because 0.19465 * 4.5V =
0.875925kgf.cm and, as above, the spec says the stall torque is 0.8kgf.cm at
4.5V for this motor.

I think there is potential in this technique of estimating comparable stall
torque between different tested motors and spec sheets, as long as the stall
voltages are reasonable close.

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
