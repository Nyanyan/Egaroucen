from random import randrange
import subprocess
import sys
from tqdm import trange

hw = 8
hw2 = 64
board_index_num = 38
dy = [0, 1, 0, -1, 1, 1, -1, -1]
dx = [1, 0, -1, 0, 1, -1, 1, -1]

def digit(n, r):
    n = str(n)
    l = len(n)
    for i in range(r - l):
        n = '0' + n
    return n

def empty(grid, y, x):
    return grid[y][x] == -1 or grid[y][x] == 2

def inside(y, x):
    return 0 <= y < hw and 0 <= x < hw

def check(grid, player, y, x):
    res_grid = [[False for _ in range(hw)] for _ in range(hw)]
    res = 0
    for dr in range(8):
        ny = y + dy[dr]
        nx = x + dx[dr]
        if not inside(ny, nx):
            continue
        if empty(grid, ny, nx):
            continue
        if grid[ny][nx] == player:
            continue
        #print(y, x, dr, ny, nx)
        plus = 0
        flag = False
        for d in range(hw):
            nny = ny + d * dy[dr]
            nnx = nx + d * dx[dr]
            if not inside(nny, nnx):
                break
            if empty(grid, nny, nnx):
                break
            if grid[nny][nnx] == player:
                flag = True
                break
            #print(y, x, dr, nny, nnx)
            plus += 1
        if flag:
            res += plus
            for d in range(plus):
                nny = ny + d * dy[dr]
                nnx = nx + d * dx[dr]
                res_grid[nny][nnx] = True
    return res, res_grid

def pot_canput_line(arr):
    res_p = 0
    res_o = 0
    for i in range(len(arr) - 1):
        if arr[i] == -1 or arr[i] == 2:
            if arr[i + 1] == 0:
                res_o += 1
            elif arr[i + 1] == 1:
                res_p += 1
    for i in range(1, len(arr)):
        if arr[i] == -1 or arr[i] == 2:
            if arr[i - 1] == 0:
                res_o += 1
            elif arr[i - 1] == 1:
                res_p += 1
    return res_p, res_o

class reversi:
    def __init__(self):
        self.grid = [[-1 for _ in range(hw)] for _ in range(hw)]
        self.grid[3][3] = 1
        self.grid[3][4] = 0
        self.grid[4][3] = 0
        self.grid[4][4] = 1
        self.player = 0 # 0: 黒 1: 白
        self.nums = [2, 2]

    def move(self, y, x):
        plus, plus_grid = check(self.grid, self.player, y, x)
        if (not empty(self.grid, y, x)) or (not inside(y, x)) or not plus:
            print('Please input a correct move')
            return 1
        self.grid[y][x] = self.player
        for ny in range(hw):
            for nx in range(hw):
                if plus_grid[ny][nx]:
                    self.grid[ny][nx] = self.player
        self.nums[self.player] += 1 + plus
        self.nums[1 - self.player] -= plus
        self.player = 1 - self.player
        return 0
    
    def check_pass(self):
        for y in range(hw):
            for x in range(hw):
                if self.grid[y][x] == 2:
                    self.grid[y][x] = -1
        res = True
        for y in range(hw):
            for x in range(hw):
                if not empty(self.grid, y, x):
                    continue
                plus, _ = check(self.grid, self.player, y, x)
                if plus:
                    res = False
                    self.grid[y][x] = 2
        if res:
            #print('Pass!')
            self.player = 1 - self.player
        return res

    def output(self):
        print('  ', end='')
        for i in range(hw):
            print(chr(ord('a') + i), end=' ')
        print('')
        for y in range(hw):
            print(str(y + 1) + ' ', end='')
            for x in range(hw):
                print('○' if self.grid[y][x] == 0 else '●' if self.grid[y][x] == 1 else '* ' if self.grid[y][x] == 2 else '. ', end='')
            print('')
    
    def output_file(self):
        res = ''
        for y in range(hw):
            for x in range(hw):
                res += '*' if self.grid[y][x] == 0 else 'O' if self.grid[y][x] == 1 else '-'
        res += ' *'
        return res

    def end(self):
        if min(self.nums) == 0:
            return True
        res = True
        for y in range(hw):
            for x in range(hw):
                if self.grid[y][x] == -1 or self.grid[y][x] == 2:
                    res = False
        return res
    
    def judge(self):
        if self.nums[0] > self.nums[1]:
            #print('Black won!', self.nums[0], '-', self.nums[1])
            return 0
        elif self.nums[1] > self.nums[0]:
            #print('White won!', self.nums[0], '-', self.nums[1])
            return 1
        else:
            #print('Draw!', self.nums[0], '-', self.nums[1])
            return -1


def record_translate(record):
    res = []
    for i in range(0, len(record), 2):
        x = ord(record[i]) - ord('a')
        y = int(record[i + 1]) - 1
        res.append([y, x])
    return res

def record_rev_translate(y, x):
    x_str = chr(ord('a') + x)
    y_str = str(y + 1)
    return x_str + y_str

with open('third_party/extract_prominence.txt', 'r') as f:
    data = [elem.split() for elem in f.read().splitlines()]
tactic = {}
for r, p in data:
    if r in tactic:
        tactic[r].append(p)
    else:
        tactic[r] = [p]
print(len(tactic))

ais = []
evaluate = subprocess.Popen('./ai.out'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)

def init_ai():
    global ais
    ais = [subprocess.Popen('./ai2.out'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL) for _ in range(2)]
    ais[0].stdin.write('0\n'.encode('utf-8'))
    ais[1].stdin.write('1\n'.encode('utf-8'))

init_ai()

def self_play():
    play_num = int(input())
    start_num = int(input())
    for num in trange(play_num):
        #print('=', end='', file=sys.stderr, flush=True)
        record = ''
        data = []
        rv = reversi()
        y = 4
        x = 5
        rv.move(y, x)
        record += record_rev_translate(y, x)
        while True:
            if rv.check_pass() and rv.check_pass():
                break
            if not record in tactic:
                break
            tactic_idx = randrange(len(tactic[record]))
            y = int(tactic[record][tactic_idx][1]) - 1
            x = ord(tactic[record][tactic_idx][0]) - ord('a')
            grid_str = ''
            for yy in range(hw):
                for xx in range(hw):
                    grid_str += '0' if rv.grid[yy][xx] == 0 else '1' if rv.grid[yy][xx] == 1 else '.'
                grid_str += '\n'
            evaluate.stdin.write((str(rv.player) + '\n' + grid_str).encode('utf-8'))
            evaluate.stdin.flush()
            add_data = evaluate.stdout.readline().decode().replace('\r\n', '')
            data.append(grid_str.replace('\n', '') + ' ' + str(rv.player) + ' ' + add_data)
            rv.move(y, x)
            record += record_rev_translate(y, x)
        break_flag = False
        while True:
            if rv.check_pass() and rv.check_pass():
                break
            try:
                grid_str = ''
                for yy in range(hw):
                    for xx in range(hw):
                        grid_str += '0' if rv.grid[yy][xx] == 0 else '1' if rv.grid[yy][xx] == 1 else '.'
                    grid_str += '\n'
                #print(grid_str)
                ais[rv.player].stdin.write(grid_str.encode('utf-8'))
                ais[rv.player].stdin.flush()
                y, x, score = ais[rv.player].stdout.readline().split()
                evaluate.stdin.write((str(rv.player) + '\n' + grid_str).encode('utf-8'))
                evaluate.stdin.flush()
                add_data = evaluate.stdout.readline().decode().replace('\r\n', '')
                y = int(y)
                x = int(x)
                score = float(score)
                if rv.player == 1:
                    score = -score
                #data.append(grid_str.replace('\n', '') + ' ' + str(rv.player) + ' ' + add_data + ' ' + str(score))
                data.append(grid_str.replace('\n', '') + ' ' + str(rv.player) + ' ' + add_data)
            except:
                print('err')
                break_flag = True
                for j in range(2):
                    try:
                        ais[j].kill()
                    except:
                        continue
                init_ai()
                break
            rv.move(y, x)
            record += record_rev_translate(y, x)
        if not break_flag:
            result = rv.nums[0] - rv.nums[1]
            vacant = hw2 - rv.nums[0] - rv.nums[1]
            if result > 0:
                result += vacant
            elif result < 0:
                result -= vacant
            '''
            with open('data/' + digit(start_num + num // 1000, 7) + '.txt', 'a') as f:
                for datum in data:
                    f.write(datum + ' ' + str(result) + '\n')
            '''
            with open('self_play/' + digit(start_num + num // 1000, 7) + '.txt', 'a') as f:
                f.write(record + '\n')


self_play()
for j in range(2):
    try:
        ais[j].kill()
    except:
        continue