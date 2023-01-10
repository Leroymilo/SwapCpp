import json

class Door :
    def __init__(self, data) -> None :
        self.pos = (data["X"], data["Y"])
        self.tiles = [(tile["X"], tile["Y"]) for tile in data["tiles"]]

class Level :
    def __init__(self, dir_) -> None :
        self.dir = dir_
        data = json.load(open(dir_))

        self.size = (data["bg"]["W"], data["bg"]["W"])
        self.grid = data["bg"]["BG"]
        self.goal = (data["bg"]["EndX"], data["bg"]["EndY"])

        self.player = data["entities"]["player"]
        self.bullet = data["entities"]["bullet"]
        self.boxes = [(box["X"], box["Y"]) for box in data["entities"]["Boxes"]]

        activators = data["logic"]["activators"]
        self.buttons = [(act["X"], act["Y"]) for act in activators if act["type"] == "I"]
        self.targets = [(act["X"], act["Y"]) for act in activators if act["type"] == "T"]

        gates = data["logic"]["gates"]
        self.ands = [(gate["X"], gate["Y"]) for gate in gates if gate["type"] == "&"]
        self.ors = [(gate["X"], gate["Y"]) for gate in gates if gate["type"] == "|"]
        self.nos = [(gate["X"], gate["Y"]) for gate in gates if gate["type"] == "!"]

        self.doors = [Door(door_data) for door_data in data["logic"]["doors"]]

        self.links = data["logic"]["links"]

    def get_surface(self, delta: int) :
        return (self.size[0] * delta, self.size[1] * delta)
    
    def get_tile(self, x: int, y: int) :
        pass