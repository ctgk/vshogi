import pytest

import vshogi.minishogi as shogi


def test_repr():
    for p in (shogi.B_FU,):
        assert repr(p) == f'BoardPiece.{p.name}'


if __name__ == '__main__':
    pytest.main([__file__])
