import re

from vshogi._game import Game
from vshogi.judkins_shogi._game import Game as JudkinsGame  # noqa: F401
from vshogi.leshogi._game import Game as LeshogiGame  # noqa: F401
from vshogi.minishogi._game import Game as MinishogiGame  # noqa: F401
from vshogi.shogi._game import Game as StandardGame  # noqa: F401


def _infer_game_variant(sfen: str) -> type[Game]:
    num_ranks = sfen.split(' ')[0].count('/') + 1
    num_files = len(
        re.sub(
            r"\d+",
            lambda x: "1" * int(x.group()),
            sfen.split("/")[0].replace("+", ""),
        )
    )

    if (num_files, num_ranks) == (4, 6):
        return LeshogiGame
    elif (num_files, num_ranks) == (5, 5):
        return MinishogiGame
    elif (num_files, num_ranks) == (6, 6):
        return JudkinsGame
    elif (num_files, num_ranks) == (9, 9):
        return StandardGame
    else:
        raise ValueError(f'Invalid SFEN: {sfen}')
