import pytest

import vshogi.shogi as shogi


@pytest.mark.parametrize(
    'm, g, expect',
    [
        (
            shogi.Move('8h2b+'),
            shogi.Game('4k4/7b1/9/9/9/9/9/1B7/4K4 b -'),
            'Bx22+',
        ),
        (
            shogi.Move('8h2b'),
            shogi.Game('4k4/7b1/9/9/9/9/9/1B7/4K4 b -'),
            'Bx22=',
        ),
        (
            shogi.Move('8h2b+'),
            shogi.Game('4k3b/7+R1/9/9/9/9/9/1B7/4K4 b -').apply('1a2b'),
            'Bx22+',
        ),
        (
            shogi.Move('B*5e'),
            shogi.Game('4k4/9/9/9/9/9/9/9/4K4 b B'),
            'B*55',
        ),
        (
            shogi.Move('B*5e'),
            shogi.Game('4k4/9/9/9/9/9/9/1B7/4K4 b B'),
            'B*55',
        ),
        (
            shogi.Move('1a1b'),
            shogi.Game('k7+S/9/9/9/9/9/9/9/K8 b -'),
            '+S-12',
        ),
        (
            shogi.Move('4i5h'),
            shogi.Game(),
            'G49-58',
        ),
    ],
)
def test_to_eng_move(m, g, expect):
    actual = g.to_eng(m)
    assert expect == actual


if __name__ == '__main__':
    pytest.main([__file__])
