import logging

import numpy as np
from flask import Flask, jsonify, render_template, request

from vshogi.shogi import Color, Game, Move


app = Flask(__name__)
logging.basicConfig(level=logging.DEBUG)
game: Game = Game()


@app.route('/')
def show_board():
    board = np.rot90(np.asarray(game.board), k=-1).tolist()
    board = [[p.to_sfen() for p in row] for row in board]
    black_stand = {
        p.to_sfen(): n
        for p, n in game.stand(Color.BLACK).items()
        if n > 0
    }
    white_stand = {
        p.to_sfen(): n
        for p, n in game.stand(Color.WHITE).items()
        if n > 0
    }
    return render_template(
        'shogi_board.html',
        board=board,
        black_stand=black_stand,
        white_stand=white_stand,
        current_turn='black' if game.turn == Color.BLACK else 'white',
    )


@app.route('/ask_or_move', methods=['POST'])
def ask_or_move():
    data = request.get_json()
    logging.debug(f'make_move.data: {data}')
    src = data.get('src')
    dst = data.get('dst')

    if isinstance(src, dict):
        src = f'{9 - int(src["col"])}{chr(ord("a") + int(src["row"]))}'
        dst = f'{9 - int(dst["col"])}{chr(ord("a") + int(dst["row"]))}'
        move = Move(f'{src}{dst}')
    else:
        src = f'{src.lower()}*'
        dst = f'{9 - int(dst["col"])}{chr(ord("a") + int(dst["row"]))}'
        move = Move(f'{src}{dst}')
    logging.debug(f'make_move.move: {move}')
    move_promotion = Move(f'{src}{dst}+')

    move_is_legal = game.is_legal(move)
    promotion_is_legal = game.is_legal(move_promotion)
    if move_is_legal and promotion_is_legal:
        return jsonify({'legal': True, 'ask': True, 'src': src, 'dst': dst})
    elif move_is_legal:
        game.apply(move)
        logging.debug(f'make_move.game after game.apply(): {game.to_sfen()}')
        return jsonify({'legal': True, 'ask': False, 'src': src, 'dst': dst})
    elif promotion_is_legal:
        game.apply(move_promotion)
        logging.debug(f'make_move.game after game.apply(): {game.to_sfen()}')
        return jsonify({'legal': True, 'ask': False, 'src': src, 'dst': dst})
    else:
        logging.debug(f'make_move: {move} is illegal at {game.to_sfen()}')
        return jsonify({'legal': False, 'ask': False, 'src': src, 'dst': dst})


@app.route('/move', methods=['POST'])
def move():
    data = request.get_json()
    logging.debug(f'move(): {data}')
    src = data.get('src')
    dst = data.get('dst')
    promote = data.get('promote')
    move = Move(f'{src}{dst}' + ('+' if promote else ''))

    if game.is_legal(move):
        game.apply(move)
        logging.debug(f'make_move.game after game.apply(): {game.to_sfen()}')
        return jsonify({'success': True})
    else:
        logging.debug(f'make_move: {move} is illegal at {game.to_sfen()}')
        return jsonify({'success': False})


if __name__ == '__main__':
    app.run()
