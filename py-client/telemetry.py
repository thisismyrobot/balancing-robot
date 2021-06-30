import datetime
import time

import multiwii


def stream(port, baud, duration=None):
    results = 0
    start = None
    if duration is not None:
        duration = datetime.timedelta(seconds=duration)

    board = multiwii.MultiWii(port, baud)
    while True:
        if start is None:
            start = datetime.datetime.now()

        actual_duration = datetime.datetime.now() - start
        if duration is not None and actual_duration > duration:
            print(f'{results} results in {actual_duration.total_seconds()} = {(results / actual_duration.total_seconds())} Hz')
            break

        data = board.getData(multiwii.MultiWii.ATTITUDE)
        #print(data)
        if data is not None:
            results += 1


if __name__ == '__main__':
    stream('COM4', 38400, 10)
