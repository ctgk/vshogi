"""Module of DL-Shogi."""

import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
os.environ['TF_USE_LEGACY_KERAS'] = '1'

from vshogi.dlshogi._network import build_policy_value_network
from vshogi.dlshogi._policy_value_function import PolicyValueFunction


_classes = [
    PolicyValueFunction,
]
_functions = [
    build_policy_value_network,
]


for _cls in _classes:
    _cls.__module__ = __name__


__all__ = (
    [_cls.__name__ for _cls in _classes]
    + [_f.__name__ for _f in _functions]
)


del _cls
del _classes
del _functions
