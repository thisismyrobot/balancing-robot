"""

A calculator of motor stall torque per volt, using a weight on a string and a
raising voltage. Just before the motor starts to move is the point you care
about.

https://courses.lumenlearning.com/physics/chapter/7-3-gravitational-potential-energy/
https://www.ftexploring.com/energy/power_1.html
http://www.auburn.edu/academic/classes/scmh/1010/Energy%20and%20Work.php
https://planetcalc.com/1908/

"""
import math


# https://en.wikipedia.org/wiki/Gravity_of_Earth
GRAVITY = 9.80665


def question(text, default=None):
    prompt = text + (f' ({default})' if default else '') + ': '
    answer = input(prompt)
    if answer == '':
        if default:
            answer = default
        else:
            answer = question(text, default)
    return float(answer)


def calculate(mass_kg, diameter_m, voltage_v):
    # F = mg
    force_to_lift_N = mass_kg * GRAVITY

    # τ = LF
    torque_Nm = (diameter_m / 2) * force_to_lift_N
    torque_kgcm = torque_Nm * (100 / GRAVITY);

    # Stall torque increases with voltage, fairly linearly, so we can
    # extrapolate a stall torque per volt value to compare motors by.
    print(f'{torque_Nm:.2f}Nm ({torque_kgcm:.2f}kgf.cm) @ {voltage_v}V => {(torque_Nm/voltage_v):.3f}Nm/V ({(torque_kgcm/voltage_v):.2f}kgf.cm/V)')


if __name__ == '__main__':
    mass_kg = question('Mass of weight in kilograms', 0.05)
    diameter_m = question('Wheel diameter in metres', 0.0665)
    voltage_v = question('Voltage when wheel moved')

    calculate(mass_kg, diameter_m, voltage_v)
