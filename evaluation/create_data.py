import glob
import subprocess
from tqdm import tqdm
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

directory = int(input('directory: '))
directory_filled = digit(directory, 7)

files = glob.glob('./self_play/' + directory_filled + '/*')
print(files)

exe = subprocess.Popen('./../src/test/create_data.out'.split(), stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr=subprocess.DEVNULL)

for num, file in enumerate(files):
    with open(file, 'r') as f:
        records = f.read().splitlines()
    with open('./data/' + digit(num, 7) + '.txt', 'w') as f:
        for record in tqdm(records):
            o = othello()
            data = []
            for i in range(0, len(record), 2):
                exe.stdin.write(o.create_input_str().encode('utf-8'))
                exe.stdin.flush()
                data.append([o.player, sum(o.n_stones) - 4, exe.stdout.readline().decode().replace('\r', '').replace('\n', '') + '\n'])
                notation = record[i:i + 2]
                y, x = notation_to_coord(notation)
                if not o.move(y, x):
                    o.do_pass()
                    o.move(y, x)
            score = o.n_stones[0] - o.n_stones[1]
            vac = 64 - sum(o.n_stones)
            if score > 0:
                score += vac
            elif score < 0:
                score -= vac
            for player, ply, datum in data:
                if player == 0:
                    f.write(str(ply) + ' ' + str(-score) + ' ' + datum)
                else:
                    f.write(str(ply) + ' ' + str(score) + ' ' + datum)

exe.kill()