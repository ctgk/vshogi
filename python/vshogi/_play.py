from vshogi._game import Game
from vshogi._vshogi import Color, Result
from vshogi.engine._engine import Engine


def play_game(
    game: Game,
    player_black: Engine,
    player_white: Engine,
    *,
    search_args: dict = {},
    select_args: dict = {},
    max_moves: int = 320,
) -> Game:
    """Make two players play the game until an end.

    Parameters
    ----------
    game : Game
        Game to make two players play
    player_black : Engine
        First player
    player_white : Engine
        Second player
    search_args : dict
        Arguments to pass to players' `search` method.
    select_args : dict
        Arguments to pass to players' `select` method.
    max_moves : int
        Maximum number of moves to apply to the game.
        If it reaches the value, return the game even if it is ongoing.

    Returns
    -------
    Game
        The game the two players played.
    """
    for _ in range(max_moves):
        if game.result != Result.ONGOING:
            break
        player = player_black if game.turn == Color.BLACK else player_white
        player.set_game(game)
        player.search(**search_args)
        move = player.select(**select_args)
        game.apply(move)
    return game
