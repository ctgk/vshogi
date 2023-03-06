from glob import glob
import os

import pytest

import vshogi
from vshogi import KIF


def test_read():
    filepath = os.path.join(os.path.dirname(__file__), '..', 'assets', 'kifu_example.kif')
    kif = KIF.Parser.parse_file(filepath)
    assert kif.names == ['Player1 初段', 'Player2 二段']
    assert kif.comment == ''
    assert len(kif.moves) == 133
    assert kif.moves[0] == 67470
    assert len(kif.comments) == 131
    assert kif.comments[0] == '▲備考：居飛車'
    assert kif.comments[1] is None
    assert kif.comments[31] == '△戦型：右四間飛車'
    assert kif.win == vshogi.BLACK_WIN
    assert kif.endgame == '%TORYO'


if __name__ == '__main__':
    pytest.main([__file__])
