from math import tan, atan
import subprocess

play_num = 1000
parallel_num = 4
parallel_exe_num = play_num // parallel_num

self_plays = [subprocess.Popen('python self_play.py'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=None) for _ in range(parallel_num)]

def self_play():
    for i in range(parallel_num):
        self_plays[i].stdin.write((str(parallel_exe_num) + '\n' + str(i * parallel_exe_num // 100) + '\n').encode('utf-8'))
        self_plays[i].stdin.flush()
    for i in range(parallel_num):
        self_plays[i].wait()


self_play()

for i in range(parallel_num):
    self_plays[i].kill()
