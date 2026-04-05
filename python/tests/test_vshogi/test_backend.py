import pytest
from fastapi.testclient import TestClient

from vshogi.shogi import Game, Color
from vshogi._backend import app


@pytest.fixture
def client():
    with TestClient(app) as c:
        yield c


def test_new_game_returns_success(client):
    response = client.post("/api/game", json={})
    assert response.status_code == 200
    assert response.json() == {
        "sfen": (
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1"
        ),
        "result": "ongoing",
        "move_history": [],
    }


def test_new_game_sets_game_state(client):
    client.post("/api/game", json={})
    assert app.state.game is not None


def test_new_game_resets_game_state(client):
    from vshogi.shogi import Game

    first_game = app.state.game
    client.post("/api/game", json={})
    assert app.state.game is not first_game
    assert isinstance(app.state.game, Game)


def test_get_state(client):
    response = client.get("/api/game/state")
    assert response.status_code == 200
    assert response.json() == {
        "sfen": (
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1"
        ),
        "result": "ongoing",
        "move_history": [],
    }


def test_make_move_legal(client):
    response = client.post("/api/move", json={"move": "2g2f"})
    assert response.status_code == 200
    assert response.json() == {
        "sfen": (
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/7P1/PPPPPPP1P/1B5R1/LNSGKGSNL w - 2"
        ),
        "result": "ongoing",
        "move_history": ["2g2f"],
    }
    game: Game = app.state.game
    assert game.turn == Color.WHITE
    assert game.ply() == 1
    assert game.get_move_at(0).to_sfen() == "2g2f"


def test_make_move_illegal(client):
    response = client.post("/api/move", json={"move": "8c8d"})
    assert response.status_code == 400
    assert response.json() == {"detail": "Invalid move (8c8d)"}
    game: Game = app.state.game
    assert game.turn == Color.BLACK
    assert game.ply() == 0


def test_resign(client):
    response = client.post("/api/resign")
    assert response.status_code == 200
    assert response.json() == {
        "sfen": (
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1"
        ),
        "result": "white_win",
        "move_history": [],
    }

    response = client.post("/api/resign")
    assert response.status_code == 400
    assert response.json() == {
        "detail": "The game has already finished with white_win.",
    }


def test_undo(client):
    response = client.post("/api/undo")
    assert response.status_code == 400
    assert response.json() == {"detail": "No moves to undo."}
    assert app.state.game.ply() == 0

    client.post("/api/move", json={"move": "2g2f"})
    assert app.state.game.ply() == 1
    response = client.post("/api/undo")
    assert response.status_code == 200
    assert response.json() == {
        "sfen": (
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1"
        ),
        "result": "ongoing",
        "move_history": [],
    }
    assert app.state.game.ply() == 0


if __name__ == "__main__":
    pytest.main([__file__])
