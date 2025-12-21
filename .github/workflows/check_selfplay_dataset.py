import typing as tp

from classopt import classopt, config
import numpy as np
import pandas as pd


@classopt(default_long=True)
class Args:
    kifu: str = config(long=False)


if __name__ == '__main__':
    args = Args.from_args()
    df = pd.read_csv(
        args.kifu,
        sep='\t',
        usecols=['state', 'result', 'q_value', 'visit_count', 'z_weight'],
        dtype={
            'state': str,
            'result': int,
            'q_value': float,
            'visit_count': str,
            'z_weight': float,
        },
    )
    if len(df) == 0:
        raise ValueError(f"Empty kifu, {args.kifu}")

    in_mate_sequence: bool = False
    for i in range(len(df)):
        row = df.iloc[i]
        visit_count: dict = eval(row.visit_count)
        state = row.state
        if visit_count != {}:
            if in_mate_sequence:
                raise ValueError(
                    f"'{state}': there should be empty visit_count "
                    f"once a checkmate is proved, but was {visit_count}"
                )
            continue

        in_mate_sequence = True
        turn: tp.Union[tp.Literal['b'], tp.Literal['w']] = state.split(' ')[1]
        if not np.isclose(abs(row.q_value), 1):
            raise ValueError(
                f"'{state}': `q_value`(={row.q_value}) should be 1 or -1 "
                "when `visit_count` is empty."
            )
        if not np.isclose(row.q_value, row.result):
            raise ValueError(
                f"'{state}': `q_value` (={row.q_value}) shoule be "
                f"{row.result}")
