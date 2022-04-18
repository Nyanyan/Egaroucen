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

    os.makedirs('learned_data/' + digit(num, 7), exist_ok=True)
    for file in ['1.txt', '2.txt', '3.txt', '4.txt', '5.txt', 'bef_10_20.h5', 'bef_20_30.h5', 'bef_30_40.h5', 'bef_40_50.h5', 'bef_50_60.h5']:      ############################################
        shutil.move('learned_data/' + file, 'learned_data/' + digit(num, 7) + '/' + file)
    for file in ['10_20.h5', '20_30.h5', '30_40.h5', '40_50.h5', '50_60.h5']:       ############################################
        shutil.copy('learned_data/' + file, 'learned_data/' + digit(num, 7) + '/' + file)
        os.rename('learned_data/' + file, 'learned_data/bef_' + file)

    os.makedirs('self_play/' + digit(num, 7), exist_ok=True)
    for file in os.listdir('self_play/'):
        if file[7:] == '.txt':
            shutil.move('self_play/' + file, 'self_play/' + digit(num, 7) + '/' + file)
