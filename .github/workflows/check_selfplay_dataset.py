from ast import literal_eval

import click as cl
import numpy as np
import pandas as pd


@cl.command()
@cl.argument('kifu', type=str)
def _check(kifu: str):
    df = pd.read_csv(
        kifu,
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
        raise ValueError(f"Empty kifu, {kifu}")

    in_mate_sequence: bool = False
    for i in range(len(df)):
        row = df.iloc[i]
        policy: dict = (
            literal_eval(row.policy) if '{' in row.policy else {row.policy: 1}
        )
        sfen = row.sfen
        if policy != {}:
            if in_mate_sequence:
                raise ValueError(
                    f"'{sfen}': there should be empty policy "
                    f"once a checkmate is proved, but was {policy}"
                )
            continue

        in_mate_sequence = True
        if not np.isclose(abs(row.q_value), 1):
            raise ValueError(
                f"'{sfen}': `q_value`(={row.q_value}) should be 1 or -1 "
                "when `policy` is empty."
            )
        if not np.isclose(row.q_value, row.result):
            raise ValueError(
                f"'{sfen}': `q_value` (={row.q_value}) shoule be "
                f"{row.result}")


if __name__ == '__main__':
    _check()
