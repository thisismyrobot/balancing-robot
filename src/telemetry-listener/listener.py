import socket
import sys
import time

import msgpack


UDP_IP = "0.0.0.0"
UDP_PORT = 9001

# Uncomment to include in CSV output.
PAYLOAD = {
#    0: ('IP', '{}'),
#    1: ('Packet', '{}'),
#    2: ('Count', '{}'),
#    3: ('Rate (Hz)', '{}'),
    4: ('Pitch Correct (°)', '{:.2f}'),
    5: ('Pitch (°)', '{:.2f}'),
    6: ('PID output (m/s)', '{:.3f}'),
    7: ('Left Command (m/s)', '{:.3f}'),
#    8: ('Right Command (m/s)', '{:.3f}'),
    9: ('Left Actual (m/s)', '{:.3f}'),
#    10: ('Right Actual (m/s)', '{:.3f}'),
}


def listen():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    print(','.join([p[0] for p in PAYLOAD.values()]))

    packets = 0
    while True:
        data, (ip, port) = sock.recvfrom(1024)
        message = msgpack.unpackb(data, use_list=False)

        packets += 1
        payload = (ip, packets) + message

        values = []
        for index, (_, template) in PAYLOAD.items():
            values.append(template.format(payload[index]))

        print(','.join(values))


if __name__ == '__main__':
    listen()
