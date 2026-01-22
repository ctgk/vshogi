import numpy as np
import pytest

from vshogi.dlshogi import ReplayBuffer, Data
from vshogi.minishogi import Move


def test_deduplicate():
    buffer = ReplayBuffer()
    buffer.add(
        Data(
            sfen="rbsgk/4p/5/P4/KGSBR b -",
            policy={Move("2e3d"): 1.0},
            value01=0.0,
            weight=1.0,
        )
    )
    buffer.add(
        Data(
            sfen="rbsgk/4p/5/P4/KGSBR b -",
            policy={Move("4e4d"): 1.0},
            value01=0.5,
            weight=1.0,
        )
    )
    buffer.deduplicate()
    p, v = buffer.__getitem__(0)[1:3]
    assert np.isclose(p, 0.5).sum() == 2
    assert np.isclose(p, 0.0).sum() == 12
    assert np.isclose(p, -1.0e05).sum() == p.size - (2 + 12)
    assert np.allclose(v, 0.25)


if __name__ == "__main__":
    pytest.main([__file__])
