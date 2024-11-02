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
    move, move_promote = Move(dst, src), Move(dst, src, promote=True)
    move_legal = game.is_legal(move)
    move_promote_legal = game.is_legal(move_promote)
    if move_legal and move_promote_legal:
        return jsonify({'valid': True, 'needs_promotion_choice': True})
    elif move_legal:
        game.apply(move)
        return jsonify({'valid': True})
    elif move_promote_legal:
        game.apply(move_promote)
        return jsonify({'valid': True})
    else:
        return jsonify({'valid': False, 'message': 'Illegal move!'})


@app.route('/promote', methods=['POST'])
def promote():
    data = request.json
    dst = Square(data['to'][0] * 9 + data['to'][1])
    src = Square(data['from'][0] * 9 + data['from'][1])
    promote = data.get('promote', False)
    move = Move(dst, src, promote=promote)
    game.apply(move)
    return jsonify({'valid': True})


if __name__ == '__main__':
    app.run()
