import pytest

import cshogi as cs


def test_legal_moves():
    board = cs.Board()
    assert len(board.legal_moves) == 30
    actual = sorted(list(map(cs.move_to_usi, board.legal_moves)))
    assert len(actual) == 30
    assert actual[0] == '1g1f'
    assert actual[-1] == '9i9h'


def test_sfen1():
    board = cs.Board()
    sfen = 'lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1'
    assert board.sfen() == sfen


def test_sfen2():
    board = cs.Board()
    moves = [67470]
    for move in moves:
        board.push(move)
    sfen = 'lnsgkgsnl/1r5b1/ppppppppp/9/9/7P1/PPPPPPP1P/1B5R1/LNSGKGSNL w - 2'
    assert board.sfen() == sfen


def test_sfen3():
    board = cs.Board()
    moves = [
        67470, 68117, 67341, 328980, 73275, 264467, 265506, 269102, 465597,
        464549, 68631, 65795, 66309, 528924, 72114, 73922, 270132, 74051,
        336700, 75083, 268851, 71600, 70953, 268079, 266538, 72761, 463275,
        204728, 529972, 268199, 531006, 401454, 198808, 71729, 1120561, 1252401,
        5594644, 13829268, 267570, 10936, 10912, 5577760, 5312672, 72890, 70824,
        1121595, 4264999, 219324, 11740092, 3235260, 9903804, 10417, 10938,
        10928, 5578032, 5642032, 268584, 399417, 10426, 1449146, 10427, 400695,
        10797, 400302, 7624357, 13045157, 67212, 1115532, 1135526, 9900710,
        10655, 3344927, 3345439, 10405, 7557030, 11604646, 10407, 1119015,
        10655, 10789, 11192, 399149, 11182, 10651, 10938, 10662, 1315879, 10910,
        466493, 10674, 4658981, 7867941, 3362726, 11734438, 3414450, 4266162,
        10655, 10780, 10673, 3544753, 6558641, 10675, 532277, 11180, 7804332,
        7559596, 12065452, 10411, 1443883, 10410, 1447338, 10409, 398607, 10675,
        529954, 11178, 528666, 4395825, 11071, 10788, 10790, 4525862, 5459878,
        11800102, 4545830, 14160550, 4612004, 10917, 10926, 529191, 6165029,
        10662, 4068006,
    ]
    for move in moves:
        board.push(move)
    sfen = 'l7l/3B2s2/2G1+Rp3/p3k1ppp/1p1n5/2PppPP1P/PP1ng2R1/2G6/L5K1L w BG2S2N3Psp 134'
    assert board.sfen() == sfen


if __name__ == '__main__':
    pytest.main([__file__])
