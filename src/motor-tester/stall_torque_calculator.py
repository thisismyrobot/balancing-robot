"""

A calculator of motor stall torque per volt, using a weight on a string and a
raising voltage.

Firstly the motor voltage is raised until the motor starts moving, then it is
lowered until it stalls.

https://courses.lumenlearning.com/physics/chapter/7-3-gravitational-potential-energy/
https://www.ftexploring.com/energy/power_1.html
http://www.auburn.edu/academic/classes/scmh/1010/Energy%20and%20Work.php
https://planetcalc.com/1908/

"""
import msvcrt
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
    voltage_v = question('Voltage when wheel stalled')

    calculate(mass_kg, diameter_m, voltage_v)


def measure_motor(port='COM8', duty=50, slow_ratio=0.75, rate=1):
    """Assumes Pololu qik 2s9v1 attached to COM8 and the motor to M0."""
    mass_kg = question('Mass of weight in kilograms', 0.05)
    diameter_m = question('Wheel diameter in metres', 0.0479)

    input('Press [Enter] to begin...')
    print('Press [Enter] when the motor starts!', end='', flush=True)

    track_enter = True
    global enter_pressed
    enter_pressed = False
    def space_listener_thread():
        global enter_pressed
        while track_enter:
            input()
            enter_pressed = True
    threading.Thread(target=space_listener_thread, daemon=True).start()

    up = True
    with serial.Serial(port, 38400, timeout=1) as conn:

        # 8-bit PWM, 3.9 kHz.
        conn.write(bytearray([0x84, 0x01, 0x03, 0x55, 0x2A]))

        while True:

            if enter_pressed:
                enter_pressed = False

                if up:
                    print('Press [Enter] when the motor stops!', end='', flush=True)
                    up = False
                    duty = duty * slow_ratio
                    track_enter = False
                else:
                    break

            # M0 speed
            if duty <= 127:
                conn.write(bytearray([0x88, int(duty)]))
            else:
                conn.write(bytearray([0x89, int(duty) - 128]))

            if up:
                duty += rate
            else:
                duty -= rate
            duty = max(0, min(duty, 255))

            time.sleep(0.1)

        stall_point_v = question('Current motor voltage')

        # M0 Coast
        conn.write(bytearray([0x86]))

        calculate(mass_kg, diameter_m, stall_point_v)


if __name__ == '__main__':
    if '--qik' in sys.argv:
        measure_motor()
    else:
        manual_entry()
