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

inf = 10000000.0

files = glob.glob('./data/*')
'''
for (i = 0; i < pattern_size; ++i){
    digit = (idx / pow3[pattern_size - 1 - i]) % 3;
    if (digit == 0){
        arr[i] = 1.0;
        arr[pattern_size + i] = 0.0;
    } else if (digit == 1){
        arr[i] = 0.0;
        arr[pattern_size + i] = 1.0;
    } else{
        arr[i] = 0.0;
        arr[pattern_size + i] = 0.0;
    }
}
'''

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

for ply_strt in [0, 10, 20, 30, 40, 50]:
    ply_end = ply_strt + 10

    game_num = 2000
    test_ratio = 0.1
    n_epochs = 1
    
    all_data = []
    all_labels = []
    
    for file in files:
        with open(file, 'r') as f:
            data = [[int(elem) for elem in line.split()] for line in f.read().splitlines()]
        for datum in data:
            if ply_strt <= datum[0] < ply_end:
                score = datum[1]
                all_labels.append(score)
                idxes = datum[2:]
                one_hot_idxes = []
                for pattern_idx in range(54):
                    one_hot_idx = [0.0 for _ in range(pattern_sizes[pattern_idx] * 2)]
                    for i in range(pattern_sizes[pattern_idx]):
                        digit = (idxes[pattern_idx] // (3 ** (pattern_sizes[pattern_idx] - 1 - i))) % 3
                        if digit == 0:
                            one_hot_idx[i] = 1.0
                            one_hot_idx[pattern_sizes[pattern_idx] + i] = 0.0
                        elif digit == 1:
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
                all_data.append(one_hot_idxes)


    '''
    x = [None for _ in range(ln_in)]
    ys = []
    names = ['line2', 'line3', 'line4', 'diagonal5', 'diagonal6', 'diagonal7', 'diagonal8', 'edge2X', 'triangle', 'edgeblock', 'cross']
    idx = 0
    for i in range(len(pattern_idx)):
        layers = []
        layers.append(Dense(16, name=names[i] + '_dense0'))
        layers.append(LeakyReLU(alpha=0.01))
        layers.append(Dense(16, name=names[i] + '_dense1'))
        layers.append(LeakyReLU(alpha=0.01))
        layers.append(Dense(1, name=names[i] + '_out'))
        layers.append(LeakyReLU(alpha=0.01))
        add_elems = []
        for j in range(len(pattern_idx[i])):
            x[idx] = Input(shape=len(pattern_idx[i][0]) * 2, name=names[i] + '_in_' + str(j))
            tmp = x[idx]
            for layer in layers:
                tmp = layer(tmp)
            add_elems.append(tmp)
            idx += 1
        ys.append(Add()(add_elems))
    y_pattern = Concatenate(axis=-1)(ys)
    x[idx] = Input(shape=3, name='additional_input')
    y_add = Dense(8, name='add_dense0')(x[idx])
    y_add = LeakyReLU(alpha=0.01)(y_add)
    y_add = Dense(8, name='add_dense1')(y_add)
    y_add = LeakyReLU(alpha=0.01)(y_add)
    y_all = Concatenate(axis=-1)([y_pattern, y_add])
    y_all = Dense(1, name='all_dense0')(y_all)

    model = Model(inputs=x, outputs=y_all)
    '''
    model = load_model('learned_data/bef_' + str(stone_strt) + '_' + str(stone_end) + '.h5')

    #model.summary()
    #plot_model(model, to_file='model.png', show_shapes=True)

    model.compile(loss='mse', metrics='mae', optimizer='adam')
    #model.save('learned_data/' + str(stone_strt) + '_' + str(stone_end) + '.h5')
    
    #model.compile(loss=my_loss, metrics='mae', optimizer='adam')

    for i in trange((game_num + 99) // 100):
        collect_data(i)
    len_data = len(all_labels)
    print(len_data)

    tmp_data = deepcopy(all_data)
    tmp_labels = deepcopy(all_labels)
    all_data = [[] for _ in range(len(tmp_data))]
    all_labels = []
    shuffled = list(range(len_data))
    shuffle(shuffled)
    for i in shuffled:
        all_labels.append(tmp_labels[i])
        for j in range(len(tmp_data)):
            all_data[j].append(tmp_data[j][i])

    all_data = [np.array(arr) for arr in all_data]
    all_labels = np.array(all_labels)

    n_train_data = int(len_data * (1.0 - test_ratio))
    n_test_data = int(len_data * test_ratio)

    train_data = [arr[0:n_train_data] for arr in all_data]
    train_labels = all_labels[0:n_train_data]
    test_data = [arr[n_train_data:len_data] for arr in all_data]
    test_labels = all_labels[n_train_data:len_data]


    print(model.evaluate(test_data, test_labels))
    early_stop = EarlyStopping(monitor='val_loss', patience=5)
    model_checkpoint = ModelCheckpoint(filepath=os.path.join('learned_data/' + str(ply_strt) + '_' + str(ply_end), 'model_{epoch:02d}_{val_loss:.5f}_{val_mae:.5f}.h5'), monitor='val_loss', verbose=1)
    reduce_lr = ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=2, min_lr=0.0001)
    history = model.fit(train_data, train_labels, epochs=n_epochs, validation_data=(test_data, test_labels), callbacks=[early_stop, model_checkpoint])

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