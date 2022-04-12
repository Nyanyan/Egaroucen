import subprocess

with open('cmd.txt', 'r') as f:
    cmds = f.read().splitlines()

for cmd in cmds:
    subprocess.run(cmd.split())