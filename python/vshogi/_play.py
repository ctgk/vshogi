from vshogi._game import Game
from vshogi._vshogi import Color, Result
from vshogi.engine._engine import Engine


def play_game(
    game: Game,
    player_black: Engine,
    player_white: Engine,
    *,
    search_args: dict = {'budget': 100},
    select_args: dict = {},
    max_moves: int = 320,
    draw_on_max_moves: bool = False,
    _return_num_searched: bool = False,
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
    draw_on_max_moves : bool
        If True, force the game to draw when the move limit is reached.

    Returns
    -------
    Game
        The game the two players played.
    """
    num_searched = []
    for i in range(max_moves):
        if game.result != Result.ONGOING:
            break
        player = player_black if game.turn == Color.BLACK else player_white
        player.set_game(game)
        player.search(**search_args)
        if _return_num_searched and (i < 2):
            num_searched.append(player.get_search_count())
        move = player.select(**select_args)
        game.apply(move)
    if draw_on_max_moves and game.result == Result.ONGOING:
        game.declare_draw()
    if _return_num_searched:
        return game, num_searched
    return game
