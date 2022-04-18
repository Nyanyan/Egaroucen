from random import randint, randrange, random
import subprocess
from xml.dom.minidom import Notation
from tqdm import trange
from othello_py import othello

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

def notation_to_coord(notation):
    y = int(notation[1]) - 1
    x = ord(notation[0].lower()) - ord('a')
    return y, x

def coord_to_notation(y, x):
    return chr(x + ord('a')) + str(y + 1)

egaroucen = subprocess.Popen('./../src/test/a.out 8'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)

def self_play():
    file_num = int(input())
    file_name = digit(file_num, 7)
    play_num = int(input())
    random_move_min = 10
    random_move_max = 15
    for num in trange(play_num):
        record = ''
        o = othello()
        n_random_moves = randint(random_move_min, random_move_max)
        for _ in range(n_random_moves):
            have_legal, legals = o.get_legal()
            if not have_legal:
                o.do_pass()
                have_legal, legals = o.get_legal()
                if not have_legal:
                    break
            moves = []
            for y in range(hw):
                for x in range(hw):
                    if legals[y][x]:
                        moves.append([y, x])
            y, x = moves[randrange(0, len(moves))]
            record += coord_to_notation(y, x)
            o.move(y, x)
        while True:
            have_legal, legals = o.get_legal()
            if not have_legal:
                o.do_pass()
                have_legal, legals = o.get_legal()
                if not have_legal:
                    break
            grid_str = str(o.player) + '\n'
            for yy in range(hw):
                for xx in range(hw):
                    grid_str += '0' if o.grid[yy][xx] == 0 else '1' if o.grid[yy][xx] == 1 else '.'
                grid_str += '\n'
            egaroucen.stdin.write(grid_str.encode('utf-8'))
            egaroucen.stdin.flush()
            _, coord = egaroucen.stdout.readline().decode().split()
            record += coord
            y, x = notation_to_coord(coord)
            o.move(y, x)
        with open('self_play/' + file_name + '.txt', 'a') as f:
            f.write(record + '\n')


self_play()
egaroucen.kill()