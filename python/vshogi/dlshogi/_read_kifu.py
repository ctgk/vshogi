import numpy as np
import pandas as pd

from vshogi.judkins_shogi._game import Game as JudkinsGame  # noqa: F401
from vshogi.minishogi._game import Game as MinishogiGame  # noqa: F401
from vshogi.shogi._game import Game as StandardGame  # noqa: F401


def read_kifu(
    tsv_path: str,
    *,
    discount_factor: float = 1.0,
    importance_decay: float = 1.0,
    default_result_rate: float = 1.0,
) -> pd.DataFrame:
    """Return dataframe of Shogi kifu.

    Parameters
    ----------
    tsv_path : str
        Path to tsv file containing Shogi kifu.
    discount_factor : float, optional
        Discount factor of result value, by default 1.
    importance_decay : float, optional
        Decay factor of data importance of each game position, by default 1.
    default_result_rate : float, optional
        `value = result_rate * result + (1 - result_rate) * q_value`

    Returns
    -------
    pd.DataFrame
        Dataframe of kifu of a Shogi game.
    """
    df = pd.read_csv(
        tsv_path,
        sep='\t',
        dtype={
            'sfen': str,
            'result': int,
            'q_value': float,
            'policy': str,
        },
    )
    total_ply = len(df)
    df['policy'] = _compute_visit_dist(df)
    df['z_weight'] = _compute_z_weight(df, default_result_rate)
    df['weight'] = _compute_weight(df, importance_decay)
    df['value'] = (
        df['z_weight']
        * df['result']
        * np.power(discount_factor, total_ply - df.index - 1)
        + (1 - df['z_weight']) * df['q_value']
    )
    df['value01'] = df['value'].apply(lambda v: np.clip((v + 1) / 2, 0.0, 1.0))
    return df


def _compute_z_weight(
    df: pd.DataFrame,
    default_result_rate: float,
) -> pd.Series:
    game_class = _infer_game_variant(df['sfen'][0])
    move_class = game_class._get_move_class()
    return df.apply(
        lambda row: (
            0.0
            if (
                row['policy'] == {}
                or move_class(row['move'])
                != max(row['policy'], key=row['policy'].get)
            )
            else default_result_rate
        ),
        axis=1,
    )


def _compute_weight(df: pd.DataFrame, importance_decay: float) -> np.ndarray:
    dq = (df['q_value'].values[:-2] - df['q_value'].values[2:]).tolist() + [
        0.0,
        0.0,
    ]
    large_dq = [np.abs(d) > 0.5 for d in dq]
    return np.maximum(
        np.power(
            importance_decay,
            np.maximum(np.cumsum(large_dq[::-1]) - 2, 0),
        )[::-1],
        0.1,
    )


def _compute_visit_dist(df: pd.DataFrame):
    if len(df) == 0:
        return
    game_class = _infer_game_variant(df['sfen'][0])
    move_class = game_class._get_move_class()
    return df.apply(
        lambda row: (
            {
                move_class(m): v / (sum(eval(row['policy']).values()))
                for m, v in eval(row['policy']).items()
            }
            if '{' in row['policy']
            else {
                m: float(m.to_sfen() == row['policy'])
                for m in game_class(row['sfen']).get_legal_moves()
            }
        ),
        axis=1,
    )


def _infer_game_variant(sfen: str) -> type:
    num_slashes = sfen.split(' ')[0].count('/')
    if num_slashes == 4:
        return MinishogiGame
    elif num_slashes == 5:
        return JudkinsGame
    elif num_slashes == 8:
        return StandardGame
    else:
        raise ValueError(f'Invalid SFEN: {sfen}')
