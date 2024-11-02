import numpy as np
from flask import Flask, jsonify, render_template, request

from vshogi.shogi import Color, Game, Move, Square


app = Flask(__name__)
game = Game()


@app.route('/')
def show_board():
    board = np.asarray(game.board).tolist()
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
    )


@app.route('/move', methods=['POST'])
def move():
    data = request.json
    dst = Square(data['to'][0] * 9 + data['to'][1])
    src = Square(data['from'][0] * 9 + data['from'][1])
    move = Move(dst, src)
    if not game.is_legal(move):
        return jsonify({'valid': False, 'message': 'Illegal move!'})
    game.apply(move)
    return jsonify({'valid': True})


if __name__ == '__main__':
    app.run()
