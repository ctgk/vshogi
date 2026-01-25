import typing as tp
import xml.etree.ElementTree as ElementTree

import numpy as np

from vshogi._vshogi import Color


NUMBER_JPN_SYMBOLS = [
    None,
    "一",
    "二",
    "三",
    "四",
    "五",
    "六",
    "七",
    "八",
    "九",
    "十",
    "十一",
    "十二",
    "十三",
    "十四",
    "十五",
    "十六",
    "十七",
    "十八",
]


class _SvgWrapper(str):
    def _repr_svg_(self):
        return self


def _to_svg(
    game: 'Game',
    lastmove: tp.Optional['Move'] = None,
    scale: float = 1.0,
    *,
    skip_white_stand: bool = False,
):
    # width = 230
    # height = 192
    width = int(20 + 0.5 + 20 * game.files + 0.5 + 9 + 20)
    height = int(10 + 1 + 0.5 + 180 + 0.5)

    svg = ElementTree.Element(
        "svg",
        {
            "xmlns": "http://www.w3.org/2000/svg",
            "version": "1.1",
            "xmlns:xlink": "http://www.w3.org/1999/xlink",
            "width": str(width * scale),
            "height": str(height * scale),
            "viewBox": "0 0 {} {}".format(width, height),
        },
    )

    defs = ElementTree.SubElement(svg, "defs")
    tf = 'transform="rotate(180)"'
    font = 'font-family="serif"'
    for name, p in game._get_board_piece_class().__members__.items():
        if name == 'VOID':
            continue
        defs.append(
            ElementTree.fromstring(
                f'<g id="{p.to_sfen()}"'
                + ('' if name.startswith('B_') else f' {tf}')
                + '>'
                + f'<text {font} font-size="17" text-anchor="middle" '
                + (
                    'x="10.5" y="16.5">'
                    if name.startswith('B_')
                    else 'x="-10.5" y="-3.5">'
                )
                + f'{p.to_jpn()}'
                + '</text>'
                + '</g>',
            )
        )

    if lastmove is not None:
        f, r = divmod(int(lastmove.destination), game.ranks)
        ElementTree.SubElement(
            svg,
            "rect",
            {
                "x": str(20.5 + (game.files - 1 - f) * 20),
                "y": str(10.5 + r * 20),
                "width": str(20),
                "height": str(20),
                "fill": "#fdfd96",
            },
        )

    svg.append(
        ElementTree.fromstring(
            '<g stroke="black">'
            + f'<rect x="20" y="10" width="{20 * game.files + 1}" '
            + f'height="{20 * game.ranks + 1}" fill="none" stroke-width="1.5" '
            + '/>'
            + ''.join(
                [
                    (
                        '<line x1="20.5" y1="{y}" x2="{x}" y2="{y}" '
                        'stroke-width="1.0" />'
                    ).format(x=20.5 + 20 * game.files, y=30.5 + 20 * r)
                    for r in range(game.ranks - 1)
                ]
            )
            + ''.join(
                [
                    (
                        '<line x1="{x}" y1="10.5" x2="{x}" y2="{y}" '
                        'stroke-width="1.0" />'
                    ).format(x=20.5 + 20 * f, y=10.5 + 20 * game.ranks)
                    for f in range(1, game.files)
                ]
            )
            + '</g>',
        )
    )
    svg.append(
        ElementTree.fromstring(
            '<g>'
            + ''.join(
                [
                    (
                        f'<text {font} text-anchor="middle" font-size="9" '
                        + f'x="{30.5 + 20 * f}" y="8">{game.files - f}</text>'
                    )
                    for f in range(game.files)
                ]
            )
            + ''.join(
                [
                    (
                        f'<text {font} font-size="9" '
                        f'x="{21 + 20 * game.files + 2.5}" '
                        f'y="{3 + 20 * r}">{c}</text>'
                    )
                    for r, c in zip(
                        range(1, game.ranks + 1), "一二三四五六七八九"
                    )
                ]
            )
            + '</g>',
        )
    )

    for i, p in enumerate(np.asarray(game.board).ravel()):
        if 'VOID' not in str(p):
            f, r = divmod(i, game.ranks)
            x = 20.5 + (game.files - 1 - f) * 20
            y = 10.5 + r * 20
            ElementTree.SubElement(
                svg,
                'use',
                {
                    'xlink:href': "#{}".format(p.to_sfen()),
                    'x': str(x),
                    'y': str(y),
                },
            )

    hand_pieces = [[], []]
    for c in (Color.BLACK, Color.WHITE):
        i = 0
        for p, n in game.stand(c).items():
            if n >= 11:
                hand_pieces[c].append((i, NUMBER_JPN_SYMBOLS[n % 10]))
                i += 1
                hand_pieces[c].append((i, NUMBER_JPN_SYMBOLS[10]))
                i += 1
            elif n >= 2:
                hand_pieces[c].append((i, NUMBER_JPN_SYMBOLS[n]))
                i += 1
            if n >= 1:
                hand_pieces[c].append((i, p.to_jpn()))
                i += 1
        i += 1
        hand_pieces[c].append((i, "手"))
        i += 1
        hand_pieces[c].append((i, "先" if c == Color.BLACK else "後"))
        i += 1
        hand_pieces[c].append((i, "☗" if c == Color.BLACK else "☖"))

    for c in (Color.BLACK, Color.WHITE):
        if skip_white_stand and (c == Color.WHITE):
            continue
        if c == Color.BLACK:
            x = 34 + 20 * game.files
            y = 10 + 20 * game.ranks
        else:
            x = -16
            y = -10
        scale = 1
        if len(hand_pieces[c]) + 1 > 13:
            scale = 13.0 / (len(hand_pieces[c]) + 1)
        for i, text in hand_pieces[c]:
            e = ElementTree.SubElement(
                svg,
                "text",
                {
                    "font-family": "serif",
                    "font-size": str(14 * scale),
                },
            )
            e.set("x", str(x))
            e.set("y", str(y - 14 * scale * i))
            if c == Color.WHITE:
                e.set("transform", "rotate(180)")
            e.text = text

    return _SvgWrapper(ElementTree.tostring(svg).decode("utf-8"))
