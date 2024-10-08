from vshogi._game import Game


def piece_value_func(g: Game) -> float:
    """Return turn player's value of the current state based on piece values.

    Parameters
    ----------
    g : Game
        The current game state to evaluate.

    Returns
    -------
    float
        Turn player's value of the current state. The value ranges between -1
        to 1.

    Examples
    --------
    >>> from vshogi.minishogi import *
    >>> game = Game()
    >>> piece_value_func(game)
    0.0
    >>> piece_value_func(game.apply("1e1b"))  # doctest: +ELLIPSIS
    -0.012...
    """
    return g._piece_value_func()
