import copy
import os
import timeit
from glob import glob

import numpy as np
import pytest
import shogi
import shogi.CSA

import cshogi


def read_kifu_cshogi(filepath: str):
    positions = []
    parser = cshogi.Parser()
    parser.parse_csa_file(filepath)
    board = cshogi.Board()
    for move, score in zip(parser.moves, parser.scores):
        hcpe = np.empty(1, dtype=cshogi.HuffmanCodedPosAndEval)
        board.to_hcp(hcpe[0]['hcp'])
        hcpe[0]['eval'] = score
        hcpe[0]['bestMove16'] = cshogi.move16(move)
        hcpe[0]['gameResult'] = parser.win
        positions.append(hcpe)
        board.push(move)
    return positions


def read_kifu_shogi(filepath: str):
    positions = []
    kifu = shogi.CSA.Parser.parse_file(filepath)[0]
    board = shogi.Board()
    for move in kifu['moves']:
        piece_bb = copy.deepcopy(board.piece_bb)
        occupied = copy.deepcopy((board.occupied[shogi.BLACK], board.occupied[shogi.WHITE]))
        pieces_in_hand = copy.deepcopy((board.pieces_in_hand[shogi.BLACK], board.pieces_in_hand[shogi.WHITE]))

        positions.append((piece_bb, occupied, pieces_in_hand, move, kifu['win']))
        board.push_usi(move)
    return positions


def test_time():
    filepath_list = glob(os.path.join(os.path.dirname(__file__), 'assets', '*.csa'))
    for filepath in filepath_list:
        duration_cshogi = timeit.timeit(lambda: read_kifu_cshogi(filepath), number=100)
        duration_shogi = timeit.timeit(lambda: read_kifu_shogi(filepath), number=100)
        print(f'\n{filepath.split("/")[-1]}: cshogi {duration_cshogi:.4f}[sec], shogi {duration_shogi:.4f}[sec]')
        assert duration_cshogi < duration_shogi


if __name__ == '__main__':
    pytest.main([__file__])
