import numpy as np

import vshogi._vshogi as _vs
from vshogi._constants import (
    COLORS, BLACK, WHITE, HAND_PIECES, HAND_PIECE_JAPANESE_SYMBOLS,
    NUMBER_JAPANESE_KANJI_SYMBOLS,
    SVG_COORDINATES, SVG_PIECE_DEFS, SVG_PIECE_DEF_IDS, SVG_SQUARES,
    SQUARES, NONE,
)


class SvgWrapper(str):

    def _repr_svg_(self):
        return self


class Board:

    def __init__(self, sfen: str = None, board: 'Board' = None):
        if sfen is not None:
            self._board = _vs._Board(sfen)
        elif board is not None:
            self._board = _vs._Board(board._board)
        else:
            self._board = _vs._Board()

    def __copy__(self):
        return Board(board=self)

    def copy(self):
        return Board(board=self)

    def set_sfen(self, sfen: str):
        self._board.set(sfen)

    def set_position(self, position: str):
        return self._board.set_position(position)

    def set_pieces(self, pieces: list, pieces_in_hand: tuple):
        raise NotImplementedError

    def set_hcp(self, hcp: np.ndarray):
        return self._board.set_hcp(hcp.data)

    def set_psfen(self, psfen: np.ndarray):
        return self._board.set_psfen(psfen.data)

    def reset(self):
        self._board.reset()

    def __repr__(self):
        return self._board.dump()

    def push(self, move: int):
        self._board.push(move)

    def push_usi(self, usi: str):
        move: int = self._board.move_from_usi(usi)
        if move != 0:
            self._board.push(move)
        return move

    def push_csa(self, csa: str):
        move: int = self._board.move_from_csa(csa)
        if move != 0:
            self._board.push(move)
        return move

    def push_move16(self, move16: int):
        move: int = self._board.move_from_move16(move16)
        self._board.push(move)
        return move

    def push_psv(self, psv: int):
        move: int = self._board.move_from_psv(psv)
        self._board.push(move)
        return move

    def push_pass(self):
        assert not self.is_check()
        return self._board.push_pass()

    def pop_pass(self):
        self._board.pop_pass()

    def pop(self):
        return self._board.pop()

    def peek(self):
        return self._board.peek()

    @property
    def history(self):
        return self._board.get_history()

    def is_game_over(self):
        return self._board.is_game_over()

    def is_draw(self, ply: int = None):
        if ply is None:
            return self._board.isDraw(2147483647)
        return self._board.isDraw(ply)

    def move(self, from_square: int, to_square: int, promotion: bool):
        return self._board.move(from_square, to_square, promotion)

    def drop_move(self, to_square: int, drop_piece_type: int):
        return self._board.drop_move(to_square, drop_piece_type)

    def move_from_usi(self, usi: str):
        return self._board.move_from_usi(usi)

    def move_from_csa(self, csa: str):
        return self._board.move_from_csa(csa)

    def move_from_move16(self, move16: int):
        return self._board.move_from_move16(move16)

    def move_from_psv(self, psv: int):
        return self._board.move_from_psv(psv)

    @property
    def legal_moves(self):
        return LegalMoveList(self)

    @property
    def pseudo_legal_moves(self):
        return PseudoLegalMoveList(self)

    @property
    def turn(self):
        return self._board.turn()

    @turn.setter
    def turn(self, turn_: int):
        self._board.setTurn(turn_)

    @property
    def move_number(self):
        return self._board.ply()

    @move_number.setter
    def move_number(self, ply: int):
        assert ply > 0
        self._board.setPly(ply)

    def sfen(self):
        return self._board.toSFEN()

    def csa_pos(self):
        return self._board.toCSAPos()

    def to_hcp(self, hcp: np.ndarray):
        return self._board.toHuffmanCodedPos(hcp)

    def to_psfen(self, hcp: np.ndarray):
        return self._board.toPackedSfen(hcp.data)

    def piece(self, sq: int):
        return self._board.piece(sq)

    def piece_type(self, sq: int):
        return _vs.piece_to_piece_type(self.piece(sq))

    def is_check(self):
        return self._board.inCheck()

    def mate_move_in_1ply(self):
        return self._board.mateMoveIn1Ply()

    def mate_move(self, ply: int):
        assert ply % 2 == 1
        assert ply >= 3
        return self._board.mateMove(ply)

    def is_mate(self, ply: int):
        assert ply % 2 == 0
        return self._board.inCheck() and self._board.is_mate(ply)

    def zobrist_hash(self):
        return self._board.getKey()

    def is_pseudo_legal(self, move: int):
        return self._board.moveIsPseudoLegal(move)

    def is_legal(self, move: int):
        return self._board.moveIsLegal(move)

    @property
    def pieces_in_hand(self):
        return (self._board.pieces_in_hand(0), self._board.pieces_in_hand(1))

    @property
    def pieces(self):
        return self._board.pieces()

    def is_nyugyoku(self):
        return self._board.is_nyugyoku()

    def piece_planes(self, features: np.ndarray):
        return self._board.piece_planes(features.data)

    def piece_planes_rotate(self, features: np.ndarray):
        return self._board.piece_planes_rotate(features.data)

    def _dlshogi_make_input_features(self, features1: np.ndarray, features2: np.ndarray):
        self._board._dlshogi_make_input_features(features1.data, features2.data)

    def is_ok(self):
        return self._board.isOK()

    def book_key(self):
        return self._board.bookKey()

    def to_svg(self, lastmove=None, scale=1.):
        import xml.etree.ElementTree as ET

        width = 230
        height = 192

        svg = ET.Element("svg", {
            "xmlns": "http://www.w3.org/2000/svg",
            "version": "1.1",
            "xmlns:xlink": "http://www.w3.org/1999/xlink",
            "width": str(width * scale),
            "height": str(height * scale),
            "viewBox": "0 0 {} {}".format(width, height),
        })

        defs = ET.SubElement(svg, "defs")
        for piece_def in SVG_PIECE_DEFS:
            defs.append(ET.fromstring(piece_def))

        if lastmove is not None:
            i, j = divmod(_vs.move_to(lastmove), 9)
            ET.SubElement(svg, "rect", {
                "x": str(20.5 + (8 - i) * 20),
                "y": str(10.5 + j * 20),
                "width": str(20),
                "height": str(20),
                "fill": "#f6b94d"
            })
            if not _vs.move_is_drop(lastmove):
                i, j = divmod(_vs.move_from(lastmove), 9)
                ET.SubElement(svg, "rect", {
                    "x": str(20.5 + (8 - i) * 20),
                    "y": str(10.5 + j * 20),
                    "width": str(20),
                    "height": str(20),
                    "fill": "#fdf0e3"
                })

        svg.append(ET.fromstring(SVG_SQUARES))
        svg.append(ET.fromstring(SVG_COORDINATES))

        for sq in SQUARES:
            pc = self.__board.piece(sq)
            if pc != NONE:
                i, j = divmod(sq, 9)
                x = 20.5 + (8 - i) * 20
                y = 10.5 + j * 20

                ET.SubElement(svg, "use", {
                    "xlink:href": "#{}".format(SVG_PIECE_DEF_IDS[pc]),
                    "x": str(x),
                    "y": str(y),
                })

        hand_pieces = [[], []]
        for c in COLORS:
            i = 0
            for hp, n in zip(HAND_PIECES, self.__board.pieces_in_hand(c)):
                if n >= 11:
                    hand_pieces[c].append((i, NUMBER_JAPANESE_KANJI_SYMBOLS[n % 10]))
                    i += 1
                    hand_pieces[c].append((i, NUMBER_JAPANESE_KANJI_SYMBOLS[10]))
                    i += 1
                elif n >= 2:
                    hand_pieces[c].append((i, NUMBER_JAPANESE_KANJI_SYMBOLS[n]))
                    i += 1
                if n >= 1:
                    hand_pieces[c].append((i, HAND_PIECE_JAPANESE_SYMBOLS[hp]))
                    i += 1
            i += 1
            hand_pieces[c].append((i, "手"))
            i += 1
            hand_pieces[c].append((i, "先" if c == BLACK else "後"))
            i += 1
            hand_pieces[c].append(( i, "☗" if c == BLACK else "☖"))

        for c in COLORS:
            if c == BLACK:
                x = 214
                y = 190
            else:
                x = -16
                y = -10
            scale = 1
            if len(hand_pieces[c]) + 1 > 13:
                scale = 13.0 / (len(hand_pieces[c]) + 1)
            for i, text in hand_pieces[c]:
                e = ET.SubElement(svg, "text", {
                    "font-family": "serif",
                    "font-size": str(14 * scale),
                })
                e.set("x", str(x))
                e.set("y", str(y - 14 * scale * i))
                if c == WHITE:
                    e.set("transform", "rotate(180)")
                e.text = text

        return SvgWrapper(ET.tostring(svg).decode("utf-8"))

    def _repr_svg_(self):
        move = self.__board.peek()
        if move == 0:
            return self.to_svg()
        else:
            return self.to_svg(move)


class LegalMoveList:

    def __init__(self, board: Board) -> None:
        self._ml = _vs._LegalMoveList(board._board)

    def __iter__(self):
        return self

    def __next__(self):
        if self._ml.end():
            raise StopIteration()
        move = self._ml.move()
        self._ml.next()
        return move

    def __len__(self):
        return self._ml.size()


class PseudoLegalMoveList:

    def __init__(self, board: Board) -> None:
        self._ml = _vs._PseudoLegalMoveList(board._board)

    def __iter__(self):
        return self

    def __next__(self):
        if self._ml.end():
            raise StopIteration()
        move = self._ml.move()
        self._ml.next()
        return move

    def __len__(self):
        return self._ml.size()
