"""Module of DL-Shogi."""

import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

from vshogi.dlshogi._data import Data
from vshogi.dlshogi._network._network import PolicyValueNetwork
from vshogi.dlshogi._policy_value_function import PolicyValueFunction
from vshogi.dlshogi._read_kifu import read_kifu
from vshogi.dlshogi._replay_buffer import ReplayBuffer
from vshogi.dlshogi._train import train


_classes = [
    Data,
    PolicyValueFunction,
    PolicyValueNetwork,
    ReplayBuffer,
]
_functions = [
    read_kifu,
    train,
]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes] + [
    _f.__name__ for _f in _functions
]


del _cls
del _classes
del _functions
