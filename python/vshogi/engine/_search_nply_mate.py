import typing as tp
import warnings
from time import time

from vshogi._game import Game
from vshogi._vshogi import Color, Result


Move = tp.TypeVar('Move')


def _raise_error_if_ended(game: Game):
    if game.result != Result.ONGOING:
        raise ValueError(
            f'Input game ({game.to_sfen()}) has already been ended the game.')


def _is_mate_after_redundant_blocks(game: Game, checker_sq) -> bool:
    expected_result = (
        Result.BLACK_WIN if game.turn == Color.WHITE else Result.WHITE_WIN
    )
    for blk in game.get_legal_moves():
        if blk.destination == checker_sq:
            return False
        game.apply(blk)
        is_mate = False
        for atk in game.get_check_moves():
            if atk.destination != blk.destination:
                continue
            game.apply(atk)
            if (
                (game.result == expected_result)
                or _is_mate_after_redundant_blocks(game, atk.destination)
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
    start: float,
    max_duration: float,
    allow_redundant_blocks: bool = False,
) -> list[tuple[Move]]:
    _raise_error_if_ended(game)
    check_moves = game.get_check_moves()
    check_moves = sorted(check_moves, key=lambda m: not m.promote)
    out = []
    expected_result = (
        Result.BLACK_WIN if game.turn == Color.BLACK else Result.WHITE_WIN
    )
    for m in check_moves:
        if (max_duration is not None) and ((time() - start) > max_duration):
            return []
        if (not m.is_drop()):
            if (not m.promote) and ((type(m)(m.to_sfen() + '+'),) in out):
                continue
        game.apply(m)
        if game.result == expected_result:
            out.append((m,))
        game.undo()
    if out:
        return out
    for m in check_moves:
        if (max_duration is not None) and ((time() - start) > max_duration):
            return []
        if (not m.is_drop()):
            if (not m.promote) and ((type(m)(m.to_sfen() + '+'),) in out):
                continue
        game.apply(m)
        if (
            (not allow_redundant_blocks)
            and _is_mate_after_redundant_blocks(game, m.destination)
        ):
            out.append((m,))
        game.undo()
    return out


def _are_same_drop_destination(m1, m2):
    if not m1.is_drop():
        return False
    if not m2.is_drop():
        return False
    return m1.destination == m2.destination


def _is_duplicate_futile_interposition(
    mate_moves_1: tuple,
    mate_moves_2: tuple,
) -> bool:
    if len(mate_moves_1) != len(mate_moves_2):
        raise ValueError(
            'len(mate_moves_1) != len(mate_mates_2), '
            f'({len(mate_moves_1)} != {len(mate_moves_2)})')
    if mate_moves_1 == mate_moves_2:
        raise ValueError(
            f'`mate_moves_1` and `mate_moves_2` are identical, {mate_moves_1}')
    if any(m1 != m2 for m1, m2 in zip(mate_moves_1[::2], mate_moves_2[::2])):
        return False  # Found different attack moves
    return all(
        ((m1 == m2) or _are_same_drop_destination(m1, m2))
        for m1, m2 in zip(mate_moves_1[1::2], mate_moves_2[1::2]))


def _has_duplicate_futile_interposition(
    mates_list: list,
    mate_moves: tuple,
) -> bool:
    for m in mates_list:
        if _is_duplicate_futile_interposition(m, mate_moves):
            return True
    return False


def _remove_duplicate_futile_interposition(mates_list: list) -> list:
    out = []
    for mate_moves in mates_list:
        if not _has_duplicate_futile_interposition(out, mate_moves):
            out.append(mate_moves)
    return out


def search_nply_mate(
    game: Game,
    num_ply: int,
    remove_duplicate_futile_interposition: bool = True,
    *,
    max_duration_second: float = 10.,
) -> list[tuple[Move, ...]]:
    """Return checkmate moves less than the specified length.

    Parameters
    ----------
    game : Game
        Game position to search for checkmate.
    num_ply : int
        Maximum length of checkmate moves.

    Returns
    -------
    list[tuple[Move, ...]]
        List of checkmate moves less than the specified length.

    Examples
    --------
    >>> import vshogi.minishogi as shogi
    >>> g = shogi.Game('3pk/5/3G1/5/R4 b psgbr')
    >>> search_nply_mate(g, 1)
    [(Move(src=SQ_5E, dst=SQ_1E),)]
    """
    start = time()
    if num_ply not in (1, 3, 5):
        raise NotImplementedError(
            f'`num_ply == {num_ply}` is not currently supported')
    if num_ply == 5:
        warnings.warn(
            'Searching 5-ply mates involving redundant blocks is '
            'not accurate yet.')
    for n in range(1, num_ply + 1, 2):
        out = {
            1: _search_1ply_mate,
            3: _search_3ply_mate,
            5: _search_5ply_mate,
        }[n](game, start, max_duration_second)
        if out:
            if remove_duplicate_futile_interposition:
                return _remove_duplicate_futile_interposition(out)
            else:
                return out
    return []


def _search_2ply_mate(
    game: Game,
    start: float,
    max_duration: float,
    checker_sq,
) -> list[tuple[Move, ...]]:
    _raise_error_if_ended(game)
    legal_moves = game.get_legal_moves()
    redundant_block_cache = [None] * (game.ranks * game.files)
    out = []
    for m in legal_moves:
        if (max_duration is not None) and ((time() - start) > max_duration):
            return []
        game.apply(m)
        if game.result != Result.ONGOING:
            game.undo()
            return []  # no mate
        if (m.destination != checker_sq):
            mates_1ply = _search_1ply_mate(game, start, max_duration, False)
            if len(mates_1ply) == 0:
                d = int(m.destination)
                if m.is_drop():
                    if redundant_block_cache[d] is None:
                        mate_moves = _search_3ply_mate(
                            game, start, max_duration, m.destination)
                        mate_moves = [
                            m for m in mate_moves
                            if game.stand(game.turn).get(m[-1].source, 1)
                        ]
                        redundant_block_cache[d] = bool(mate_moves)
                    if redundant_block_cache[d]:
                        game.undo()
                        continue
                else:
                    mate_moves = _search_3ply_mate(
                        game, start, max_duration, m.destination)
                    mate_moves = [
                        m for m in mate_moves
                        if game.stand(game.turn).get(m[-1].source, 1)
                    ]
                    if bool(mate_moves):
                        game.undo()
                        continue
        else:
            mates_1ply = _search_1ply_mate(game, start, max_duration, False)
        if len(mates_1ply) == 0:
            game.undo()
            return []  # No mate
        out.extend([(m, *moves) for moves in mates_1ply])
        game.undo()
    return out


def _search_3ply_mate(
    game: Game,
    start,
    max_duration,
    target=None,
) -> list[tuple[Move, ...]]:
    _raise_error_if_ended(game)
    check_moves = game.get_check_moves()
    check_moves = sorted(check_moves, key=lambda m: not m.promote)
    out = []
    for m in check_moves:
        if (max_duration is not None) and ((time() - start) > max_duration):
            return []
        if (target is not None) and (target != m.destination):
            continue
        if (not m.is_drop()):
            if (
                (not m.promote)
                and ((type(m)(m.to_sfen() + '+'),) in [(o[0],) for o in out])
            ):
                continue
        game.apply(m)
        mates_2ply = _search_2ply_mate(
            game, start, max_duration, m.destination)
        out.extend([(m, *moves) for moves in mates_2ply])
        game.undo()
    return out


def _search_4ply_mate(
    game: Game,
    start,
    max_duration,
) -> list[tuple[Move, ...]]:
    _raise_error_if_ended(game)
    legal_moves = game.get_legal_moves()
    out = []
    for m in legal_moves:
        game.apply(m)
        if game.result != Result.ONGOING:
            game.undo()
            return []  # no mate
        if _search_1ply_mate(game, start, max_duration):
            game.undo()
            continue
        mates_3ply = _search_3ply_mate(game, start, max_duration)
        if len(mates_3ply) == 0:
            game.undo()
            return []  # No mate
        out.extend([(m, *moves) for moves in mates_3ply])
        game.undo()
    return out


def _search_5ply_mate(
    game: Game,
    start,
    max_duration,
) -> list[tuple[Move, ...]]:
    _raise_error_if_ended(game)
    check_moves = game.get_check_moves()
    check_moves = sorted(check_moves, key=lambda m: not m.promote)
    out = []
    for m in check_moves:
        if (max_duration is not None) and ((time() - start) > max_duration):
            return []
        if (not m.is_drop()):
            if (
                (not m.promote)
                and ((type(m)(m.to_sfen() + '+'),) in [(o[0],) for o in out])
            ):
                continue
        game.apply(m)
        mates_4ply = _search_4ply_mate(game, start, max_duration)
        out.extend([(m, *moves) for moves in mates_4ply])
        game.undo()
    return out
