# -*- coding: utf-8 -*-
from flask import *
import json
import subprocess
from time import sleep, time

hw = 8

app = Flask(__name__)

ai = subprocess.Popen('./ai.out'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE)

@app.route('/')
def index():
    return render_template('base.html')

@app.route("/start", methods=["POST"])
def start_game():
    req = dict(request.form)
    print(req)
    stdin = req["ai_player"] + '\n'
    print('stdin', stdin)
    ai.stdin.write(stdin.encode('utf-8'))
    ai.stdin.flush()
    print('START', flush=True)
    return jsonify(values=json.dumps({}))

@app.route("/ai", methods=["POST"])
def call_ai():
    req = dict(request.form)
    grid = [[-1 for _ in range(hw)] for _ in range(hw)]
    for y in range(hw):
        for x in range(hw):
            tmp = req[str(y * hw + x)]
            try:
                tmp = int(tmp)
                if -1 <= tmp <= 2:
                    grid[y][x] = tmp
                else:
                    return jsonify(values=json.dumps({"r": -1, "c": -1, "s": 0.0}))
            except:
                return jsonify(values=json.dumps({"r": -2, "c": -1, "s": 0.0}))
    stdin = ''
    for y in range(hw):
        for x in range(hw):
            stdin += '0' if grid[y][x] == 0 else '1' if grid[y][x] == 1 else '.'
        stdin += '\n'
    #print('stdin')
    #print(stdin)
    strt = time()
    ai.stdin.write(stdin.encode('utf-8'))
    ai.stdin.flush()
    r, c, s = [float(i) for i in ai.stdout.readline().decode().strip().split()]
    print('TIME', time() - strt)
    r = int(r)
    c = int(c)
    print('RESULT', r, c, s, flush=True)
    res = {"r": r, "c": c, "s": s}
    return jsonify(values=json.dumps(res))

if __name__ == '__main__':
    print('start python', flush=True)
    app.run(threaded=True)