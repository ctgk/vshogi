from vshogi._game import Game
from vshogi._vshogi import Color, Result
from vshogi.engine._mcts import MonteCarloTreeSearcher


def play_game(
    game: Game,
    player_black: MonteCarloTreeSearcher,
    player_white: MonteCarloTreeSearcher,
    *,
    max_moves: int = 400,
    search_args: dict = {},
) -> Game:
    """Make two players play the game until an end.

    Parameters
    ----------
    game : Game
        Game to make two players play
    player_black : MonteCarloTreeSearcher
        First player
    player_white : MonteCarloTreeSearcher
        Second player
    max_moves : int
        Maximum number of moves to apply to the game.
        If it reaches the value, return the game even if it is ongoing.
    search_args : dict
        Arguments to pass to players' `search` method.

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
        move = player.select()
        game.apply(move)
    return game
