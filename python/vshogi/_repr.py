def _repr_enum(self) -> str:
    return f'{self.__class__.__name__}.{self.name}'


def _repr_move(m) -> str:
    return (
        f'{m.__class__.__name__}(dst={m.destination.name}, src={m.source.name}'
        + (', promote=True' if getattr(m, 'promote', False) else '')
        + ')'
    )


def _repr_square(sq) -> str:
    return f'{sq.__class__.__name__}.{sq.name[-2:]}'


def _repr_stand(s) -> str:
    return '-' if not s.any() else ','.join([
        k.name + ('' if v == 1 else f'x{v}') for k, v in s.to_dict().items()
        if v > 0
    ])
