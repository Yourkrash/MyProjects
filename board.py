from card import Card
from deck import CompareCardWithTrump
from typing import List
import json

class Board:
    def __init__(self, trump_suit: str) -> None:
        self.__card_attack = []
        self.__card_defence = []
        self.trump_suit = trump_suit

    def CanAttack(self, card: Card) -> bool:
        return len(self.__card_attack) < 7 and ((card.GetValue() in [crd.GetValue() for crd in self.__card_defence]) or (self.__card_attack == []) or (card.GetValue() == self.__card_attack[0].GetValue()))
    
    def AttackCard(self, card: Card) -> None:
        if len(self.__card_attack) < 7 and ((card.GetValue() in [crd.GetValue() for crd in self.__card_defence]) or (self.__card_attack == [])):
            self.__card_attack.append(card)
        
    def CanDefence(self, card: Card) -> bool:
        return CompareCardWithTrump(self.__card_attack[-1], card, self.trump_suit)
        
    def DefenceCard(self, card: Card) -> None:
        if CompareCardWithTrump(self.__card_attack[-1], card, self.trump_suit):
            self.__card_defence.append(card)
    
    def GetAttackCard(self) -> List[Card]:
        return self.__card_attack
    
    def GetDefenceCard(self) -> List[Card]:
        return self.__card_defence

    def ResetBoard(self) -> None:
        self.__card_attack.clear()
        self.__card_defence.clear()
    
    def Beated(self) -> None:
        return len(self.__card_attack) == len(self.__card_defence)

    def toJSON(self) -> str:
        return json.dumps({"card_attack" : self.__card_attack, "card_defence" : self.__card_defence, "trump_suit" : self.trump_suit}, default=(lambda x: x.toJSON()))
