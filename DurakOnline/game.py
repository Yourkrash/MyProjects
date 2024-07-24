from deck import Deck
from player import Player
from board import Board
from config import HAND_SIZE, BASE_CARD
from typing import List
import asyncio

class Game:
    def __init__(self) -> None:
        self.deck = Deck(BASE_CARD)
        self.board = Board(self.deck.TrumpSuit())
        self.player_dict = {}
        self.winlist = []
        self.count_player = 0
        self.current_player = -1
        self.current_card = -1
    
    def GetListPasses(self) -> List[bool]:
        return [plr.ispassed for plr in self.player_dict]
    
    def AddPlayer(self, name: str) -> int:
        self.player_dict.update({self.count_player : Player(self.deck.GrabCardsTop(HAND_SIZE), name)})
        self.count_player += 1
        return self.count_player - 1
    
    def PlayerThrowCard(self, id: int, ind_card: int) -> str:
        self.current_player = id
        self.current_card = ind_card
        print(ind_card)
        return 'Твоя карта принята'      
    
    def ResetLastCard(self) -> None:
        self.current_card = -1

    def PlayerPass(self, id: int) -> str:
        if not self.player_dict[id].ispassed:
            self.player_dict[id].ispassed = True
            return 'Ты посанул'
        else:
            return 'Ты уже посавал'


    async def Attack(self, attack_id: int) -> None:
        while self.current_player != attack_id or self.player_dict[attack_id].ispassed or not self.board.CanAttack(self.player_dict[attack_id].GiveHand()[self.current_card]):
            if self.player_dict[attack_id].ispassed:
                return
            await asyncio.sleep(0.1)
        else:
            self.board.AttackCard(self.player_dict[attack_id].PutCard(self.current_card))
        print("attack ended")

    async def Defence(self, attack_id: int) -> None:
        while self.current_player != attack_id or self.player_dict[attack_id].ispassed or not self.board.CanDefence(self.player_dict[attack_id].GiveHand()[self.current_card]):
            if self.player_dict[attack_id].ispassed:
                return
            await asyncio.sleep(0.1)
        else:
            self.board.DefenceCard(self.player_dict[attack_id].PutCard(self.current_card))
        print("defence ended")
                     
    
    async def Round(self, attack_id: int, defence_id: int) -> bool:
        is_lose = False
        attack_counter = 0
        while attack_counter < 6 and not self.player_dict[defence_id].ispassed and not self.player_dict[attack_id].ispassed:
            await self.AttackAndDefence(attack_id, defence_id)
            attack_counter += 1
        if not self.board.Beated():
            self.player_dict[defence_id].Grab(self.board.GetDefenceCard())
            self.player_dict[defence_id].Grab(self.board.GetAttackCard())
            is_lose = True
        self.board.ResetBoard()
        for player in self.player_dict:
            player.ispassed = False

    async def AttackAndDefence(self, attack_id: int, defence_id: int) -> None:
        print("run attack")
        await self.Attack(attack_id)
        print("run defence")
        if not self.player_dict[attack_id].ispassed:
           await self.Defence(defence_id)
    
    def PushWinList(self) -> None:
        for player in self.player_dict.values():
            if not player.HaveCard():
                self.winlist.append(player)
                self.player_dict.erase(player)


    async def StartGame(self) -> None:
        ind = 0
        while len(self.player_dict) > 1:
            await self.Round(ind % len(self.player_dict), ind + 1 % len(self.player_dict))
            self.PushWinList()
            ind += 1
        
        