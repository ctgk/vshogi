def _repr_move(m) -> str:
    return (
        f'{m.__class__.__name__}(dst={m.destination.__name__}, '
        + f'src={m.source.__name__}'
        + (', promote=True' if getattr(m, 'promote', False) else '')
        + ')'
    )


def _repr_stand(s) -> str:
    return '-' if not s.any() else ','.join([
        k.__name__ + ('' if v == 1 else f'x{v}')
        for k, v in s.to_dict().items() if v > 0
    ])
