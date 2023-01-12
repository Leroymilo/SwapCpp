import json

import pygame as pg

from src.python.sprites import door_hub, door_tile

class Door :
    def __init__(self, data) -> None :
        self.pos = (data["X"], data["Y"])
        self.tiles = {(tile["X"], tile["Y"]) for tile in data["tiles"]}
    
    def draw_lines(self, surf: pg.Surface, delta: int) -> None :
        hx, hy = self.pos
        for tx, ty in self.tiles :
            sx, sy, ex, ey = delta * (hx+0.5), delta * (hy+0.5), delta * (tx+0.5), delta * (ty+0.5)
            pg.draw.line(surf, (200, 0, 0), (sx, sy), (ex, ey), delta//16)

    def draw_tiles(self, surf: pg.Surface, delta: int) -> None :
        for x, y in self.tiles :
            surf.blit(door_tile, (delta * x, delta * y))

    def draw_hub(self, surf: pg.Surface, delta: int) -> None :
        surf.blit(door_hub, (delta * self.pos[0], delta * self.pos[1]))

class Link :
    def __init__(self, data) -> None:
        self.nodes = [(node["X"], node["Y"]) for node in data["nodes"]]
        self.offsets = data["offsets"]
    
    def get_offset(self, i) :
        if i < 0 or i >= len(self.offsets) :
            return 0
        return self.offsets[i]
    
    def draw(self, surf: pg.Surface, delta: int) :
        width = delta//16

        for i in range(len(self.nodes) - 1) :
            x1, y1 = self.nodes[i]
            x2, y2 = self.nodes[i+1]
            x1, y1, x2, y2 = delta * (x1+0.5) - 1, delta * (y1+0.5) - 1, delta * (x2+0.5) - 1, delta * (y2+0.5) - 1

            if x1 == x2 :
                C1 = (x1 + width * self.get_offset(i), y1 + width * self.get_offset(i-1))
                C2 = (x2 + width * self.get_offset(i), y2 + width * self.get_offset(i+1))
            elif y1 == y2 :
                C1 = (x1 + width * self.get_offset(i-1), y1 + width * self.get_offset(i))
                C2 = (x2 + width * self.get_offset(i+1), y2 + width * self.get_offset(i))
            else :
                C1 = (x1, y1)
                C2 = (x2, y2)

            pg.draw.line(surf, (61, 176, 254), C1, C2, width)


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

        self.links = [Link(link_data) for link_data in data["logic"]["links"]]

    def get_surface(self, delta: int) :
        return (self.size[0] * delta, self.size[1] * delta)
    
    def get_tile(self, x: int, y: int) :
        return self.grid[y][x]
    
    def set_tile(self, x: int, y: int, char: str) :
        self.grid[y][x] = char
    
    def erase_any(self, x: int, y: int) :
        # Removes anything standing on (x, y) to place a wall or a grate
        
        if (self.player["X"], self.player["Y"]) == (x, y) :
            self.player["alive"] = False
            self.player["X"], self.player["Y"] = 0, 0
        
        if (self.bullet["X"], self.bullet["Y"]) == (x, y) :
            self.bullet["alive"] = False
            self.bullet["X"], self.bullet["Y"] = 0, 0
        
        self.boxes.discard((x, y))
        self.buttons.discard((x, y))
        self.targets.discard((x, y))
        for L in self.gates.values() :
            L.discard((x, y))
        
        i = 0
        while i < len(self.doors) :
            if (x, y) == self.doors[i].pos :
                self.doors.pop(i)
                continue
            self.doors[i].tiles.discard((x, y))
            i += 1
    
    def can_place(self, x: int, y: int, type_: str) :

        # Walls and goal check :

        if self.get_tile(x, y) in {'X', 'x'} :
            return False
        
        if (x, y) == self.goal :
            return False

        # Building sets of blocked coordinates :

        entities = self.boxes.copy()
        if self.player["alive"] :
            entities.add((self.player["X"], self.player["Y"]))
        if self.bullet["alive"] :
            entities.add((self.bullet["X"], self.bullet["Y"]))

        logics = self.buttons | self.targets
        for L in self.gates.values() :
            logics |= L

        door_tiles = set()
        door_hubs = set()
        for door in self.doors :
            door_hubs.add(door.pos)
            door_tiles |= door.tiles
        
        # Specific checks :

        if type_ in {"player", "bullet", "box"} :
            if (x, y) in (door_tiles | entities | self.targets) :
                return False
            return True
        
        if type_ in {"button", "target", "gate"} :
            if (x, y) in (door_tiles | door_hubs | logics) :
                return False
            return True
            
        if type_ == "door_tile" :
            if (x, y) in (door_tiles | logics) :
                return False
            return True
        
        if type_ == "door_hub" :
            return ((x, y) not in logics)

        return True

    def add_door_hub(self, x: int, y: int) :
        if (x, y) not in {door.pos for door in self.doors} :
            self.doors.append(Door({"X": x, "Y": y, "tiles": []}))
    
    def connect_door(self, tile_pos: tuple[int], hub_pos: tuple[int]) :
        for door in self.doors :
            if door.pos == hub_pos :
                door.tiles.add(tile_pos)
                return
        
        hx, hy = hub_pos
        tx, ty = tile_pos
        self.doors.append(Door({"X": hx, "Y": hy, "tiles": [{"X": tx, "Y": ty}]}))
    
    def remove_door_tile(self, x: int, y: int) :
        for door in self.doors :
            if (x, y) in door.tiles :
                door.tiles.remove((x, y))
    
    def remove_door_hub(self, x: int, y: int) :
        for i in range(len(self.doors)) :
            if self.doors[i].pos == (x, y) :
                self.doors.pop(i)
                return