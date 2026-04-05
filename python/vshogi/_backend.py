from contextlib import asynccontextmanager

from fastapi import APIRouter, FastAPI

from vshogi.shogi import Game


router = APIRouter()


@router.post("/game")
async def new_game():
    app.state.game = Game()
    return {"success": True}


@asynccontextmanager
async def lifespan(app: FastAPI):
    app.state.game = Game()
    yield


app = FastAPI(title="Shogi API", version="0.1.0", lifespan=lifespan)
app.include_router(router=router, prefix="/api")
