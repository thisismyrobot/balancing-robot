import multiwii


def stream(port):
    board = multiwii.MultiWii(port)
    while True:
        print(board.getData(multiwii.MultiWii.ATTITUDE))


if __name__ == '__main__':
    stream('COM4')
