from copy import deepcopy
import tensorflow as tf
from tensorflow.keras.datasets import boston_housing
from tensorflow.keras.layers import Activation, Add, BatchNormalization, Conv2D, Dense, GlobalAveragePooling2D, Input, concatenate, Flatten, Dropout, Lambda, LeakyReLU, Concatenate
from tensorflow.keras.models import Sequential, Model, load_model
from tensorflow.keras.callbacks import EarlyStopping, LearningRateScheduler, LambdaCallback, ModelCheckpoint
from tensorflow.keras.optimizers import Adam
#from keras.layers.advanced_activations import LeakyReLU
from tensorflow.keras.regularizers import l2
from tensorflow.keras.utils import plot_model
from tensorflow.keras.constraints import max_norm
from tensorflow.keras.callbacks import ReduceLROnPlateau
import numpy as np
import matplotlib.pyplot as plt
from tqdm import trange
from random import randrange, shuffle
import subprocess
import datetime
import os
from math import tanh, log
from copy import deepcopy
import glob
from tqdm import tqdm

def digit(n, r):
    n = str(n)
    l = len(n)
    for i in range(r - l):
        n = '0' + n
    return n

inf = 10000000.0

files = glob.glob('./data/*')

pattern_sizes = [
    8, 8, 8, 8, 
    8, 8, 8, 8, 
    8, 8, 8, 8, 
    5, 5, 5, 5, 
    6, 6, 6, 6, 
    7, 7, 7, 7, 
    8, 8, 
    10, 10, 10, 10, 
    10, 10, 10, 10, 
    10, 10, 10, 10, 
    10, 10, 10, 10, 
    9, 9, 9, 9, 
    10, 10, 10, 10, 
    10, 10, 10, 10
]

input_pattern_sizes = [
    16, 16, 16, 16, 
    16, 16, 16, 16, 
    16, 16, 16, 16, 
    10, 10, 10, 10, 
    12, 12, 12, 12, 
    14, 14, 14, 14, 
    16, 16, 
    20, 20, 20, 20, 
    20, 20, 20, 20, 
    20, 20, 20, 20, 
    20, 20, 20, 20, 
    18, 18, 18, 18, 
    20, 20, 20, 20, 
    20, 20, 20, 20, 
    
    16, 16, 16, 16, 
    16, 16, 16, 16, 
    16, 16, 16, 16, 
    16, 16, 16, 16
]

same_pattern_num = [
    4, 4, 4, 
    4, 4, 4, 2, 
    4, 4, 4, 4, 
    4, 4, 4, 
    
    4, 4, 4, 4
]

#for ply_strt in reversed([0, 10, 20, 30, 40, 50]):
for ply_strt in reversed([10, 20, 30, 40, 50]):
    ply_end = ply_strt + 10

    game_num = 2000
    test_ratio = 0.1
    n_epochs = 30
    
    all_data = []
    
    '''
    x = [None for _ in range(70)]
    ys = []
    names = [chr(ord('a') + i) for i in range(18)]
    idx = 0
    for i in range(18):
        layers = []
        layers.append(Dense(16, name=names[i] + '_dense0'))
        layers.append(LeakyReLU(alpha=0.01))
        layers.append(Dense(8, name=names[i] + '_dense1'))
        layers.append(LeakyReLU(alpha=0.01))
        layers.append(Dense(1, name=names[i] + '_out'))
        add_elems = []
        for j in range(same_pattern_num[i]):
            x[idx] = Input(shape=input_pattern_sizes[idx], name=names[i] + '_in_' + str(j))
            tmp = x[idx]
            for layer in layers:
                tmp = layer(tmp)
            add_elems.append(tmp)
            idx += 1
        ys.append(Add()(add_elems))
    y_all = Add(name='out')(ys)

    model = Model(inputs=x, outputs=y_all)
    '''
    
    model = load_model('learned_data/bef_' + str(ply_strt) + '_' + str(ply_end) + '.h5')

    #model.summary()
    #plot_model(model, to_file='model.png', show_shapes=True)

    model.compile(loss='mse', metrics='mae', optimizer='adam')
    #model.save('learned_data/' + str(stone_strt) + '_' + str(stone_end) + '.h5')
    
    #model.compile(loss=my_loss, metrics='mae', optimizer='adam')
    
    for file in tqdm(files):
        with open(file, 'r') as f:
            data = [[int(elem) for elem in line.split()] for line in f.read().splitlines()]
        for datum in data:
            if ply_strt <= datum[0] < ply_end:
                score = datum[1]
                idxes = datum[2:]
                one_hot_idxes = []
                for pattern_idx in range(54):
                    one_hot_idx = [0.0 for _ in range(pattern_sizes[pattern_idx] * 2)]
                    for i in range(pattern_sizes[pattern_idx]):
                        dig = (idxes[pattern_idx] // (3 ** (pattern_sizes[pattern_idx] - 1 - i))) % 3
                        if dig == 0:
                            one_hot_idx[i] = 1.0
                            one_hot_idx[pattern_sizes[pattern_idx] + i] = 0.0
                        elif dig == 1:
                            one_hot_idx[i] = 0.0
                            one_hot_idx[pattern_sizes[pattern_idx] + i] = 1.0
                        else:
                            one_hot_idx[i] = 0.0
                            one_hot_idx[pattern_sizes[pattern_idx] + i] = 0.0
                    one_hot_idxes.append(one_hot_idx)
                for pattern_idx in range(54, 54 + 16):
                    one_hot_idx = [float((idxes[pattern_idx] >> i) & 1) for i in range(16)]
                    one_hot_idxes.append(one_hot_idx)
                #for arr in one_hot_idxes:
                #    print(arr)
                #exit()
                all_data.append([score, one_hot_idxes])
    
    len_data = len(all_data)
    print('n_data:', len_data)
    
    shuffle(all_data)

    all_problem = [np.array([problem[i] for _, problem in all_data]) for i in range(70)]
    all_labels = np.array([score for score, _ in all_data])
    
    print('converted')
    
    n_train_data = int(len_data * (1.0 - test_ratio))
    n_test_data = int(len_data * test_ratio)

    train_data = [arr[0:n_train_data] for arr in all_problem]
    train_labels = all_labels[0:n_train_data]
    test_data = [arr[n_train_data:len_data] for arr in all_problem]
    test_labels = all_labels[n_train_data:len_data]


    print(model.evaluate(test_data, test_labels))
    early_stop = EarlyStopping(monitor='val_loss', patience=5)
    model_checkpoint = ModelCheckpoint(filepath=os.path.join('learned_data/' + str(ply_strt) + '_' + str(ply_end), 'model_{epoch:02d}_{val_loss:.5f}_{val_mae:.5f}.h5'), monitor='val_loss', verbose=1)
    reduce_lr = ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=2, min_lr=0.0001)
    history = model.fit(train_data, train_labels, batch_size=256, epochs=n_epochs, validation_data=(test_data, test_labels), callbacks=[early_stop])

    now = datetime.datetime.today()
    print(str(now.year) + digit(now.month, 2) + digit(now.day, 2) + '_' + digit(now.hour, 2) + digit(now.minute, 2))
    model.save('learned_data/' + str(ply_strt) + '_' + str(ply_end) + '.h5')

'''
for key in ['loss', 'val_loss']:
    plt.plot(history.history[key], label=key)
plt.xlabel('epoch')
plt.ylabel('loss')
plt.legend(loc='best')
plt.savefig('graph/loss.png')
plt.clf()
'''