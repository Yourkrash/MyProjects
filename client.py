from card import Card
import requests
from config import IP

class Client:
    def __init__(self, name: str) -> None:
        resp = requests.get('http://' + IP + '/registration/' + name)
        self.name = name
        self.unique_id = resp.json()
        self.list_attack = []
        self.list_defence = []
        self.hand = []
        self.passed_list = []
        self.trump_suit = ''

    def ThrowCard(self, ind_card: int) -> None:
        repr = requests.get('http://' + IP + '/throwcard/' + self.name + '/' + str(self.unique_id) + '/' + str(ind_card))
        print(repr.json())
    
    def Pass(self) -> None:
        repr = requests.get('http://' + IP + '/pass/' + self.name + '/' + str(self.unique_id))
        print(repr.json())

    def FetchPasses(self) -> None:
        repr = requests.get('http://' + IP + '/gamestate/passes/' + self.name + '/' + str(self.unique_id))
        self.hand = [Card('A', 0).StrCast(val[1:-1])for val in repr.json()[1:-1].split(sep=', ') if val != '']
        print(self.hand)
        
    def FetchHand(self) -> None:
        repr = requests.get('http://' + IP + '/gamestate/hand/' + str(self.name) + '/' + str(self.unique_id))
        self.hand = [Card('A', 0).StrCast(val[1:-1])for val in repr.json()[1:-1].split(sep=', ') if val != '']
        print(self.hand)
    
    def FetchAttack(self) -> None:
        repr = requests.get('http://' + IP + '/gamestate/board/attack')
        self.list_attack = [Card('A', 0).StrCast(val[1:-1]) for val in repr.json()[1:-1].split(sep=', ') if val != '']
        print(self.list_attack)

    def FetchDefence(self) -> None:
        repr = requests.get('http://' + IP + '/gamestate/board/defence')
        self.list_defence = [Card('A', 0).StrCast(val[1:-1]) for val in repr.json()[1:-1].split(sep=', ') if val != '']
        print(self.list_defence)
    
    def FetchTrump(self) -> None:
        repr = requests.get('http://' + IP + '/gamestate/board/trump_suit')
        self.trump_suit = repr.json()
        print(self.trump_suit[1:-1])

    def StartGame(self) -> None:
        repr = requests.get('http://' + IP + '/gamestart')
        self.trump_suit = repr.json()
        print(self.trump_suit[1:-1])