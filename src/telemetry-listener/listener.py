import socket

import msgpack


UDP_IP = "0.0.0.0"
UDP_PORT = 9001


def listen():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    packets = 0
    while True:
        data = sock.recvfrom(1024)[0]
        message = msgpack.unpackb(data, use_list=False)
        count, rate, pitch = message
        packets += 1
        print(f'Packet: {packets}, Count: {count}, Rate: {rate}Hz, Pitch: {pitch}°')


if __name__ == '__main__':
    listen()