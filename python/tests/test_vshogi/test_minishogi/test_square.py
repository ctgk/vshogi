import pytest

import vshogi.minishogi as shogi


def test_equal():
    assert shogi.SQ_1A == shogi.A1


if __name__ == '__main__':
    pytest.main([__file__])
