import json
from card import Card
from typing import List
import random

def CompareCardWithTrump(card1: Card, card2: Card, trump_suit: str) -> bool:
        if card1.GetSuit() == card2.GetSuit():
            return card1 < card2
        elif card1.GetSuit() == trump_suit:
            return False
        elif card2.GetSuit() == trump_suit:
            return True
        else:
            return card1 < card2

class Deck:
    def __init__(self, base_card: int) -> None:
        self.__list_cards = [Card(suit, value) for value in range(base_card, 15) for suit in ['H', 'D', 'S', 'T']]
        random.shuffle(self.__list_cards)

    def Reshuffle(self) -> None:
        random.shuffle(self.__list_cards)

    def GetDeckList(self) -> List[Card]:
        return self.__list_cards
    
    def Size(self) -> int:
        return len(self.__list_cards)
    
    def GrabCardsTop(self, count_cards: int) -> List[Card]:
        top, self.__list_cards = self.__list_cards[:count_cards], self.__list_cards[count_cards:]
        return top

    def GrabOneCard(self) -> Card:
        return self.GrabCardsTop(1)
    
    def TrumpSuit(self) -> str:
        return self.__list_cards[-1].GetSuit()
    
    def LastCard(self) -> Card:
        return self.__list_cards[-1]
    
