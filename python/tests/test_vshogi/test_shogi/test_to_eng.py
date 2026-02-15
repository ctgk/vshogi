import pytest

import vshogi.shogi as shogi


@pytest.mark.parametrize(
    'move, sfen, expect',
    [
        (
            '8h2b+',
            '4k4/7b1/9/9/9/9/9/1B7/4K4 b -',
            'Bx22+',
        ),
        (
            '8h2b',
            '4k4/7b1/9/9/9/9/9/1B7/4K4 b -',
            'Bx22=',
        ),
        (
            '8h2b+',
            "4k4/7b1/9/9/9/9/9/1B7/4K4 w r 2",
            'Bx22+',
        ),
        (
            'B*5e',
            '4k4/9/9/9/9/9/9/9/4K4 b B',
            'B*55',
        ),
        (
            'B*5e',
            '4k4/9/9/9/9/9/9/1B7/4K4 b B',
            'B*55',
        ),
        (
            '1a1b',
            'k7+S/9/9/9/9/9/9/9/K8 b -',
            '+S-12',
        ),
        (
            '4i5h',
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1",
            'G49-58',
        ),
    ],
)
def test_to_eng_move(move, sfen, expect):
    actual = shogi.Game(sfen).to_eng(move)
    assert expect == actual


if __name__ == '__main__':
    pytest.main([__file__])
