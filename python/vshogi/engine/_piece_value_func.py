from vshogi._game import Game
from vshogi._move import Move


def piece_value_func(
    g: Game, *, return_action_values: bool = False
) -> float | dict[Move, float]:
    """Return turn player's value of the current state based on piece values.

    Parameters
    ----------
    g : Game
        The current game state to evaluate.
    return_action_values : bool
        Return action values if true.

    Returns
    -------
    float | dict[Move, float]
        Turn player's value of the current state. The value ranges between -1
        to 1.
        If `return_action_values` is true, then returns dict of action values.

    Examples
    --------
    >>> from vshogi.minishogi import *
    >>> game = Game()
    >>> piece_value_func(game)
    0.0
    >>> piece_value_func(game.apply("1e1b"))  # doctest: +ELLIPSIS
    -0.012...
    >>> values = piece_value_func(game, return_action_values=True)
    >>> len(values)
    2
    >>> values[Move(src=SQ_1A, dst=SQ_1B)]  # doctest: +ELLIPSIS
    0.2289...
    >>> values[Move(src=SQ_2A, dst=SQ_1B)]  # doctest: +ELLIPSIS
    0.2289...
    """
    if return_action_values:
        g_tmp = g.copy()
        legal_moves = g_tmp.get_legal_moves()
        out: dict[Move, float] = {}
        for move in legal_moves:
            with g_tmp._apply_context(move):
                v = g_tmp._piece_value_func()
                out[move] = -v
        return out
    return g._piece_value_func()
