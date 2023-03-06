from vshogi import animal_shogi


_modules = [animal_shogi]


__all__ = [_m.__name__ for _m in _modules]


del _modules
