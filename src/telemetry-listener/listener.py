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
        count, rate, pitch_correction, pitch, pid_pitch_output, leftDist, rightDist = message
        packets += 1
        print(f'IP: {ip}, Packet: {packets}, Count: {count}, Rate: {rate}Hz, Pitch Correct: {pitch_correction:.1f}°, Pitch: {pitch:.1f}°, Drive: {int(pid_pitch_output / 2.55)}%, Left: {round(leftDist * 100, 1)}cm, Right: {round(rightDist * 100, 1)}cm')


if __name__ == '__main__':
    listen()
