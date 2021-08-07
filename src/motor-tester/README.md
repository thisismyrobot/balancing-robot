# Experiments in measuring comparable motor performance

##  stall_torque_calculator.py - measuring stall torque per volt

WIP, but the idea is to ramp voltage until a weight moves, then back-calculate
the stall-torque per volt so it's comparable to values on other spec sheets.

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
