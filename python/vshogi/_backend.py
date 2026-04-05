import asyncio
import os
import typing as tp
from contextlib import asynccontextmanager

from fastapi import APIRouter, FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel

from vshogi.engine import AlphaZero
from vshogi.shogi import Game, Result
from vshogi.dlshogi import PolicyValueFunction


router = APIRouter()
engines = {"black": None, "white": None}  # None means that the player is human
engine_budgets = {"black": 800, "white": 800}


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


class EngineRequest(BaseModel):
    color: tp.Literal["black", "white"]
    tflite_path: str | None  # None means that the player is human
    num_simulations: int = 800
    coeff_puct: float
    kldgain_threshold: float | None
    random_rate: float
    dfpn_search_root: int
    dfpn_search_leaf: int


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


@router.get("/engine")
async def get_engines():
    return {
        "black": engines["black"] is not None,
        "white": engines["white"] is not None,
    }


@router.post("/engine")
async def load_engine(req: EngineRequest):
    if req.tflite_path is None:
        engines[req.color] = None
    else:
        engines[req.color] = AlphaZero(
            PolicyValueFunction(req.tflite_path),
            coeff_puct=req.coeff_puct,
            random_rate=req.random_rate,
            kldgain_threshold=req.kldgain_threshold,
            dfpn_search_root=req.dfpn_search_root,
            dfpn_search_leaf=req.dfpn_search_leaf,
        )
    engine_budgets[req.color] = req.num_simulations


@router.post("/engine/move")
async def engine_move():
    game: Game = app.state.game
    if game.result != Result.ONGOING:
        raise HTTPException(status_code=400, detail="Game is not ongoing.")
    color = game.turn.name.lower()
    engine = engines[color]
    if engine is None:
        raise HTTPException(
            status_code=400,
            detail=f"No engine is configured for {color}.",
        )
    budget = engine_budgets[color]

    def _run() -> object:
        engine.set_game(game)
        engine.search(budget)
        return engine.select()

    move = await asyncio.to_thread(_run)
    game.apply(move)
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
