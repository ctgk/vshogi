import os
from contextlib import asynccontextmanager

from fastapi import APIRouter, FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel

from vshogi.shogi import Game, Result


router = APIRouter()


def _get_allowed_origins() -> list[str]:
    origins = os.environ.get(
        "VSHOGI_ALLOWED_ORIGINS",
        ",".join(
            [
                "http://127.0.0.1:8000",
                "http://localhost:8000",
                "http://127.0.0.1:5173",
                "http://localhost:5173",
            ]
        ),
    )
    return [origin.strip() for origin in origins.split(",") if origin.strip()]


class MoveRequest(BaseModel):
    move: str


def _serialize(game: Game) -> dict:
    return {
        "sfen": game.to_sfen(),
        "result": game.result.name.lower(),
        "move_history": [
            game.get_move_at(i).to_sfen() for i in range(game.ply())
        ],
    }


@router.post("/game")
async def new_game():
    app.state.game = Game()
    return _serialize(app.state.game)


@router.get("/game/state")
async def get_state():
    return _serialize(app.state.game)


@router.post("/move")
async def make_move(request: MoveRequest):
    move = request.move
    game: Game = app.state.game
    if not game.is_legal(move):
        raise HTTPException(status_code=400, detail=f"Invalid move ({move})")
    game.apply(move)
    return _serialize(game)


@router.post("/resign")
async def resign():
    game: Game = app.state.game
    if game.result != Result.ONGOING:
        raise HTTPException(
            status_code=400,
            detail=(
                "The game has already finished with "
                f"{game.result.name.lower()}."
            ),
        )
    game.resign()
    return _serialize(game)


@router.post("/undo")
async def undo_move():
    game: Game = app.state.game
    if game.ply() == 0:
        raise HTTPException(status_code=400, detail="No moves to undo.")
    game.undo()
    return _serialize(game)


@asynccontextmanager
async def lifespan(app: FastAPI):
    app.state.game = Game()
    yield


app = FastAPI(title="Shogi API", version="0.1.0", lifespan=lifespan)
app.add_middleware(
    CORSMiddleware,
    allow_origins=_get_allowed_origins(),
    allow_credentials=False,
    allow_methods=["*"],
    allow_headers=["*"],
)
app.include_router(router=router, prefix="/api")
