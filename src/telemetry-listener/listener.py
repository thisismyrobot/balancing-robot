import socket
import time

import msgpack


UDP_IP = "0.0.0.0"
UDP_PORT = 9001


def listen():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    packets = 0
    while True:
        data, (ip, port) = sock.recvfrom(1024)
        message = msgpack.unpackb(data, use_list=False)
        (
            count,
            rate,
            pitch_correction,
            pitch,
            pid_pitch_output,
            left_cmd,
            right_cmd,
            left_actual,
            right_actual
        ) = message
        packets += 1

        output = ','.join((
            f'IP: {ip}',
            f'Packet: {packets}',
            f'Count: {count}',
            f'Rate (Hz): {rate}',
            f'Pitch Correct (°): {pitch_correction:.2f}',
            f'Pitch (°): {pitch:.2f}',
            f'PID output (m/s): {pid_pitch_output:.3f}',
            f'Left (m/s): {left_cmd:.3f}, {left_actual:.3f}',
            f'Right (m/s): {right_cmd:.3f}, {right_actual:.3f}',
        ))
        print(output)


if __name__ == '__main__':
    listen()
