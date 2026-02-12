import numpy as np
import pytest

from vshogi.dlshogi import ReplayBuffer, Data
from vshogi.minishogi import Game, Move


def test_deduplicate():
    legal_moves = Game("rbsgk/4p/5/P4/KGSBR b -").get_legal_moves()
    buffer = ReplayBuffer(1, alpha=0.9)
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
    assert len(buffer) == 2
    p, v = buffer.__getitem__(0)[1:3]
    assert np.isclose(p, 0.9).sum() == 1
    assert np.isclose(p, 0.1).sum() == 1
    assert np.isclose(p, 0.0).sum() == 12
    assert np.isclose(p, -1.0e05).sum() == p.size - (2 + 12)
    assert np.allclose(v, 0.05)
    assert np.isclose(
        buffer.get_ema_of("rbsgk/4p/5/P4/KGSBR b -").value01, 0.05
    )

    buffer.add(
        Data(
            sfen="4k/4p/5/P4/K4 b -",
            policy={
                m: float(m == Move("5e4d"))
                for m in Game("4k/4p/5/P4/K4 b -").get_legal_moves()
            },
            value01=0.5,
            weight=1.0,
        )
    )
    assert len(buffer) == 2
    assert np.isclose(buffer[0][2], 0.5)
    assert buffer.get_ema_of("rbsgk/4p/5/P4/KGSBR b -") is None
    assert np.isclose(buffer.get_ema_of("4k/4p/5/P4/K4 b -").value01, 0.5)


if __name__ == "__main__":
    pytest.main([__file__])
