import glob
from othello_py import othello

def digit(n, r):
    n = str(n)
    l = len(n)
    for i in range(r - l):
        n = '0' + n
    return n

def notation_to_coord(notation):
    y = int(notation[1]) - 1
    x = ord(notation[0].lower()) - ord('a')
    return y, x

directory = int(input())
directory_filled = digit(directory, 7)

files = glob.glob('./self_play/' + directory_filled + '/*')
print(files)

for file in files:
    with open(file, 'r') as f:
        records = f.read().splitlines()
    for record in records:
        o = othello()
        for i in range(0, len(record), 2):
            notation = record[i:i + 2]
            y, x = notation_to_coord(notation)
            if not othello.move(y, x):
                o.do_pass()
                othello.move(y, x)