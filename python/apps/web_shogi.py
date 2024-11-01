import numpy as np
from flask import Flask, render_template

from vshogi.shogi import Game


app = Flask(__name__)


@app.route('/')
def hello():
    game = Game()
    board = np.asarray(game.board).tolist()
    board = [[p.to_sfen() for p in row] for row in board]
    return render_template('shogi_board.html', board=board)


if __name__ == '__main__':
    app.run()
