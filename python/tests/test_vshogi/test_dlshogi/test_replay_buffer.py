import numpy as np
import pytest

from vshogi.dlshogi import ReplayBuffer, Data
from vshogi.minishogi import Game, Move


def test_add():
    legal_moves = Game("rbsgk/4p/5/P4/KGSBR b -").get_legal_moves()
    buffer = ReplayBuffer(2)
    buffer.add(
        Data(
            sfen="rbsgk/4p/5/P4/KGSBR b -",
            policy={m: float(m == Move("2e3d")) for m in legal_moves},
            value01=0.0,
            weight=1.0,
        )
    )
    buffer.add(
        Data(
            sfen="rbsgk/4p/5/P4/KGSBR b -",
            policy={m: float(m == Move("4e4d")) for m in legal_moves},
            value01=0.5,
            weight=1.0,
        )
    )
    assert len(buffer) == 4
    p, v = buffer.__getitem__(0)[1:3]
    assert np.isclose(p, 1.0).sum() == 1
    assert np.isclose(p, 0.0).sum() == 13
    assert np.isclose(p, -1.0e05).sum() == p.size - (1 + 13)
    assert np.allclose(v, 0.0)

    buffer.add(
        Data(
            sfen="4k/4p/5/P4/K4 b -",
            policy={
                m: float(m == Move("5e4d"))
                for m in Game("4k/4p/5/P4/K4 b -").get_legal_moves()
            },
            value01=1.0,
            weight=1.0,
        )
    )
    assert len(buffer) == 4
    assert np.isclose(buffer[0][2], 0.5)
    assert np.isclose(buffer[1][2], 1.0)


def test_action_values():
    buffer = ReplayBuffer()
    buffer.add(
        Data(
            sfen="5/5/5/4g/4K b -",
            policy={Move("1e1d"): 1.0},
            value01={Move("1e1d"): 1.0},
        )
    )
    v = buffer.__getitem__(0)[2]
    assert np.sum(~np.isnan(v)) == 1


if __name__ == "__main__":
    pytest.main([__file__])
