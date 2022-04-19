import subprocess
import shutil
import os

def digit(n, r):
    n = str(n)
    l = len(n)
    for i in range(r - l):
        n = '0' + n
    return n

start_num = int(input('start num: '))

for num in range(start_num, start_num + 100):

    subprocess.run('python self_play_parallel.py'.split())
    
    shutil.rmtree('data')
    os.mkdir('data')
    subprocess.run('python create_data.py'.split())

    subprocess.run('python learn.py'.split())

    subprocess.run('python output_model_all.py'.split())
    
    subprocess.run(('python clearnup.py ' + str(num)).split())
