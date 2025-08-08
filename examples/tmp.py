from glob import glob

import numpy as np

from dlshogi import read_kifu
from vshogi.dlshogi import PolicyValueFunction
from vshogi.minishogi import *

pv_func = PolicyValueFunction('minishogi_1/models/model_0001.tflite')
tsv_list = glob('minishogi_1/datasets/dataset_0001/record*.tsv')
print(tsv_list[:5])
v_list = []
for tsv_path in tsv_list:
    df = read_kifu(tsv_path=tsv_path)
    for sfen in df['state']:
        print(sfen)
        g = Game(sfen)
        _, v = pv_func(g)
        print(sfen, v)
        v_list.append(v)
print(np.mean(v_list))
