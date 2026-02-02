import typing as tp

from vshogi._game import Game
from vshogi._vshogi import Color, Result


Move = tp.TypeVar("Move")


def _is_unnecessary_declined_promotion(
    move: Move,
    out: list[tuple[Move]],
) -> bool:
    if move.is_drop() or move.promote:
        return False
    return type(move)(move.to_sfen() + "+") in [t[0] for t in out]


def _search_defence(
    game: Game,
    max_ply: int,
    *,
    search_all: bool,
) -> list[tuple[Move]]:
    if max_ply <= 0:
        return []
    if max_ply % 2 == 1:
        msg = f"`max_ply` must be an even number, but was {max_ply}"
        raise ValueError(msg)
    out: list[tuple[Move, Move]] = []
    forced_mate: bool = True
    legal_moves = sorted(
        game.get_legal_moves(),
        key=lambda m: game.is_aigoma(m),
    )
    for m in legal_moves:
        is_aigoma = game.is_aigoma(m)
        with game._apply_context(m):
            if mates := _search_offence(
                game,
                max_ply=max_ply - 1,
                search_all=search_all,
            ):
                out.extend([(m, *mate) for mate in mates])
            elif (
                not is_aigoma
                or not out
                or not _search_offence(
                    game,
                    max_ply=max_ply + 1,
                    dst=m.destination,
                    search_all=False,
                )
            ):
                forced_mate = False
        if not forced_mate:
            break
    if forced_mate:
        max_len = max(len(mate) for mate in out)
        out = [mate for mate in out if len(mate) == max_len]
        out = _remove_virtually_duplicating_interposition(out)
    return out if forced_mate else []


def _remove_virtually_duplicating_interposition(
    out: list[tuple[Move]],
) -> list[tuple[Move]]:
    removed: list[tuple[Move]] = []
    for mate in out:
        virtual_dupes = [
            r
            for r in removed
            if (r[0].destination == mate[0].destination)
            and (r[1:] == mate[1:])
        ]
        if not virtual_dupes:
            removed.append(mate)
    return removed


def _is_tsumi_with_futile_block(game: Game) -> bool:
    assert game.turn == Color.WHITE
    if game.result != Result.ONGOING:
        return False
    is_tsumi: bool = True
    for m in game.get_legal_moves():
        if not game.is_aigoma(m):
            return False
        with game._apply_context(m):
            if not _search_offence(
                game,
                max_ply=1,
                dst=m.destination,
                search_all=False,
            ):
                is_tsumi = False
        if not is_tsumi:
            return False
    return is_tsumi


def _search_offence(
    game: Game,
    max_ply: int,
    *,
    search_all: bool,
    dst=None,
) -> list[tuple[Move]]:
    if max_ply < 0:
        return []
    assert game.turn == Color.BLACK
    assert max_ply % 2 == 1
    out: list[tuple[Move, ...]] = []
    check_moves = sorted(
        game.get_check_moves(),
        key=lambda m: -m.promote,
        # key=lambda m: 2 * m.is_drop() - m.promote,
    )
    if dst is not None:
        check_moves = [m for m in check_moves if m.destination == dst]
    for m in check_moves:
        if _is_unnecessary_declined_promotion(m, out):
            continue
        with game._apply_context(m, banish=dst is not None):
            if game.result == Result.BLACK_WIN:
                out.append((m,))
            elif mates := _search_defence(
                game,
                max_ply=max_ply - 1,
                search_all=search_all,
            ):
                out.extend([(m, *mate) for mate in mates])
        if not search_all and out:
            break
    if not out:
        for m in check_moves:
            if _is_unnecessary_declined_promotion(m, out):
                continue
            with game._apply_context(m):
                if _is_tsumi_with_futile_block(game):
                    out.append((m,))
            if not search_all and out:
                break
    if out:
        min_len = min(len(mate) for mate in out)
        out = [mate for mate in out if len(mate) == min_len]
    return out


def _solve_tsumeshogi(
    game: Game,
    max_ply: int,
    *,
    search_all: bool,
) -> list[tuple[Move]]:
    t = game.turn
    if max_ply % 2 == 0:
        if t == Color.BLACK:
            game = game.rotate()
        mates = _search_defence(
            game,
            max_ply=max_ply,
            search_all=search_all,
        )
        if t == Color.BLACK:
            return [tuple(m.rotate() for m in mate) for mate in mates]
        return mates
    else:
        if t == Color.WHITE:
            game = game.rotate()
        mates = _search_offence(
            game,
            max_ply=max_ply,
            search_all=search_all,
        )
        if t == Color.WHITE:
            return [tuple(m.rotate() for m in mate) for mate in mates]
        return mates


def _is_exact(game: Game) -> bool:
    is_exact = (
        sum(
            game.stand(
                Color.BLACK if game.turn == Color.WHITE else Color.WHITE
            ).values()
        )
        == 0
    )
    return is_exact


def _remove_redundant_mate(
    game: Game,
    mates: list[tuple[Move]],
) -> list[tuple[Move]]:
    if not mates:
        return []

    out: list[tuple[Move]] = []
    found_redundant_mate: bool = False
    for m0 in set(mate[0] for mate in mates):
        with game._apply_context(m0):
            if len(mates[0]) == 1:
                if _is_exact(game):
                    out.append((m0,))
                else:
                    found_redundant_mate = True
            else:
                if removed := _remove_redundant_mate(
                    game, [mate[1:] for mate in mates if mate[0] == m0]
                ):
                    out.extend([(m0, *mate) for mate in removed])
                else:
                    found_redundant_mate = True
    return [] if found_redundant_mate and len(mates[0]) % 2 == 1 else out


def solve_tsumeshogi(
    game: Game,
    max_ply: int = 5,
    *,
    exact: bool = True,
) -> list[tuple[Move]]:
    """Solve tsumeshogi puzzle.

    Parameters
    ----------
    game : Game
        Game position of the puzzle.
    max_ply : int
        Maximum number of ply. Note that it does not count futile blocks.
    exact : bool, optional
        Return exact mates only if true, otherwise return all mates that
        include redundant pieces left in stand. By default True.

    Returns
    -------
    list[tuple[Move]]
        The answers of the puzzle.
    """
    if not isinstance(max_ply, int):
        msg = f"`max_ply` must be an instance of int, but was {type(max_ply)}"
        raise TypeError(msg)
    out = _solve_tsumeshogi(game, max_ply=max_ply, search_all=True)
    return _remove_redundant_mate(game, out) if exact else out
