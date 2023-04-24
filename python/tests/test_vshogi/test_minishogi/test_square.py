import pytest

import vshogi.minishogi as shogi


def test_repr():
    assert repr(shogi.SQ_1A) == 'Square.1A'


if __name__ == '__main__':
    pytest.main([__file__])
