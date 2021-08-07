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
import serial
import time


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


def manual_entry():
    mass_kg = question('Mass of weight in kilograms', 0.05)
    diameter_m = question('Wheel diameter in metres', 0.0665)
    voltage_v = question('Voltage when wheel moved')

    calculate(mass_kg, diameter_m, voltage_v)


def measure_motor(port='COM1'):
    """Assumes Pololu qik 2s9v1 attached to COM1 and the motor to M0."""
    speed = 0

    with serial.Serial('/dev/ttyS0', 38400, timeout=1) as conn:

        # 8-bit PWM, 3.9 kHz.
        conn.write(bytearray([0x84, 0x01, 0x03, 0x55, 0x2A]))

        # Increase until move.
        while True:
            print(speed)
            if speed <= 127:
                conn.write(bytearray([0x88, speed]))
            else:
                conn.write(bytearray([0x89, speed - 128]))
            time.sleep(0.1)


if __name__ == '__main__':
    if '--qik' in sys.argv:
        measure_motor()
    else:
        manual_entry()
