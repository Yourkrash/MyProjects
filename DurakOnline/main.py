from game import Game
from fastapi import FastAPI
from typing import List
from card import Card
import json
import asyncio


def toJSON(x : List[Card]) -> str:
    return str(x)

app = FastAPI()
game = Game()

@app.get("/registration/{name}")
async def root(name : str):
    global game
    return game.AddPlayer(name)

@app.get("/throwcard/{name}/{id}/{card_ind}")
async def root(name : str, id: int, card_ind: int):
    global game
    return game.PlayerThrowCard(id, card_ind)

@app.get("/pass/{name}/{id}")
async def root(name : str, id: int):
    global game
    return game.PlayerPass(id)

@app.get("/gamestate/board/attack")
async def root():
    global game
    return json.dumps(game.board.GetAttackCard(), default=toJSON)

@app.get("/gamestate/board/passes")
async def root():
    global game
    return json.dumps(game.board.GetListPasses(), default=toJSON)

@app.get("/gamestate/board/defence")
async def root():
    global game
    return json.dumps(game.board.GetDefenceCard(), default=toJSON)

@app.get("/gamestate/board/trump_suit")
async def root():
    global game
    return json.dumps(game.board.trump_suit)

@app.get("/gamestate/hand/{name}/{id}")
async def root(id: int, name: str):
    global game
    return json.dumps(game.player_dict[id].GiveHand(), default=toJSON)

async def main() -> None:
    global game
    print("main started")
    while game.count_player < 2:
        print(game.count_player)
        await asyncio.sleep(0.1)
    await game.StartGame()
    

@app.on_event("startup")
async def startup_event():
    asyncio.create_task(main())