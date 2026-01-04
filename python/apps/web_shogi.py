import logging
import os
import tempfile

import numpy as np
from flask import Flask, jsonify, render_template, request

import vshogi
from vshogi.dlshogi import PolicyValueFunction
from vshogi.shogi import Color, Game, Move


app = Flask(__name__)
logging.basicConfig(level=logging.DEBUG)
game: Game = Game()

alphazero_player = vshogi.engine.AlphaZero(
    kldgain_threshold=1e-4,
    dfpn_search_root=10000,
    dfpn_search_leaf=100,
)
logging.info("AlphaZero player initialized")
alphazero_model_path: str | None = None


def load_policy_value_function(model_path: str) -> None:
    global alphazero_model_path
    if not os.path.isfile(model_path):
        raise FileNotFoundError(f"Model file not found: {model_path}")
    alphazero_player._policy_value_func = PolicyValueFunction(model_path)
    alphazero_player.clear()
    alphazero_model_path = model_path
    logging.info("Loaded AlphaZero network weight from %s", model_path)


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
        model_path=alphazero_model_path,
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

        # Make AlphaZero move if game is still ongoing
        if game.result == vshogi.Result.ONGOING:
            make_alphazero_move()

        return jsonify({'legal': True, 'ask': False, 'src': src, 'dst': dst})
    elif promotion_is_legal:
        game.apply(move_promotion)
        logging.debug(f'make_move.game after game.apply(): {game.to_sfen()}')

        # Make AlphaZero move if game is still ongoing
        if game.result == vshogi.Result.ONGOING:
            make_alphazero_move()

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

        # Make AlphaZero move if game is still ongoing
        if game.result == vshogi.Result.ONGOING:
            make_alphazero_move()

        return jsonify({'success': True})
    else:
        logging.debug(f'make_move: {move} is illegal at {game.to_sfen()}')
        return jsonify({'success': False})


@app.route('/load_model', methods=['POST'])
def load_model():
    data = request.get_json() or {}
    model_path = data.get('model_path')
    if not model_path:
        return (
            jsonify({'success': False, 'error': 'Model path is required'}),
            400,
        )

    try:
        load_policy_value_function(model_path)
    except FileNotFoundError:
        return (
            jsonify({'success': False, 'error': 'Model file not found'}),
            400,
        )
    except Exception as exc:  # pylint: disable=broad-except
        logging.exception('Failed to load model from %s', model_path)
        return jsonify({'success': False, 'error': str(exc)}), 500

    return jsonify({'success': True, 'model_path': model_path})


@app.route('/upload_model', methods=['POST'])
def upload_model():
    file_storage = request.files.get('model')
    if file_storage is None or file_storage.filename == '':
        return jsonify({'success': False, 'error': 'No file provided'}), 400

    _, ext = os.path.splitext(file_storage.filename)
    if ext.lower() != '.tflite':
        return (
            jsonify({
                'success': False,
                'error': 'Only .tflite files are supported',
            }),
            400,
        )

    try:
        with tempfile.NamedTemporaryFile(
            delete=False,
            suffix=ext,
            prefix='uploaded_model_',
        ) as tmp:
            file_storage.save(tmp.name)
            temp_path = tmp.name

        load_policy_value_function(temp_path)
        return jsonify({'success': True, 'model_path': file_storage.filename})
    except FileNotFoundError:
        return jsonify({'success': False, 'error': 'Failed to save file'}), 500
    except Exception as exc:  # pylint: disable=broad-except
        logging.exception('Failed to upload model')
        return jsonify({'success': False, 'error': str(exc)}), 500


def make_alphazero_move():
    """Make AlphaZero play its move."""
    if game.result != vshogi.Result.ONGOING:
        return

    logging.info(f"AlphaZero is thinking... : {game.to_sfen()}")
    alphazero_player.set_game(game)
    alphazero_player.search(n_or_t=1000)
    az_move = alphazero_player.select()

    if az_move and game.is_legal(az_move):
        game.apply(az_move)
        logging.info(f"AlphaZero played: {az_move.to_sfen()}")
    else:
        logging.error(f"AlphaZero selected invalid move: {az_move}")

    alphazero_player.clear()


if __name__ == '__main__':
    app.run()
