from contextlib import asynccontextmanager

from fastapi import APIRouter, FastAPI, HTTPException
from pydantic import BaseModel

from vshogi.shogi import Game


router = APIRouter()


class MoveRequest(BaseModel):
    move: str


@router.post("/game")
async def new_game():
    app.state.game = Game()
    return {"success": True}


@router.get("/game/sfen")
async def get_sfen(include_move_count: bool = True):
    return {"sfen": app.state.game.to_sfen(include_move_count)}


@router.post("/move")
async def make_move(request: MoveRequest):
    move = request.move
    game: Game = app.state.game
    if not game.is_legal(move):
        raise HTTPException(status_code=400, detail=f"Invalid move ({move})")
    game.apply(move)
    return {"sfen": game.to_sfen()}


@asynccontextmanager
async def lifespan(app: FastAPI):
    app.state.game = Game()
    yield


app = FastAPI(title="Shogi API", version="0.1.0", lifespan=lifespan)
app.include_router(router=router, prefix="/api")
