import pytest

import vshogi.shogi as shogi


@pytest.mark.parametrize(
    'p, expect',
    [
        ("FU", "歩"),
        ("KY", "香"),
        ("KE", "桂"),
        ("GI", "銀"),
        ("KA", "角"),
        ("HI", "飛"),
        ("KI", "金"),
        ("OU", "玉"),
        ("TO", "と"),
        ("NY", "杏"),
        ("NK", "圭"),
        ("NG", "全"),
        ("UM", "馬"),
        ("RY", "龍"),
        ("NA", "？"),
        ("B_FU", "歩"),
        ("B_KY", "香"),
        ("B_KE", "桂"),
        ("B_GI", "銀"),
        ("B_KA", "角"),
        ("B_HI", "飛"),
        ("B_KI", "金"),
        ("B_OU", "玉"),
        ("B_TO", "と"),
        ("B_NY", "杏"),
        ("B_NK", "圭"),
        ("B_NG", "全"),
        ("B_UM", "馬"),
        ("B_RY", "龍"),
        ("W_FU", "歩"),
        ("W_KY", "香"),
        ("W_KE", "桂"),
        ("W_GI", "銀"),
        ("W_KA", "角"),
        ("W_HI", "飛"),
        ("W_KI", "金"),
        ("W_OU", "玉"),
        ("W_TO", "と"),
        ("W_NY", "杏"),
        ("W_NK", "圭"),
        ("W_NG", "全"),
        ("W_UM", "馬"),
        ("W_RY", "龍"),
        ("VOID", "？"),
    ],
)
def test_to_jpn_piece(p, expect):
    p = getattr(shogi.Piece if len(p) == 2 else shogi.BoardPiece, p)
    actual = p.to_jpn()
    assert expect == actual


@pytest.mark.parametrize(
    'sq, expect',
    [
        ("SQ_1A", "１一"),
        ("SQ_1B", "１二"),
        ("SQ_1C", "１三"),
        ("SQ_1D", "１四"),
        ("SQ_1E", "１五"),
        ("SQ_1F", "１六"),
        ("SQ_1G", "１七"),
        ("SQ_1H", "１八"),
        ("SQ_1I", "１九"),
        ("SQ_2A", "２一"),
        ("SQ_2B", "２二"),
        ("SQ_2C", "２三"),
        ("SQ_2D", "２四"),
        ("SQ_2E", "２五"),
        ("SQ_2F", "２六"),
        ("SQ_2G", "２七"),
        ("SQ_2H", "２八"),
        ("SQ_2I", "２九"),
        ("SQ_3A", "３一"),
        ("SQ_3B", "３二"),
        ("SQ_3C", "３三"),
        ("SQ_3D", "３四"),
        ("SQ_3E", "３五"),
        ("SQ_3F", "３六"),
        ("SQ_3G", "３七"),
        ("SQ_3H", "３八"),
        ("SQ_3I", "３九"),
        ("SQ_4A", "４一"),
        ("SQ_4B", "４二"),
        ("SQ_4C", "４三"),
        ("SQ_4D", "４四"),
        ("SQ_4E", "４五"),
        ("SQ_4F", "４六"),
        ("SQ_4G", "４七"),
        ("SQ_4H", "４八"),
        ("SQ_4I", "４九"),
        ("SQ_5A", "５一"),
        ("SQ_5B", "５二"),
        ("SQ_5C", "５三"),
        ("SQ_5D", "５四"),
        ("SQ_5E", "５五"),
        ("SQ_5F", "５六"),
        ("SQ_5G", "５七"),
        ("SQ_5H", "５八"),
        ("SQ_5I", "５九"),
        ("SQ_6A", "６一"),
        ("SQ_6B", "６二"),
        ("SQ_6C", "６三"),
        ("SQ_6D", "６四"),
        ("SQ_6E", "６五"),
        ("SQ_6F", "６六"),
        ("SQ_6G", "６七"),
        ("SQ_6H", "６八"),
        ("SQ_6I", "６九"),
        ("SQ_7A", "７一"),
        ("SQ_7B", "７二"),
        ("SQ_7C", "７三"),
        ("SQ_7D", "７四"),
        ("SQ_7E", "７五"),
        ("SQ_7F", "７六"),
        ("SQ_7G", "７七"),
        ("SQ_7H", "７八"),
        ("SQ_7I", "７九"),
        ("SQ_8A", "８一"),
        ("SQ_8B", "８二"),
        ("SQ_8C", "８三"),
        ("SQ_8D", "８四"),
        ("SQ_8E", "８五"),
        ("SQ_8F", "８六"),
        ("SQ_8G", "８七"),
        ("SQ_8H", "８八"),
        ("SQ_8I", "８九"),
        ("SQ_9A", "９一"),
        ("SQ_9B", "９二"),
        ("SQ_9C", "９三"),
        ("SQ_9D", "９四"),
        ("SQ_9E", "９五"),
        ("SQ_9F", "９六"),
        ("SQ_9G", "９七"),
        ("SQ_9H", "９八"),
        ("SQ_9I", "９九"),
    ],
)
def test_to_jpn_square(sq, expect):
    sq = getattr(shogi.Square, sq)
    actual = sq.to_jpn()
    assert expect == actual


@pytest.mark.parametrize(
    'move, sfen, expect',
    [
        (
            '8h2b+',
            '4k4/7b1/9/9/9/9/9/1B7/4K4 b -',
            '２二角成',
        ),
        (
            '8h2b',
            '4k4/7b1/9/9/9/9/9/1B7/4K4 b -',
            '２二角不成',
        ),
        (
            'B*5e',
            '4k4/9/9/9/9/9/9/9/4K4 b B',
            '５五角',
        ),
        (
            'B*5e',
            '4k4/9/9/9/9/9/9/1B7/4K4 b B',
            '５五角打',
        ),
        (
            '1a1b',
            'k7+S/9/9/9/9/9/9/9/K8 b -',
            '１二成銀',
        ),
        (
            '4a6c',
            '1+B3+B3/9/9/9/9/9/9/9/9 b -',
            '６三馬右',
        ),
    ],
)
def test_to_jpn_move(move, sfen, expect):
    actual = shogi.Game(sfen).to_jpn(move)
    assert expect == actual


def test_to_jpn_move_dou():
    g = shogi.Game('4k3b/7+R1/9/9/9/9/9/1B7/4K4 b -').apply('1a2b')
    actual = g.to_jpn('8h2b+')
    assert '同角成' == actual


if __name__ == '__main__':
    pytest.main([__file__])
