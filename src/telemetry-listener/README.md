# Telemetry Listener

Prints out telemetry from the robot, received over UDP.

![](streaming.gif)

## PID tuning

An example application is the closed-loop motor control tuning I did using
data from the listener:

![](motor_control_pid_loop_performance_1.PNG)

![](motor_control_pid_loop_performance_2.PNG)

## Install

    pip install pipenv
    pipenv install

## Run

    pipenv run python listener.py
