import typing as tp

from vshogi._game import Game
from vshogi._vshogi import Color, Result


Move = tp.TypeVar('Move')


def _raise_error_if_ended(game: Game):
    if game.result != Result.ONGOING:
        raise ValueError(
            f'Input game ({game.to_sfen()}) has already been ended the game.')


def _is_mate_after_redundant_blocks(game: Game) -> bool:
    expected_result = (
        Result.BLACK_WIN if game.turn == Color.WHITE else Result.WHITE_WIN
    )
    block_moves = game.get_legal_moves()
    for blk in block_moves:
        game.apply(blk)
        is_mate = False
        for atk in game.get_legal_moves():
            if atk.destination != blk.destination:
                continue
            game.apply(atk)
            if not game.in_check():
                game.undo()
                continue
            if (
                (game.result == expected_result)
                or _is_mate_after_redundant_blocks(game)
            ):
                is_mate = True
                game.undo()
                break
            game.undo()
        game.undo()
        if not is_mate:
            return False
    return True


def _search_1ply_mate(
    game: Game,
    allow_redundant_blocks: bool,
) -> tp.List[tp.Tuple[Move]]:
    _raise_error_if_ended(game)
    legal_moves = game.get_legal_moves()
    out = []
    expected_result = (
        Result.BLACK_WIN if game.turn == Color.BLACK else Result.WHITE_WIN
    )
    for m in legal_moves:
        game.apply(m)
        if not game.in_check():
            game.undo()
            continue
        if (
            (game.result == expected_result)
            or (
                (not allow_redundant_blocks)
                and _is_mate_after_redundant_blocks(game)
            )
        ):
            out.append((m,))
        game.undo()
    return out


def search_nply_mate(
    game: Game,
    num_ply: int,
    *,
    allow_redundant_blocks: bool = False,
) -> tp.List[tp.Tuple[Move, ...]]:
    """Return checkmate moves less than the specified length.

    Parameters
    ----------
    game : Game
        Game position to search for checkmate.
    num_ply : int
        Maximum length of checkmate moves.

    Returns
    -------
    tp.List[tp.Tuple[Move, ...]]
        List of checkmate moves less than the specified length.

    Examples
    --------
    >>> import vshogi.minishogi as shogi
    >>> g = shogi.Game('3pk/5/3G1/5/R4 b psgbr')
    >>> search_nply_mate(g, 1)
    [(Move(dst=SQ_1E, src=SQ_5E),)]
    >>> search_nply_mate(g, 1, allow_redundant_blocks=True)
    []
    """
    if num_ply != 1:
        raise NotImplementedError('Only `num_ply == 1` is supported currently')
    return _search_1ply_mate(game, allow_redundant_blocks)


def _search_2ply_mate(game: Game) -> tp.List[tp.Tuple[Move, ...]]:
    _raise_error_if_ended(game)
    legal_moves = game.get_legal_moves()
    out = []
    for m in legal_moves:
        game.apply(m)
        mates_1ply = _search_1ply_mate(game, False)
        if len(mates_1ply) == 0:
            game.undo()
            return []  # No mate
        out.extend([(m, *moves) for moves in mates_1ply])
        game.undo()
    return out


def _search_3ply_mate(game: Game) -> tp.List[tp.Tuple[Move, ...]]:
    _raise_error_if_ended(game)
    legal_moves = game.get_legal_moves()
    out = []
    for m in legal_moves:
        game.apply(m)
        if game.in_check():
            mates_2ply = _search_2ply_mate(game)
            out.extend([(m, *moves) for moves in mates_2ply])
        game.undo()
    return out


if __name__ == '__main__':
    import vshogi.minishogi as shogi
    g = shogi.Game('2pp1/3k1/5/3P1/R4 b Gsgbr')
    print(g)
    print(_search_3ply_mate(g))
