import numpy as np
import pytest

import vshogi.shogi as shogi


def test_ranks():
    assert 9 == shogi.Game.ranks
    assert 9 == shogi.Game().ranks


def test_files():
    assert 9 == shogi.Game.files
    assert 9 == shogi.Game().files


def test_feature_channels():
    assert 2 * (7 + 14) == shogi.Game.feature_channels
    assert 2 * (7 + 14) == shogi.Game().feature_channels


def test_num_dlshogi_policy():
    assert 9 * 9 * (2 * 10 + 7) == shogi.Game.num_dlshogi_policy
    assert 9 * 9 * (2 * 10 + 7) == shogi.Game().num_dlshogi_policy


@pytest.mark.parametrize('game, expected', [
    (
        # Turn: BLACK
        # White: FUx4,KYx2
        #     9   8   7   6   5   4   3   2   1
        #   +---+---+---+---+---+---+---+---+---+
        # A |   |-KE|-OU|-KI|   |   |   |+RY|   |
        #   +---+---+---+---+---+---+---+---+---+
        # B |   |   |-GI|   |-KI|   |   |   |-KY|
        #   +---+---+---+---+---+---+---+---+---+
        # C |+FU|-FU|-FU|-FU|-FU|   |   |   |   |
        #   +---+---+---+---+---+---+---+---+---+
        # D |+KE|-KE|   |   |-GI|   |-FU|   |-FU|
        #   +---+---+---+---+---+---+---+---+---+
        # E |   |   |   |   |   |   |   |   |   |
        #   +---+---+---+---+---+---+---+---+---+
        # F |   |   |+FU|+GI|   |   |   |+FU|+FU|
        #   +---+---+---+---+---+---+---+---+---+
        # G |   |+FU|   |+FU|+FU|   |   |   |   |
        #   +---+---+---+---+---+---+---+---+---+
        # H |   |   |+OU|+GI|   |+KY|-RY|   |   |
        #   +---+---+---+---+---+---+---+---+---+
        # I |   |+KE|+KI|   |+KI|   |   |   |-UM|
        #   +---+---+---+---+---+---+---+---+---+
        # Black: FU,KA
        shogi.Game(
            '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/'
            '1NG1G3+b b BP2l4p'),
        (
            {
                shogi.FU: 1,
                shogi.KY: 0,
                shogi.KE: 0,
                shogi.GI: 0,
                shogi.KI: 0,
                shogi.KA: 1,
                shogi.HI: 0,
            },
            {
                shogi.FU: 4,
                shogi.KY: 2,
                shogi.KE: 0,
                shogi.GI: 0,
                shogi.KI: 0,
                shogi.KA: 0,
                shogi.HI: 0,
            },
        ),
    ),
])
def test_stand(game, expected):
    assert game.stand(shogi.BLACK) == expected[0]
    assert game.stand(shogi.WHITE) == expected[1]


@pytest.mark.parametrize('game, index, expected', [
    (
        shogi.Game(
            '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/'
            '1NG1G3+b b BP2l4p',
        ).apply(shogi.SQ_9B, shogi.SQ_9C, True).apply(shogi.D7, shogi.C7),
        0,
        '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/1NG1G3+b b '
        'BP2l4p',
    ),
    (
        shogi.Game(
            '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/'
            '1NG1G3+b b BP2l4p',
        ).apply(shogi.SQ_9B, shogi.SQ_9C, True).apply(shogi.D7, shogi.C7),
        1,
        '1nkg3+R1/+P1s1g3l/1pppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/1NG1G3+b w'
        ' BP2l4p',
    ),
    (
        shogi.Game(
            '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/'
            '1NG1G3+b b BP2l4p',
        ).apply(shogi.SQ_9B, shogi.SQ_9C, True).apply(shogi.D7, shogi.C7),
        -1,
        '1nkg3+R1/+P1s1g3l/1pppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/1NG1G3+b w'
        ' BP2l4p',
    ),
])
def test_get_sfen_at(game: shogi.Game, index, expected):
    assert game.get_sfen_at(index, False) == expected


def test_array_black():
    # Turn: BLACK
    # White: FUx4,KYx2
    #     9   8   7   6   5   4   3   2   1
    #   +---+---+---+---+---+---+---+---+---+
    # A |   |-KE|-OU|-KI|   |   |   |+RY|   |
    #   +---+---+---+---+---+---+---+---+---+
    # B |   |   |-GI|   |-KI|   |   |   |-KY|
    #   +---+---+---+---+---+---+---+---+---+
    # C |+FU|-FU|-FU|-FU|-FU|   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # D |+KE|-KE|   |   |-GI|   |-FU|   |-FU|
    #   +---+---+---+---+---+---+---+---+---+
    # E |   |   |   |   |   |   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # F |   |   |+FU|+GI|   |   |   |+FU|+FU|
    #   +---+---+---+---+---+---+---+---+---+
    # G |   |+FU|   |+FU|+FU|   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # H |   |   |+OU|+GI|   |+KY|-RY|   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # I |   |+KE|+KI|   |+KI|   |   |   |-UM|
    #   +---+---+---+---+---+---+---+---+---+
    # Black: FU,KA
    game = shogi.Game(
        '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/1NG1G3+b '
        'b BP2l4p')
    actual = np.asarray(game)
    assert actual.dtype == np.float32
    assert actual.shape == (1, 9, 9, 2 * (7 + 14))
    assert np.allclose(actual[0, ..., 0], 1)  # black's captured pawn
    assert np.allclose(actual[0, ..., 1], 0)  # black's captured lance
    assert np.allclose(actual[0, ..., 2], 0)  # black's captured knight
    assert np.allclose(actual[0, ..., 3], 0)  # black's captured silver
    assert np.allclose(actual[0, ..., 4], 1)  # black's captured bishop
    assert np.allclose(actual[0, ..., 5], 0)  # black's captured rook
    assert np.allclose(actual[0, ..., 6], 0)  # black's captured gold
    assert np.allclose(actual[0, ..., 7], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 1, 0, 0, 0, 0, 1, 1],
        [0, 1, 0, 1, 1, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # black's board pawn
    assert np.allclose(actual[0, ..., 8], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 1, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # black's board lance
    assert np.allclose(actual[0, ..., 9], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 1, 0, 0, 0, 0, 0, 0, 0],
    ])  # black's board knight
    assert np.allclose(actual[0, ..., 10], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 1, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 1, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # black's board silver
    assert np.allclose(actual[0, ..., 11], 0)  # black's board bishop
    assert np.allclose(actual[0, ..., 12], 0)  # black's board rook
    assert np.allclose(actual[0, ..., 13], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 1, 0, 1, 0, 0, 0, 0],
    ])  # black's board gold
    assert np.allclose(actual[0, ..., 14], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 1, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # black's board king
    assert np.allclose(actual[0, ..., 15], 0)  # black's promoted pawn
    assert np.allclose(actual[0, ..., 14 + 21], [
        [0, 0, 1, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # white's king


def test_array_white():
    # Turn: WHITE
    # White: FUx4,KY,KE,KA,HI,KI
    #     9   8   7   6   5   4   3   2   1
    #   +---+---+---+---+---+---+---+---+---+
    # A |-KY|   |   |   |   |   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # B |   |-OU|   |   |-KI|   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # C |   |-FU|-KE|   |-FU|   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # D |   |   |+KE|-FU|-GI|   |-FU|   |-FU|
    #   +---+---+---+---+---+---+---+---+---+
    # E |   |   |   |   |   |   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # F |   |   |+GI|   |   |   |   |+FU|+FU|
    #   +---+---+---+---+---+---+---+---+---+
    # G |+FU|+FU|   |+FU|+FU|   |   |-UM|   |
    #   +---+---+---+---+---+---+---+---+---+
    # H |   |   |+OU|   |-TO|   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # I |   |+KE|+KI|   |   |   |   |   |   |
    #   +---+---+---+---+---+---+---+---+---+
    # Black: FUx2,KYx2,GIx2,HI,KI
    game = shogi.Game(
        'l8/1k2g4/1pn1p4/2Nps1p1p/9/2S4PP/PP1PP2+b1/2K1+p4/1NG6 w '
        'RG2S2L2Prbgnl4p')
    actual = np.asarray(game)
    assert np.allclose(actual[0, ..., 0], 4)  # White's captured pawn
    assert np.allclose(actual[0, ..., 1], 1)  # White's captured lance
    assert np.allclose(actual[0, ..., 2], 1)  # White's captured knight
    assert np.allclose(actual[0, ..., 3], 0)  # White's captured silver
    assert np.allclose(actual[0, ..., 4], 1)  # White's captured bishop
    assert np.allclose(actual[0, ..., 5], 1)  # White's captured rook
    assert np.allclose(actual[0, ..., 6], 1)  # White's captured gold
    assert np.allclose(actual[0, ..., 7], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, 0, 1, 0, 0, 1, 0, 0, 0],
        [0, 0, 0, 0, 1, 0, 0, 1, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # White's board pawn
    assert np.allclose(actual[0, ..., 8], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 1],
    ])  # White's board lance
    assert np.allclose(actual[0, ..., 9], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 1, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # White's board knight
    assert np.allclose(actual[0, ..., 10], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 1, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # White's board silver
    assert np.allclose(actual[0, ..., 11], 0)  # White's board bishop
    assert np.allclose(actual[0, ..., 12], 0)  # White's board rook
    assert np.allclose(actual[0, ..., 13], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 1, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # White's board gold
    assert np.allclose(actual[0, ..., 14], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 1, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # White's board king
    assert np.allclose(actual[0, ..., 15], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 1, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # White's board promoted pawn
    assert np.allclose(actual[0, ..., 0 + 21], 2)  # Black's captured pawn
    assert np.allclose(actual[0, ..., 1 + 21], 2)  # Black's captured lance
    assert np.allclose(actual[0, ..., 2 + 21], 0)  # Black's captured knight
    assert np.allclose(actual[0, ..., 3 + 21], 2)  # Black's captured silver
    assert np.allclose(actual[0, ..., 14 + 21], [
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 1, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
    ])  # Black's board king


@pytest.mark.parametrize('game, move, expected', [
    (shogi.Game(), shogi.Move(shogi.F2, shogi.G2), np.eye(2187)[1410]),
    (
        shogi.Game('9/9/1k7/9/9/9/9/9/8K w -'),
        shogi.Move(shogi.D8, shogi.C8),
        np.eye(2187)[1410],
    ),
])
def test_to_dlshogi_policy(game, move, expected):
    actual = game.to_dlshogi_policy(move)
    assert np.allclose(actual, expected, rtol=0, atol=1e-2)


if __name__ == '__main__':
    pytest.main([__file__])
