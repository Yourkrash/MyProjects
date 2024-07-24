from card import Card
from typing import List
from deck import Deck
import json

class Player:
    def __init__(self, cards: List[Card], name: str) -> None:
        self.__cards = cards
        self.name = name
        self.ispassed = False

    def GrabCards(self, deck: Deck) -> None:
        if len(self.__cards) < 6:
            self.__cards += deck.GrabCardsTop(6 - len(self.__cards))
    
    def GiveHand(self) -> List[Card]:
        return self.__cards

    def PutCard(self, index: int) -> Card:
        card = self.__cards[index]
        self.__cards.pop(index)
        return card

    def HaveCard(self) -> bool:
        return self.__cards != []
    
    def Grab(self, list: List[Card]) -> None:
        self.__cards += list
    
    def toJSON(self) -> str:
        return json.dumps(self.__cards, default=(lambda x: x.toJSON()))
    





