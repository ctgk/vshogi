import io
import os

import numpy as np
import pandas as pd
import pytest

from vshogi.dlshogi import read_kifu
from vshogi.dlshogi._read_kifu import _compute_lambda_returns
from vshogi.minishogi import Move


@pytest.mark.parametrize(
    ("q_values, lambda_, expect"),
    [
        (
            (0.6, -0.8, 1.0),
            0.5,
            (
                (0.6 + 0.8 * 0.5 + 1.0 * 0.25) / (1 + 0.5 + 0.25),
                (-0.8 + -1.0 * 0.5) / (1.0 + 0.5),
                1.0,
            ),
        ),
        (
            (0.6, -0.8, 1.0),
            0.0,
            (0.6, -0.8, 1.0),
        ),
        (
            (0.6, -0.8, 1.0),
            1.0,
            (1.0, -1.0, 1.0),
        ),
    ],
)
def test_compute_lambda_returns_follow_any(q_values, lambda_, expect):
    df = pd.DataFrame(
        [{"q_value": q, "move": Move("1a1b"), "policy": {}} for q in q_values]
    )
    actual = _compute_lambda_returns(df, lambda_=lambda_)
    assert np.allclose(actual, expect)


def test_compute_lambda_returns_follow_best_only():
    df = pd.DataFrame(
        [
            {
                "q_value": 0.1,
                "move": Move("1e1d"),
                "policy": {Move("1e1d"): 1.0},
            },
            {
                "q_value": -0.2,
                "move": Move("5a5b"),
                "policy": {Move("5a5c"): 1.0},
            },
            {
                "q_value": 1.0,
                "move": Move("1d1c"),
                "policy": {},
            },
        ]
    )
    actual = _compute_lambda_returns(
        df,
        lambda_=0.5,
        follow_any_path=False,
    )
    assert np.allclose(
        actual,
        [
            (0.1 + 0.2 * 0.5) / (1 + 0.5),
            -0.2,
            1.0,
        ],
    )
    actual = _compute_lambda_returns(
        df,
        lambda_=1.0,
        follow_any_path=False,
    )
    assert np.allclose(
        actual,
        [
            (0.1 + 0.2) / 2,
            -0.2,
            1.0,
        ],
    )


def test_read_kifu():
    kifu_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        'kifu_00000.tsv',
    )
    df = read_kifu(
        kifu_path,
        result_backup_rate=1.0,
        follow_any_path=False,
    )
    print(df)
    assert np.isclose(df['z_weight'][0], 0.0)
    assert np.isclose(df['z_weight'][1], 0.0)
    assert np.isclose(df["z_weight"][30], 0.0)
    assert np.isclose(df['z_weight'][31], 1.0)
    assert np.isclose(df['z_weight'][38], 1.0)
    assert np.isclose(df['z_weight'][len(df) - 1], 0.0)

    df = read_kifu(kifu_path, result_backup_rate=1.0)
    print(df)
    assert np.isclose(df['z_weight'][0], 1.0)
    assert np.isclose(df['z_weight'][1], 1.0)
    assert np.isclose(df["z_weight"][30], 1.0)
    assert np.isclose(df['z_weight'][31], 1.0)
    assert np.isclose(df['z_weight'][38], 1.0)
    assert np.isclose(df['z_weight'][len(df) - 1], 0.0)

    assert np.isclose(df['value01'][0], 0.0)
    assert np.isclose(df['value01'][1], 1.0)
    assert np.isclose(df["value01"][30], 0.0)
    assert np.isclose(df['value01'][31], 1.0)
    assert np.isclose(df['value01'][38], 0.0)
    assert np.isclose(df['value01'][len(df) - 1], 1.0)


def test_read_kifu_q():
    kifu_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        'kifu_00000.tsv',
    )
    df = read_kifu(kifu_path, result_backup_rate=0.0)
    assert np.isclose(2 * df["value01"][0] - 1, 0.40515002608299255)


def test_read_kifu_lambda1():
    kifu_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        'kifu_00000.tsv',
    )
    df = read_kifu(kifu_path, result_backup_rate=0.0, lambda_=1.0)
    assert np.isclose(df["value01"][0], 0.0)
    assert np.isclose(df["value01"][1], 1.0)


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
