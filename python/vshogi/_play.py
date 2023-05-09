from vshogi._game import Game
from vshogi._vshogi import Color, Result
from vshogi.mcts._mcts import MonteCarloTreeSearcher


def play_game(
    game: Game,
    player_black: MonteCarloTreeSearcher,
    player_white: MonteCarloTreeSearcher,
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

    Returns
    -------
    Game
        The game the two players played.
    """
    while True:
        if game.result != Result.ONGOING:
            break
        player = player_black if game.turn == Color.BLACK else player_white
        player.set_root(game)
        player.explore()
        move = player.select()
        game.apply(move)
    return game
