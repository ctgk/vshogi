from copy import copy

import numpy as np
import pytest

import vshogi.minishogi as shogi


def test_ranks():
    assert 5 == shogi.Game.ranks
    assert 5 == shogi.Game().ranks


def test_files():
    assert 5 == shogi.Game.files
    assert 5 == shogi.Game().files


def test_feature_channels():
    assert 2 * (10 + 5) == shogi.Game.feature_channels
    assert 2 * (10 + 5) == shogi.Game().feature_channels


def test_num_dlshogi_policy():
    assert 5 * 5 * (2 * 8 + 5) == shogi.Game.num_dlshogi_policy
    assert 5 * 5 * (2 * 8 + 5) == shogi.Game().num_dlshogi_policy


def test_init():
    shogi.Game()


@pytest.mark.parametrize('sfen, expect_turn, expect_result', [
    ('4k/5/5/5/K4 b -', shogi.BLACK, shogi.ONGOING),
    ('4k/5/5/5/K3R w -', shogi.WHITE, shogi.ONGOING),
    ('4k/5/4p/4g/4K b -', shogi.BLACK, shogi.WHITE_WIN),
])
def test_init_sfen(sfen, expect_turn, expect_result):
    g = shogi.Game(sfen)
    assert g.turn == expect_turn
    assert g.result == expect_result


def test_board():
    game: shogi.Game = shogi.Game()
    actual = np.asarray(game.board)

    expected = np.rot90([
        [shogi.W_HI, shogi.W_KA, shogi.W_GI, shogi.W_KI, shogi.W_OU],
        [shogi.VOID, shogi.VOID, shogi.VOID, shogi.VOID, shogi.W_FU],
        [shogi.VOID, shogi.VOID, shogi.VOID, shogi.VOID, shogi.VOID],
        [shogi.B_FU, shogi.VOID, shogi.VOID, shogi.VOID, shogi.VOID],
        [shogi.B_OU, shogi.B_KI, shogi.B_GI, shogi.B_KA, shogi.B_HI],
    ])
    assert (actual == expected).all()


def test_to_sfen():
    game = shogi.Game().apply(shogi.Move(shogi.SQ_1B, shogi.SQ_1E))

    actual = shogi.Game(game.to_sfen())
    assert shogi.B_HI == actual.board[shogi.SQ_1B]
    assert shogi.VOID == actual.board[shogi.SQ_1E]
    assert actual.stand(shogi.BLACK) == {
        shogi.FU: 1, shogi.GI: 0, shogi.KI: 0, shogi.KA: 0, shogi.HI: 0,
    }
    assert actual.stand(shogi.WHITE) == {
        shogi.FU: 0, shogi.GI: 0, shogi.KI: 0, shogi.KA: 0, shogi.HI: 0,
    }


def test_shallow_copy():
    g1 = shogi.Game()
    g2 = copy(g1)
    g1.apply(shogi.Move(shogi.SQ_5C, shogi.SQ_5D))
    assert g1.board[shogi.SQ_5C] == g2.board[shogi.SQ_5C]


def test_copy():
    g1 = shogi.Game()
    g2 = g1.copy()
    g1.apply(shogi.Move(shogi.SQ_5C, shogi.SQ_5D))
    assert g1.board[shogi.SQ_5C] != g2.board[shogi.SQ_5C]


def test_play():
    game = shogi.Game()
    moves = ["4e3d", "2a3b", "3d2c", "3b4c", "2c1b"]
    for m in moves:
        assert shogi.ONGOING == game.result
        game.apply(m)
    assert shogi.BLACK_WIN == game.result


def test_get_legal_moves():
    game = shogi.Game()
    actual = game.get_legal_moves()
    assert len(actual) == 14


def test_get_check_moves():
    game = shogi.Game()
    actual = game.get_check_moves()
    assert len(actual) == 1
    assert actual[0] == shogi.Move('1e1b')


def test_get_attention():
    a = shogi.Game.get_attention()
    assert a.shape == (5, 5, 5, 5)
    a = a.reshape(25, 25)
    assert np.allclose(a.T, a)
    expect = np.array([
        [1, 0, 1, 1, 1],
        [1, 1, 1, 0, 0],
        [0, 1, 0, 1, 0],
        [0, 1, 0, 0, 1],
        [0, 1, 0, 0, 0],
    ])
    assert np.allclose(a[1].reshape(5, 5), expect)


def test_get_local_attentions():
    a = shogi.Game.get_local_attentions()
    assert a.shape == (8, 5, 5, 5, 5)
    expect = np.array([
        [0, 0, 0, 0, 0],
        [1, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
    ])
    assert np.allclose(a[0, 0, 1], expect)

    a = a.sum(axis=0)
    a = a.reshape(25, 25)
    assert np.allclose(a.T, a)
    expect = np.array([
        [1, 0, 1, 0, 0],
        [1, 1, 1, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
    ])
    assert np.allclose(a[1].reshape(5, 5), expect)


def test_get_adjacent_attention():
    a = shogi.Game.get_adjacent_attention()
    assert a.shape == (5, 5, 5, 5)
    a = a.reshape(25, 25)
    assert np.allclose(a.T, a)
    expect = np.array([
        [1, 0, 1, 1, 1],
        [0, 1, 0, 0, 0],
        [0, 1, 0, 0, 0],
        [0, 1, 0, 0, 0],
        [0, 1, 0, 0, 0],
    ])
    assert np.allclose(a[1].reshape(5, 5), expect)


def test_get_diagonal_attention():
    a = shogi.Game.get_diagonal_attention()
    assert a.shape == (5, 5, 5, 5)
    a = a.reshape(25, 25)
    assert np.allclose(a.T, a)
    expect = np.array([
        [0, 0, 0, 0, 0],
        [1, 0, 1, 0, 0],
        [0, 0, 0, 1, 0],
        [0, 0, 0, 0, 1],
        [0, 0, 0, 0, 0],
    ])
    assert np.allclose(a[1].reshape(5, 5), expect)


def test_array_black():
    # Turn: BLACK
    # White: KA
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |-KA|-GI|-OU|   |
    #   *---*---*---*---*---*
    # B |-HI|   |   |   |-FU|
    #   *---*---*---*---*---*
    # C |   |   |   |   |   |
    #   *---*---*---*---*---*
    # D |+FU|   |   |   |   |
    #   *---*---*---*---*---*
    # E |+OU|+KI|+GI|   |+HI|
    #   *---*---*---*---*---*
    # Black: KI
    game = shogi.Game().apply(shogi.C4, shogi.E2).apply(shogi.B5, shogi.A5)
    game.apply(shogi.A2, shogi.C4, True).apply(shogi.A2, shogi.A1)

    actual = game.to_dlshogi_features()
    assert np.allclose(actual[0, ..., 0], 0)  # white's captured pawn
    assert np.allclose(actual[0, ..., 1], 0)  # white's captured silver
    assert np.allclose(actual[0, ..., 2], 0)  # white's captured bishop
    assert np.allclose(actual[0, ..., 3], 0)  # white's captured rook
    assert np.allclose(actual[0, ..., 4], 1)  # white's captured gold


def test_array_white():
    game = shogi.Game()
    game.apply(shogi.Move(shogi.SQ_1B, shogi.SQ_1E))
    # Turn: WHITE
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |-HI|-KA|-GI|-KI|-OU|
    #   *---*---*---*---*---*
    # B |   |   |   |   |+HI|
    #   *---*---*---*---*---*
    # C |   |   |   |   |   |
    #   *---*---*---*---*---*
    # D |+FU|   |   |   |   |
    #   *---*---*---*---*---*
    # E |+OU|+KI|+GI|+KA|   |
    #   *---*---*---*---*---*
    # Black: FU

    actual = game.to_dlshogi_features()
    assert actual.dtype == np.float32
    assert actual.shape == (1, 5, 5, 30)
    assert np.allclose(actual[0, ..., 0], 0)  # white's captured pawn
    assert np.allclose(actual[0, ..., 1], 0)  # white's captured silver
    assert np.allclose(actual[0, ..., 2], 0)  # white's captured gold
    assert np.allclose(actual[0, ..., 3], 0)  # white's captured bishop
    assert np.allclose(actual[0, ..., 4], 0)  # white's captured rook
    assert np.allclose(actual[0, ..., 5], 0)  # white's board pawn
    assert np.allclose(actual[0, ..., 6], np.rot90([
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 1, 0, 0],
    ]))  # white's board silver
    assert np.allclose(actual[0, ..., 7], np.rot90([
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 1, 0],
    ]))  # white's board bishop
    assert np.allclose(actual[0, ..., 8], np.rot90([
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 1],
    ]))  # white's board rook
    assert np.allclose(actual[0, ..., 9], np.rot90([
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0],
    ]))  # white's board gold
    assert np.allclose(actual[0, ..., 15], 1)  # black's captured pawn
    assert np.allclose(actual[0, ..., 23], np.rot90([
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
        [1, 0, 0, 0, 0],
        [0, 0, 0, 0, 0],
    ]))  # black's rook


def test_stand():
    game = shogi.Game('2r2/1p1B+B/k1s2/G1R2/K1G2 w SP 2')
    # Turn: WHITE
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |   |-HI|   |   |
    #   *---*---*---*---*---*
    # B |   |-FU|   |+KA|+UM|
    #   *---*---*---*---*---*
    # C |-OU|   |-GI|   |   |
    #   *---*---*---*---*---*
    # D |+KI|   |+HI|   |   |
    #   *---*---*---*---*---*
    # E |+OU|   |+KI|   |   |
    #   *---*---*---*---*---*
    # Black: GI,FU
    assert game.stand(shogi.BLACK) == {
        shogi.HI: 0, shogi.KA: 0, shogi.KI: 0, shogi.GI: 1, shogi.FU: 1,
    }
    assert game.stand(shogi.WHITE) == {
        shogi.HI: 0, shogi.KA: 0, shogi.KI: 0, shogi.GI: 0, shogi.FU: 0,
    }


def test_to_dlshogi_policy():
    # Turn: WHITE
    # White: -
    #     5   4   3   2   1
    #   *---*---*---*---*---*
    # A |   |-KA|-GI|-KI|-OU|
    #   *---*---*---*---*---*
    # B |   |   |   |   |-FU|
    #   *---*---*---*---*---*
    # C |   |   |   |   |   |
    #   *---*---*---*---*---*
    # D |+FU|   |   |   |   |
    #   *---*---*---*---*---*
    # E |+OU|+KI|+GI|+KA|+HI|
    #   *---*---*---*---*---*
    # Black: -
    game = shogi.Game("1bsgk/4p/5/P4/KGSBR w - 1")
    a = shogi.Move(shogi.C1, shogi.B1)
    actual = game.to_dlshogi_policy({
        m: 0.5 if m == a else 0.05 for m in game.get_legal_moves()
    }, default_value=-1.)

    expected = np.zeros(5 * 5 * (2 * 8 + 5)) - 1
    expected[a.rotate()._to_dlshogi_policy_index()] = 0.5
    for m in game.get_legal_moves():
        if m == a:
            continue
        expected[m.rotate()._to_dlshogi_policy_index()] = 0.05
    assert np.allclose(expected, actual)


def test_get_sfen_at():
    actual = shogi.Game().apply(shogi.C5, shogi.D5).get_sfen_at(0)
    assert "rbsgk/4p/5/P4/KGSBR b - 1" == actual

    actual = shogi.Game().apply(shogi.C5, shogi.D5).get_sfen_at(
        0, include_move_count=False)
    assert "rbsgk/4p/5/P4/KGSBR b -" == actual

    game = shogi.Game('4k/5/5/5/K4 b RBGSPrbgsp')
    game.apply(game.get_legal_moves()[0])
    actual = game.get_sfen_at(-1)
    assert "4k/5/5/5/K4 b RBGSPrbgsp 1" == actual


def test_to_svg():
    actual = shogi.Game().to_svg()
    expect = (
        '<svg xmlns="http://www.w3.org/2000/svg" version="1.1" '
        'xmlns:xlink="http://www.w3.org/1999/xlink" width="150.0" '
        'height="192.0" viewBox="0 0 150 192">'
        '<defs>'
        '<g id="P">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#27497;</text>'
        '</g>'
        '<g id="S">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#37504;</text>'
        '</g>'
        '<g id="B">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#35282;</text>'
        '</g>'
        '<g id="R">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#39131;</text>'
        '</g>'
        '<g id="G">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#37329;</text>'
        '</g>'
        '<g id="K">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#29577;</text>'
        '</g>'
        '<g id="+P">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#12392;</text>'
        '</g>'
        '<g id="+S">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#20840;</text>'
        '</g>'
        '<g id="+B">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#39340;</text>'
        '</g>'
        '<g id="+R">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="10.5" y="16.5">&#40845;</text>'
        '</g>'
        '<g id="p" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#27497;</text>'
        '</g>'
        '<g id="s" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#37504;</text>'
        '</g>'
        '<g id="b" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#35282;</text>'
        '</g>'
        '<g id="r" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#39131;</text>'
        '</g>'
        '<g id="g" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#37329;</text>'
        '</g>'
        '<g id="k" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#29577;</text>'
        '</g>'
        '<g id="+p" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#12392;</text>'
        '</g>'
        '<g id="+s" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#20840;</text>'
        '</g>'
        '<g id="+b" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#39340;</text>'
        '</g>'
        '<g id="+r" transform="rotate(180)">'
        '<text font-family="serif" font-size="17" text-anchor="middle" '
        'x="-10.5" y="-3.5">&#40845;</text>'
        '</g>'
        '</defs>'
        '<g stroke="black">'
        '<rect x="20" y="10" width="101" height="101" fill="none" '
        'stroke-width="1.5" />'
        '<line x1="20.5" y1="30.5" x2="120.5" y2="30.5" stroke-width="1.0" />'
        '<line x1="20.5" y1="50.5" x2="120.5" y2="50.5" stroke-width="1.0" />'
        '<line x1="20.5" y1="70.5" x2="120.5" y2="70.5" stroke-width="1.0" />'
        '<line x1="20.5" y1="90.5" x2="120.5" y2="90.5" stroke-width="1.0" />'
        '<line x1="40.5" y1="10.5" x2="40.5" y2="110.5" stroke-width="1.0" />'
        '<line x1="60.5" y1="10.5" x2="60.5" y2="110.5" stroke-width="1.0" />'
        '<line x1="80.5" y1="10.5" x2="80.5" y2="110.5" stroke-width="1.0" />'
        '<line x1="100.5" y1="10.5" x2="100.5" y2="110.5" '
        'stroke-width="1.0" />'
        '</g>'
        '<g>'
        '<text font-family="serif" text-anchor="middle" '
        'font-size="9" x="30.5" y="8">5</text>'
        '<text font-family="serif" text-anchor="middle" '
        'font-size="9" x="50.5" y="8">4</text>'
        '<text font-family="serif" text-anchor="middle" '
        'font-size="9" x="70.5" y="8">3</text>'
        '<text font-family="serif" text-anchor="middle" '
        'font-size="9" x="90.5" y="8">2</text>'
        '<text font-family="serif" text-anchor="middle" '
        'font-size="9" x="110.5" y="8">1</text>'
        '<text font-family="serif" font-size="9" x="123.5" y="23">'
        '&#19968;</text>'
        '<text font-family="serif" font-size="9" x="123.5" y="43">'
        '&#20108;</text>'
        '<text font-family="serif" font-size="9" x="123.5" y="63">'
        '&#19977;</text>'
        '<text font-family="serif" font-size="9" x="123.5" y="83">'
        '&#22235;</text>'
        '<text font-family="serif" font-size="9" x="123.5" y="103">'
        '&#20116;</text>'
        '</g>'
        '<use xlink:href="#k" x="100.5" y="10.5" />'
        '<use xlink:href="#p" x="100.5" y="30.5" />'
        '<use xlink:href="#R" x="100.5" y="90.5" />'
        '<use xlink:href="#g" x="80.5" y="10.5" />'
        '<use xlink:href="#B" x="80.5" y="90.5" />'
        '<use xlink:href="#s" x="60.5" y="10.5" />'
        '<use xlink:href="#S" x="60.5" y="90.5" />'
        '<use xlink:href="#b" x="40.5" y="10.5" />'
        '<use xlink:href="#G" x="40.5" y="90.5" />'
        '<use xlink:href="#r" x="20.5" y="10.5" />'
        '<use xlink:href="#P" x="20.5" y="70.5" />'
        '<use xlink:href="#K" x="20.5" y="90.5" />'
        '<text font-family="serif" font-size="14" x="134" y="96">'
        '&#25163;</text>'
        '<text font-family="serif" font-size="14" x="134" y="82">'
        '&#20808;</text>'
        '<text font-family="serif" font-size="14" x="134" y="68">'
        '&#9751;</text>'
        '<text font-family="serif" font-size="14" x="-16" y="-24" '
        'transform="rotate(180)">&#25163;</text>'
        '<text font-family="serif" font-size="14" x="-16" y="-38" '
        'transform="rotate(180)">&#24460;</text>'
        '<text font-family="serif" font-size="14" x="-16" y="-52" '
        'transform="rotate(180)">&#9750;</text>'
        '</svg>'
    )
    assert actual == expect


if __name__ == '__main__':
    pytest.main([__file__])
