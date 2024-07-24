from typing import Self

class Card:
    '''
    в suit хранится символ:
    H - чирвы
    S - пики
    D - бубны
    T - трефы
    в value значение карты от 2(6) до 14(туз)
    '''
    def StrCast(self, string: str) -> Self:
        self.__suit = string[0]
        self.__value = int(string[1:])
        return self

    
    def __init__(self, suit : str, value: int) -> None:
        self.__suit = suit
        self.__value = value

    def GetSuit(self) -> str:
        return self.__suit
    
    def GetValue(self) -> int:
        return self.__value
    
    def SetValue(self, value: int) -> None:
        self.__value = value
    
    def SetSuit(self, suit: str) -> None:
        self.__suit = suit

    def __lt__(self, other : Self) -> bool:
        if self.__suit == other.GetSuit():
            return self.__value < other.__value
        else:
            return False

    def __str__(self) -> str:
        return f'{self.__suit}{self.__value}'
    
    def toJSON(self) -> str:
        return str(self)
    
    def __repr__(self):
        return str(self)