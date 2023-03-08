from vshogi._vshogi.animal_shogi import Move


def _move_repr(self: Move) -> str:
    return f'Move({self.source().name} -> {self.destination().name})'


Move.__repr__ = _move_repr
