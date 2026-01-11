import os

import numpy as np
import pytest

from vshogi.dlshogi import read_kifu


def test_read_kifu():
    kifu_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        'kifu_00000.tsv',
    )
    df = read_kifu(kifu_path, default_result_rate=1.0)
    assert np.isclose(df['z_weight'][0], 0.0)
    assert np.isclose(df['z_weight'][1], 1.0)
    assert np.isclose(df['z_weight'][len(df) - 1], 0.0)


if __name__ == '__main__':
    pytest.main([__file__])
