import numpy as np
import pytest

import vshogi.shogi as shogi


def test_to_dlshogi_features_black():
    sfen = (
        '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/1NG1G3+b '
        'b BP2l4p'
    )
    game = shogi.Game(sfen)
    state = shogi.State(sfen)
    assert np.allclose(game.to_dlshogi_features(), state.to_dlshogi_features())


def test_to_dlshogi_features_white():
    sfen = (
        '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/1NG1G3+b '
        'w BP2l4p'
    )
    game = shogi.Game(sfen)
    state = shogi.State(sfen)
    assert np.allclose(game.to_dlshogi_features(), state.to_dlshogi_features())

    a = np.empty(
        (2, shogi.Game.ranks, shogi.Game.files, shogi.Game.feature_channels),
        dtype=np.float32,
    )
    state.to_dlshogi_features(a[1])
    assert np.allclose(game.to_dlshogi_features(), a[1])


def test_hflip():
    sfen = (
        '1nkg3+R1/2s1g3l/Ppppp4/Nn2s1p1p/9/2PS3PP/1P1PP4/2KS1L+r2/1NG1G3+b '
        'b BP2l4p'
    )
    game = shogi.Game(sfen).hflip()
    state = shogi.State(sfen).hflip()
    assert np.allclose(game.to_dlshogi_features(), state.to_dlshogi_features())


def test_to_dlshogi_policy():
    sfen = (
        'lns2g3/4pk3/p2B2npl/1r3pp1p/1ppNP4/Pb1G1P1P1/2SP2P1P/4GS1R1/L1SK3NL '
        'w G3p'
    )
    visit_probas = {
        shogi.Move('8f7g+'): 0.56355421509411,
        shogi.Move('8f7g'): 0.21471347279913788,
        shogi.Move('7a6b'): 0.015104486499432926,
        shogi.Move('8d6d'): 0.012723612914922775,
        shogi.Move('8d8c'): 0.012961183567051294,
        shogi.Move('8a7c'): 0.009437045494320097,
        shogi.Move('5b5c'): 0.010101306150379302,
        shogi.Move('8f9g+'): 0.010049348685506346,
        shogi.Move('4b5a'): 0.00766370202479017,
        shogi.Move('4a3b'): 0.009516890659961882,
        shogi.Move('4b4c'): 0.005723274679662467,
        shogi.Move('4b3b'): 0.007723155840429607,
        shogi.Move('P*6b'): 0.006200883483739795,
        shogi.Move('4a5a'): 0.0074180626674919745,
        shogi.Move('P*6h'): 0.008088914014014962,
        shogi.Move('4b3a'): 0.004497030326486887,
        shogi.Move('P*6d'): 0.004438120914013447,
        shogi.Move('4a3a'): 0.004846942651824834,
        shogi.Move('8f9g'): 0.006656473725232425,
        shogi.Move('7a7b'): 0.003747322871671379,
        shogi.Move('7e7f'): 0.006486915647181811,
        shogi.Move('2c2d'): 0.004228492168517591,
        shogi.Move('8d9d'): 0.004055438138352515,
        shogi.Move('8d8b'): 0.006089579071047845,
        shogi.Move('8d7d'): 0.0052485314343969245,
        shogi.Move('4d4e'): 0.002313676007927299,
        shogi.Move('3d3e'): 0.0057836962529822615,
        shogi.Move('3c4e'): 0.00432572762225032,
        shogi.Move('3c2e'): 0.005676076807211283,
        shogi.Move('7a8b'): 0.004176951495224835,
        shogi.Move('9c9d'): 0.0034601474821509734,
        shogi.Move('8f9e'): 0.0033544245467463448,
        shogi.Move('P*6a'): 0.004052214841118307,
        shogi.Move('1d1e'): 0.004743156107785823,
        shogi.Move('9a9b'): 0.0047365409351798615,
        shogi.Move('8d5d'): 0.006102962489321444,
    }
    game = shogi.Game(sfen)
    state = shogi.State(sfen)
    assert np.allclose(
        game.to_dlshogi_policy(visit_probas, default_value=-1.),
        state.to_dlshogi_policy(visit_probas, default_value=-1.),
    )

    assert np.sum(np.isnan(state.to_dlshogi_policy(
        visit_probas, default_value=np.nan))) > 0

    a = np.empty((2, shogi.Game.num_dlshogi_policy), dtype=np.float32)
    state.to_dlshogi_policy(visit_probas, default_value=-1., out=a[1])
    assert np.allclose(
        game.to_dlshogi_policy(visit_probas, default_value=-1.), a[1])


if __name__ == '__main__':
    pytest.main([__file__])
