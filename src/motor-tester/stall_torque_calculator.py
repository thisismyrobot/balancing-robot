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
import msvcrt
import statistics
import sys
import threading
import time

import serial


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
    print(f'{torque_Nm:.2f}Nm ({torque_kgcm:.2f}kgf.cm) @ {voltage_v:.2f}V => {(torque_Nm/voltage_v):.5f}Nm/V ({(torque_kgcm/voltage_v):.4f}kgf.cm/V)')


def manual_entry():
    mass_kg = question('Mass of weight in kilograms', 0.05)
    diameter_m = question('Wheel diameter in metres', 0.0479)
    voltage_v = question('Voltage when wheel moved')

    calculate(mass_kg, diameter_m, voltage_v)


def measure_motor(port='COM8', samples=2, slow_ratio=0.75):
    """Assumes Pololu qik 2s9v1 attached to COM1 and the motor to M0."""
    mass_kg = question('Mass of weight in kilograms', 0.05)
    diameter_m = question('Wheel diameter in metres', 0.0479)
    supply_voltage_v = question('Supply voltage')

    global space_pressed
    space_pressed = False
    def space_listener_thread():
        global space_pressed
        while True:
            ch = msvcrt.getch()
            if ch == b' ':
                space_pressed = True
            else:
                break
            time.sleep(0.01)
    threading.Thread(target=space_listener_thread).start()

    up = True
    move_points_v = []
    motor_voltage_v = 0
    with serial.Serial(port, 38400, timeout=1) as conn:

        # 8-bit PWM, 3.9 kHz.
        conn.write(bytearray([0x84, 0x01, 0x03, 0x55, 0x2A]))

        # Increase until move.
        while True:

            if space_pressed:
                space_pressed = False
                print(f'{motor_voltage_v:.2f}V ({duty}/255)')

                move_points_v.append(motor_voltage_v)
                up = not up
                motor_voltage_v = motor_voltage_v * slow_ratio

            duty = int((motor_voltage_v / supply_voltage_v) * 255.0)
            duty = max(0, min(duty, 255))

            if duty <= 127:
                conn.write(bytearray([0x88, duty]))
            else:
                conn.write(bytearray([0x89, duty - 128]))

            if up and motor_voltage_v < supply_voltage_v:
                motor_voltage_v += 0.02
            elif not up and motor_voltage_v > 0:
                motor_voltage_v -= 0.02

            time.sleep(0.1)

            if len(move_points_v) == samples:
                conn.write(bytearray([0x86]))
                break

    average_move_v = statistics.mean(move_points_v)
    points = ','.join([f'{p:.2f}' for p in move_points_v])
    print(f'{average_move_v:.2f}V over {samples} samples: ({points})')
    calculate(mass_kg, diameter_m, average_move_v)


if __name__ == '__main__':
    if '--qik' in sys.argv:
        measure_motor()
    else:
        manual_entry()
