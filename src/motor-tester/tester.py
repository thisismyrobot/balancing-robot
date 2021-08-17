"""
A calculator of motor specs from video of the wheel lifting a weight.

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


def calculate(mass_kg, diameter_m, rotations_r, time_s, voltage_v):
    # c = 2πr
    height_m = math.pi * diameter_m * rotations_r

    # W = mgh
    work_J = mass_kg * GRAVITY * height_m

    # P = W/t
    power_W = work_J / time_s

    angular_velocity_rps = rotations_r / time_s

    # ω = 2π * rotations/sec
    angular_velocity_rads = 2 * math.pi * angular_velocity_rps

    # τ = P / ω
    torque_Nm = power_W / angular_velocity_rads

    torque_kgcm = torque_Nm * (100 / GRAVITY)
    angular_velocity_rpm = angular_velocity_rps * 60

    print(f'{torque_Nm:.2f}Nm ({torque_kgcm:.2f}kgf.cm) & {power_W:.2f}W @ {voltage_v}V & {angular_velocity_rpm:.2f}RPM')


if __name__ == '__main__':
    mass_kg = question('Mass of weight in kilograms', 0.05)
    diameter_m = question('Wheel diameter in metres', 0.0665)
    rotations_r = question('Wheel rotations', 2)
    time_s = question('Duration of lift in seconds')
    voltage_v = question('Battery voltage', 5)

    calculate(mass_kg, diameter_m, rotations_r, time_s, voltage_v)
