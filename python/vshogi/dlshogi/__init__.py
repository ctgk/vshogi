"""Module of DL-Shogi."""

import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

from vshogi.dlshogi._policy_value_function import PolicyValueFunction


_classes = [
    PolicyValueFunction,
]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = [_cls.__name__ for _cls in _classes]


del _cls
del _classes
