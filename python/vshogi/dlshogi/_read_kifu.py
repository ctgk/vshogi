import numpy as np
import pandas as pd

from vshogi.judkins_shogi._game import Game as JudkinsGame  # noqa: F401
from vshogi.minishogi._game import Game as MinishogiGame  # noqa: F401
from vshogi.shogi._game import Game as StandardGame  # noqa: F401


def read_kifu(
    tsv_path: str,
    *,
    lambda_: float = 0.0,
    discount_factor: float = 1.0,
    importance_decay: float = 1.0,
    result_backup_rate: float = 1.0,
    tail_fraction: float | None = None,
    follow_any_path: bool = True,
) -> pd.DataFrame:
    """Return dataframe of Shogi kifu.

    Parameters
    ----------
    tsv_path : str
        Path to tsv file containing Shogi kifu.
    lambda_ : float, optional
        Hyperparameter used to blend all possible n-step returns,
        by default 0.0 (pure bootstrapping).
    discount_factor : float, optional
        Discount factor of result value, by default 1.
    importance_decay : float, optional
        Decay factor of data importance of each game position, by default 1.
    result_backup_rate : float, optional
        `value = result_rate * result + (1 - result_rate) * q_value`
    tail_fraction : float, optional
        Return fraction of the dataframe from tail if given, by default None.
    follow_any_path : bool, optional
        If True, compute targets along the recorded game path regardless of
        whether each move matches the policy argmax. If False, only continue
        backup while moves follow the policy argmax path, by default True.

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
    df["malignancy"] = 1.0 - (df.index + 1) / total_ply
    df["malignancy"] = df["malignancy"].apply(lambda x: max(x, 0.01))
    game_class = _infer_game_variant(df['sfen'][0])
    move_class = game_class._get_move_class()
    df['weight'] = _compute_weight(df, importance_decay)
    if tail_fraction is not None:
        df = df.tail(int(total_ply * tail_fraction))
        df.reset_index(drop=True, inplace=True)
    if len(df) == 0:
        return df
    df["move"] = df["move"].apply(lambda s: move_class(s))
    df['policy'] = _preprocess_policy(df, move_class)
    df['z_weight'] = _compute_z_weight(
        df,
        result_backup_rate,
        always_backup_result=follow_any_path,
    )
    df['value'] = df['z_weight'] * df['result'] * np.power(
        discount_factor, total_ply - df.index - 1
    ) + (1 - df['z_weight']) * _compute_lambda_returns(
        df,
        lambda_,
        discount_factor=discount_factor,
        follow_any_path=follow_any_path,
    )
    df['value01'] = df['value'].apply(lambda v: np.clip((v + 1) / 2, 0.0, 1.0))
    return df


def _compute_lambda_returns(
    df: pd.DataFrame,
    lambda_: float,
    *,
    discount_factor: float = 1.0,
    follow_any_path: bool = True,
) -> np.ndarray:
    q_values = df["q_value"].values
    if np.isclose(lambda_, 0.0):
        return q_values

    is_best: list[bool] = list(
        (
            df["move"]
            == df.apply(
                lambda row: (
                    max(row["policy"], key=row["policy"].get)
                    if row["policy"]
                    else row["move"]
                ),
                axis=1,
            )
        ).values
    )
    lambda_returns: list[float] = []
    for i in range(len(q_values)):
        n = len(q_values) - i
        discount = np.power(discount_factor, np.arange(n))
        if lambda_ >= 1.0:
            if (not follow_any_path) and (not all(is_best[i:-1])):
                weights = np.power(lambda_, np.arange(n))
                weights *= np.cumprod([True] + is_best[i:-1])
                lambda_returns.append(
                    np.dot(
                        weights,
                        q_values[i:] * discount * ((-1) ** np.arange(n)),
                    )
                    / weights.sum()
                )
            else:
                lambda_returns.append(
                    q_values[-1]
                    * discount[-1]
                    * (2 * ((len(q_values) - i) % 2) - 1)
                )
        else:
            weights = np.power(lambda_, np.arange(n))
            if not follow_any_path:
                weights *= np.cumprod([True] + is_best[i:-1])
            lambda_returns.append(
                np.dot(
                    weights, q_values[i:] * discount * ((-1) ** np.arange(n))
                )
                / weights.sum()
            )
    return np.asarray(lambda_returns)


def _compute_z_weight(
    df: pd.DataFrame,
    result_backup_rate: float,
    *,
    always_backup_result: bool = True,
) -> list[float]:
    policy: list[dict] = df["policy"].to_list()
    if not always_backup_result:
        is_best: list[bool] = df.apply(
            lambda row: row["policy"] == {}
            or (row["move"] == max(row["policy"], key=row["policy"].get)),
            axis=1,
        ).values.tolist()
        is_all_best_later_turns: list[bool] = [
            all(is_best[i:]) for i in range(len(is_best))
        ]
    else:
        is_all_best_later_turns = [True for _ in policy]
    assert len(policy) == len(is_all_best_later_turns)
    return [
        0.0
        if policy[i] == {}
        else result_backup_rate
        if is_all_best_later_turns[i]
        else 0.0
        for i in range(len(policy))
    ]


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


def _preprocess_policy(df: pd.DataFrame, move_class: type):
    if len(df) == 0:
        return
    return df["policy"].apply(
        lambda d: {move_class(m): v for m, v in eval(d).items()}
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
