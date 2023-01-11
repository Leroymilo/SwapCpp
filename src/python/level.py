import json

class Door :
    def __init__(self, data) -> None :
        self.pos = (data["X"], data["Y"])
        self.tiles = {(tile["X"], tile["Y"]) for tile in data["tiles"]}

class Level :
    def __init__(self, dir_) -> None :
        self.dir = dir_
        data = json.load(open(dir_))

        self.size = (data["bg"]["W"], data["bg"]["H"])
        self.grid = data["bg"]["BG"]
        self.goal = (data["bg"]["EndX"], data["bg"]["EndY"])

        self.player = data["entities"]["player"]
        self.bullet = data["entities"]["bullet"]
        self.boxes = {(box["X"], box["Y"]) for box in data["entities"]["Boxes"]}

        activators = data["logic"]["activators"]
        self.buttons = {(act["X"], act["Y"]) for act in activators if act["type"] == "I"}
        self.targets = {(act["X"], act["Y"]) for act in activators if act["type"] == "T"}

        gates = data["logic"]["gates"]
        self.ands = {(gate["X"], gate["Y"]) for gate in gates if gate["type"] == "&"}
        self.ors = {(gate["X"], gate["Y"]) for gate in gates if gate["type"] == "|"}
        self.nos = {(gate["X"], gate["Y"]) for gate in gates if gate["type"] == "!"}
        self.gates = {"AND": self.ands, "OR": self.ors, "NO": self.nos}

        self.doors = [Door(door_data) for door_data in data["logic"]["doors"]]

        self.links = data["logic"]["links"]

    def get_surface(self, delta: int) :
        return (self.size[0] * delta, self.size[1] * delta)
    
    def get_tile(self, x: int, y: int) :
        return self.grid[y][x]
    
    def set_tile(self, x: int, y: int, char: str) :
        self.grid[y][x] = char
    
    def place_wall(self, x: int, y: int) :
        # Removes anything standing on (x, y) to place a wall or a grate
        
        if (self.player["X"], self.player["Y"]) == (x, y) :
            self.player["alive"] = False
            self.player["X"], self.player["Y"] = 0, 0
        
        if (self.bullet["X"], self.bullet["Y"]) == (x, y) :
            self.bullet["alive"] = False
            self.bullet["X"], self.bullet["Y"] = 0, 0
        
        self.boxes.difference_update({(x, y)})
        self.buttons.difference_update({(x, y)})
        self.targets.difference_update({(x, y)})
        for L in self.gates.values() :
            L.difference_update({(x, y)})
        
        for door in self.doors :
            if (x, y) == door.pos :
                self.doors.remove(door)
                break

            door.tiles.difference_update((x, y))
    
    def can_place(self, x: int, y: int, type_: str) :

        # Walls and goal check :

        if self.get_tile(x, y) in {'X', 'x'} :
            return False
        
        if (x, y) == self.goal :
            return False

        # Building sets of blocked coordinates :

        entities = self.boxes
        if self.player["alive"] :
            entities.add((self.player["X"], self.player["Y"]))
        if self.bullet["alive"] :
            entities.add((self.bullet["X"], self.bullet["Y"]))

        logics = self.buttons | self.targets
        for L in self.gates.values() :
            logics |= L

        door_tiles = set()
        for door in self.doors :
            logics.add(door.pos)
            door_tiles |= door.tiles
        
        # Specific checks :

        if type_ in {"player", "bullet", "box"} :
            if (x, y) in (door_tiles | entities | self.targets) :
                return False
            return True
        
        if type_ in {"button", "target", "gate", "door"} :
            if (x, y) in (door_tiles | logics) :
                return False

        return True
    
    def remove_door_tile(self, x: int, y: int) :
        for door in self.doors :
            if (x, y) in door.tiles :
                door.tiles.remove((x, y))