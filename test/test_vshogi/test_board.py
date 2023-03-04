import pytest

import cshogi as cs


def test_legal_moves():
    board = cs.Board()
    assert len(board.legal_moves) == 30
    actual = sorted(list(map(cs.move_to_usi, board.legal_moves)))
    assert len(actual) == 30
    assert actual[0] == '1g1f'
    assert actual[-1] == '9i9h'


if __name__ == '__main__':
    pytest.main([__file__])
