import subprocess
import sys

argv = sys.argv
if len(argv) != 3:
    print('arg err')
    exit(1)

cmd = 'g++ ' + argv[1] + ' -O3 -march=native -fexcess-precision=fast -funroll-loops -flto -mtune=native -lpthread -Wall -o ' + argv[2]
o = subprocess.run(cmd.split(), encoding='utf-8', stderr=subprocess.STDOUT, timeout=None)
print('------------------compiled------------------')