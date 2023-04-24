import pytest

import vshogi.minishogi as shogi


def test_repr():
    for sq in (shogi.SQ_11,):
        assert repr(sq) == f'Square.{sq.name}'


if __name__ == '__main__':
    pytest.main([__file__])
