import enum
import msvcrt
import socket
import sys


UDP_PORT = 9001


class Mode(enum.Enum):
    ANGLE = enum.auto()
    PROPORTIONAL = enum.auto()
    INTEGRAL = enum.auto()
    DERIVATIVE = enum.auto()


def main(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    mode = Mode.ANGLE
    print('Angle')

    while True:
        char = msvcrt.getch().decode('utf-8').lower()
        mode_prefix = str(mode).split('.')[1][0]
        message = None
        if char == 'a':
            mode = Mode.ANGLE
            print('Angle')
        elif char == 'p':
            mode = Mode.PROPORTIONAL
            print("Proportional")
        elif char == 'i':
            mode = Mode.INTEGRAL
            print('Integral')
        elif char == 'd':
            mode = Mode.DERIVATIVE
            print('Derivative')
        elif char == '=':
            message = f'{mode_prefix}+'
            print('+')
        elif char == '-':
            message = f'{mode_prefix}-'
            print('-')
        else:
            return

        if message:
            sock.sendto(message.encode('utf-8'), (ip, port))


if __name__ == '__main__':
    ip = sys.argv[-1]
    main(ip, UDP_PORT)

