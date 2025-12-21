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
        usecols=['sfen', 'result', 'q_value', 'policy', 'z_weight'],
        dtype={
            'sfen': str,
            'result': int,
            'q_value': float,
            'policy': str,
            'z_weight': float,
        },
    )
    if len(df) == 0:
        raise ValueError(f"Empty kifu, {args.kifu}")

    in_mate_sequence: bool = False
    for i in range(len(df)):
        row = df.iloc[i]
        policy: dict = eval(row.policy)
        sfen = row.sfen
        if policy != {}:
            if in_mate_sequence:
                raise ValueError(
                    f"'{sfen}': there should be empty policy "
                    f"once a checkmate is proved, but was {policy}"
                )
            continue

        in_mate_sequence = True
        turn: tp.Union[tp.Literal['b'], tp.Literal['w']] = sfen.split(' ')[1]
        if not np.isclose(abs(row.q_value), 1):
            raise ValueError(
                f"'{sfen}': `q_value`(={row.q_value}) should be 1 or -1 "
                "when `policy` is empty."
            )
        if not np.isclose(row.q_value, row.result):
            raise ValueError(
                f"'{sfen}': `q_value` (={row.q_value}) shoule be "
                f"{row.result}")
