import io
import os

import numpy as np
import pytest

from vshogi.dlshogi import read_kifu
from vshogi.minishogi import Move


def test_read_kifu():
    kifu_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        'kifu_00000.tsv',
    )
    df = read_kifu(kifu_path, default_result_rate=1.0)
    print(df)
    assert np.isclose(df['z_weight'][0], 0.0)
    assert np.isclose(df['z_weight'][1], 0.0)
    assert np.isclose(df["z_weight"][30], 0.0)
    assert np.isclose(df['z_weight'][31], 1.0)
    assert np.isclose(df['z_weight'][38], 1.0)
    assert np.isclose(df['z_weight'][len(df) - 1], 0.0)

    df = read_kifu(
        kifu_path,
        default_result_rate=1.0,
        always_backup_result=True,
    )
    print(df)
    assert np.isclose(df['z_weight'][0], 1.0)
    assert np.isclose(df['z_weight'][1], 1.0)
    assert np.isclose(df["z_weight"][30], 1.0)
    assert np.isclose(df['z_weight'][31], 1.0)
    assert np.isclose(df['z_weight'][38], 1.0)
    assert np.isclose(df['z_weight'][len(df) - 1], 0.0)


def test_read_kifu_single_policy():
    content = (
        "sfen\tmove\tresult\tq_value\tpolicy\n"
        "rbsgk/4p/5/P4/KGSBR b -\t4e4d\t-1\t0.5\t{'2e3d': 1.0}\n"
        "rbsgk/4p/5/P4/KGSBR b -\t4e4d\t-1\t0.5\t{'4e4d': 1.0}\n"
    )
    with io.StringIO(initial_value=content) as f:
        f.seek(0)
        df = read_kifu(f)
    assert df['policy'][0] == {Move("2e3d"): 1.0}
    assert df['policy'][1] == {Move("4e4d"): 1.0}


if __name__ == '__main__':
    pytest.main([__file__])
