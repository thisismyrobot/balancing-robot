import datetime

import multiwii


def stream(port, baud, duration=None):
    results = 0
    start = None
    if duration is not None:
        duration = datetime.timedelta(seconds=duration)

    board = multiwii.MultiWii(port)
    while True:
        if start is None:
            start = datetime.datetime.now()

        actual_duration = datetime.datetime.now() - start
        if duration is not None and actual_duration > duration:            
            print(f'{results} results in {actual_duration.total_seconds()} = {(results / actual_duration.total_seconds())} Hz')
            break

        print(board.getData(multiwii.MultiWii.ATTITUDE))
        results += 1



if __name__ == '__main__':
    stream('COM4', 115200)
