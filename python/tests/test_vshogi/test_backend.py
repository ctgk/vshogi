import pytest
from fastapi.testclient import TestClient

from vshogi._backend import app


@pytest.fixture
def client():
    with TestClient(app) as c:
        yield c


def test_new_game_returns_success(client):
    response = client.post("/api/game", json={})
    assert response.status_code == 200
    assert response.json() == {"success": True}


def test_new_game_sets_game_state(client):
    client.post("/api/game", json={})
    assert app.state.game is not None


def test_new_game_resets_game_state(client):
    from vshogi.shogi import Game

    first_game = app.state.game
    client.post("/api/game", json={})
    assert app.state.game is not first_game
    assert isinstance(app.state.game, Game)


def test_get_sfen(client):
    response = client.get("/api/game/sfen")
    assert response.status_code == 200
    assert response.json() == {
        "sfen": (
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b - 1"
        ),
    }

    response = client.get(
        "/api/game/sfen",
        params={"include_move_count": False},
    )
    assert response.status_code == 200
    assert response.json() == {
        "sfen": (
            "lnsgkgsnl/1r5b1/ppppppppp/9/9/9/PPPPPPPPP/1B5R1/LNSGKGSNL b -"
        ),
    }


if __name__ == "__main__":
    pytest.main([__file__])
