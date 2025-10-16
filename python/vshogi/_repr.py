from string import ascii_uppercase


def _repr_enum(self) -> str:
    return f'{self.__class__.__name__}.{self.name}'


def _repr_move(m) -> str:
    return (
        f'{m.__class__.__name__}(src={m.source.name}, dst={m.destination.name}'
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


def _repr_board(b) -> str:
    nf = b.num_files
    nr = b.num_ranks
    top_label = ' ' + ''.join(f'   {nf - i}' for i in range(nf)) + '\n'
    divider = '  +' + '+'.join('---' for _ in range(nf)) + '+\n'
    row = '|' + '|'.join('{}' for _ in range(nf)) + '|\n'
    template = (
        top_label + divider
        + divider.join(f'{c} ' + row for c in ascii_uppercase[:nr])
        + divider[:-1]
    )
    return template.format(*[
        b[f * nr + r]._to_3char()
        for r in range(nr) for f in range(nf - 1, -1, -1)
    ])
