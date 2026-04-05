from contextlib import asynccontextmanager

from fastapi import APIRouter, FastAPI, HTTPException
from pydantic import BaseModel

from vshogi.shogi import Game


router = APIRouter()


class MoveRequest(BaseModel):
    move: str


def _serialize(game: Game) -> dict:
    return {"sfen": game.to_sfen(), "result": game.result.name.lower()}


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


@asynccontextmanager
async def lifespan(app: FastAPI):
    app.state.game = Game()
    yield


app = FastAPI(title="Shogi API", version="0.1.0", lifespan=lifespan)
app.include_router(router=router, prefix="/api")
