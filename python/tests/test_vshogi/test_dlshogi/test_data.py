import numpy as np
import pytest

from vshogi.dlshogi import Data
from vshogi.minishogi import Move


def test_merge_moving_average():
    d1 = Data(
        sfen="5/5/5/5/5 b -",
        policy={Move("1e1d"): 1.0},
        value01=0.0,
        count=100,
    )
    d = Data(sfen="5/5/5/5/5 b -", policy={Move("1e2e"): 1.0}, value01=1.0)
    actual = d.merge(d1)
    assert actual.sfen == "5/5/5/5/5 b -"
    assert np.isclose(0.01, actual.value01)
    assert set(actual.policy.keys()) == set([Move("1e1d"), Move("1e2e")])
    assert np.isclose(0.99, actual.policy[Move("1e1d")])
    assert np.isclose(0.01, actual.policy[Move("1e2e")])


if __name__ == "__main__":
    pytest.main([__file__])
