import subprocess

for num, ply_strt in enumerate([10, 20, 30, 40, 50]):       ############################################
    ply_end = ply_strt + 10
    subprocess.run(('python output_model.py ' + str(ply_strt) + '_' + str(ply_end) + '.h5 ' + str(num) + '.txt').split())
subprocess.run('python join_model.py 0.txt 1.txt 2.txt 3.txt 4.txt 5.txt'.split())