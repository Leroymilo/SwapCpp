from __future__ import annotations
from typing import Any

import json

import pygame as pg

from src.python.sprites import door_hub, door_tile, gates

dirs = ["U", "R", "D", "L"]
orients = ["N", "H", "V"]

class Gate :
    def __init__(self, **kwargs) -> None:
        self.pos = (kwargs["X"], kwargs["Y"])
        self.type = kwargs["type"]
        self.dir = kwargs["dir"]

    def to_dict(self) -> dict :
        return {
            "X": self.pos[0], "Y": self.pos[1],
            "type": self.type,
            "dir": self.dir
        }
    
    def draw(self, surf: pg.Surface, delta: int) -> None :
        surf.blit(gates[self.type][self.dir], (delta * self.pos[0], delta * self.pos[1]))


class Door :
    def __init__(self, **kwargs) -> None :
        self.pos = (kwargs["X"], kwargs["Y"])
        self.tiles = {(tile["X"], tile["Y"]): tile["orient"] for tile in kwargs["tiles"]}
    
    def to_dict(self) -> dict :
        return {
            "X": self.pos[0],
            "Y": self.pos[1],
            "nb_tiles": len(self.tiles),
            "tiles": [
                {
                    "X": x,
                    "Y": y,
                    "orient": orient
                }
                for (x, y), orient in self.tiles.items()
            ]
        }
    
    def draw_lines(self, surf: pg.Surface, delta: int) -> None :
        hx, hy = self.pos
        for tx, ty in self.tiles.keys() :
            sx, sy, ex, ey = delta * (hx+0.5), delta * (hy+0.5), delta * (tx+0.5), delta * (ty+0.5)
            pg.draw.line(surf, (200, 0, 0), (sx, sy), (ex, ey), delta//16)

    def draw_tiles(self, surf: pg.Surface, delta: int) -> None :
        for (x, y), orient in self.tiles.items() :
            surf.blit(door_tile[orient], (delta * x, delta * y))

    def draw_hub(self, surf: pg.Surface, delta: int) -> None :
        surf.blit(door_hub, (delta * self.pos[0], delta * self.pos[1]))


class Link :
    def __init__(self, **kwargs) -> None :
        self.nodes = [(node["X"], node["Y"]) for node in kwargs["nodes"]]
        self.offsets = kwargs["offsets"]
        self.type_start = kwargs["type_start"]
        self.type_end = kwargs["type_end"]
    
    def to_dict(self) -> dict :
        return {
            "type_start": self.type_start,
            "type_end": self.type_end,
            "nb_nodes": len(self.nodes),
            "nodes": [{"X": node[0], "Y": node[1]} for node in self.nodes],
            "offsets": self.offsets
        }
    
    def get_id(self) :
        return f"{self.nodes[0]} -> {self.nodes[-1]}"
    
    def get_offset(self, i) :
        if i < 0 or i >= len(self.offsets) :
            return -1
        return self.offsets[i] - 1
    
    def __eq__(self, __o: Link) -> bool:
        return self.get_id() == __o.get_id()
    
    def draw(self, surf: pg.Surface, delta: int, seg: int = -1) :
        thick = delta//8

        colors = [(61, 176, 254), (255, 127, 39)]

        for i in range(len(self.nodes) - 1) :
            x1, y1 = self.nodes[i]
            x2, y2 = self.nodes[i+1]

            color = colors[i == seg]

            if x1 == x2 :
                X = delta * x1 + thick * (4.5 + self.get_offset(i))
                W = thick

                if y1 < y2 :
                    Y = delta * y1 + thick * (4.5 + self.get_offset(i-1))
                    H = delta * (y2 - y1) + thick * (1 - self.get_offset(i-1) + self.get_offset(i+1))
                else :
                    Y = delta * y2 + thick * (4.5 + self.get_offset(i+1))
                    H = delta * (y1 - y2) + thick * (1 - self.get_offset(i+1) + self.get_offset(i-1))

            elif y1 == y2 :
                Y = delta * y1 + thick * (4.5 + self.get_offset(i))
                H = thick

                if x1 < x2 :
                    X = delta * x1 + thick * (4.5 + self.get_offset(i-1))
                    W = delta * (x2 - x1) + thick * (1 - self.get_offset(i-1) + self.get_offset(i+1))
                else :
                    X = delta * x2 + thick * (4.5 + self.get_offset(i+1))
                    W = delta * (x1 - x2) + thick * (1 - self.get_offset(i+1) + self.get_offset(i-1))
            else :
                C1 = (delta * (x1 + 0.5), delta * (y1 + 0.5))
                C2 = (delta * (x2 + 0.5), delta * (y2 + 0.5))
                pg.draw.line(surface=surf, color=color, start_pos=C1, end_pos=C2, width=thick)

            pg.draw.rect(surface=surf, color=color, rect=pg.Rect(X, Y, W, H))


# Level representation class ===============================================================================================================================
class Level :
    def __init__(self, source: str | dict[str, Any]) -> None :
        if source.endswith(".json") :
            data = json.load(open(source))
        elif type(source) == str :
            data = json.loads(source)
        elif type(source) == dict :
            data = source
        else :
            raise TypeError("source should be json dict, json dump or json file path")

        self.size = (data["bg"]["W"], data["bg"]["H"])
        self.grid = data["bg"]["BG"]
        self.goal = (data["bg"]["EndX"], data["bg"]["EndY"])

        self.player = data["entities"]["player"]
        self.bullet = data["entities"]["bullet"]
        self.boxes = {(box["X"], box["Y"]) for box in data["entities"]["Boxes"]}

        activators = data["logic"]["activators"]
        self.buttons = {(act["X"], act["Y"]) for act in activators if act["type"] == "I"}
        self.targets = {(act["X"], act["Y"]) for act in activators if act["type"] == "T"}

        self.gates = [Gate(**gate_data) for gate_data in data["logic"]["gates"]]
        self.gates = {gate.pos: gate for gate in self.gates}

        self.doors = [Door(**door_data) for door_data in data["logic"]["doors"]]
        self.doors = {door.pos: door for door in self.doors}

        self.links = [Link(**link_data) for link_data in data["logic"]["links"]]
        self.links = {link.get_id(): link for link in self.links}

        if "text" in data.keys() :
            self.text = data["text"]
        else :
            self.text = []
        self.name = data["name"]
        self.perf_steps = data["perf_steps"]
        self.flags = data["flags"]
    
    def to_dict(self) :
        return {
            "bg": {"H": self.size[1], "W": self.size[0], "EndX": self.goal[0], "EndY": self.goal[1], "BG": self.grid},

            "entities": {
                "player": self.player, "bullet": self.bullet,
                "nbBoxes": len(self.boxes), "Boxes": [{"X": box[0], "Y": box[1]} for box in self.boxes]
            },

            "logic": {
                "nb_activators": len(self.buttons) + len(self.targets),
                "activators": [
                    {"X": button[0], "Y": button[1], "type": 'I'} for button in self.buttons
                ] + [
                    {"X": target[0], "Y": target[1], "type": 'T'} for target in self.targets
                ],

                "nb_gates": len(self.gates),
                "gates": [gate.to_dict() for gate in self.gates.values()],

                "nb_doors": len(self.doors),
                "doors": [door.to_dict() for door in self.doors.values()],

                "nb_links": len(self.links),
                "links": [link.to_dict() for link in self.links.values()],
            },

            "text": self.text, "name": self.name, "perf_steps": self.perf_steps, "flags": self.flags
        }
    
    def get_link_dict(self) :
        return self.links | {"": None}
    
    def save(self, dir_, swap: bool = True) :
        self.flags["can_swap"] = swap
        json.dump(self.to_dict(), open(dir_, 'w'))

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
        
        for door in self.doors.values() :
            if (x, y) in door.tiles :
                door.tiles.pop((x, y))
    
    def can_place(self, x: int, y: int, type_: str) :

        # Walls and goal check :

        if type_ not in {"link", "gate", "door_tile", "door_hub"} and self.get_tile(x, y) in {'X', 'x'} :
            return False
        
        if (x, y) == self.goal :
            return False

        # Building sets of blocked coordinates :

        entities = self.boxes.copy()
        if self.player["alive"] :
            entities.add((self.player["X"], self.player["Y"]))
        if self.bullet["alive"] :
            entities.add((self.bullet["X"], self.bullet["Y"]))

        logics = self.buttons | self.targets | set(self.gates.keys())

        door_tiles = set()
        for door in self.doors.values() :
            door_tiles |= set(door.tiles.keys())
        door_hubs = set(self.doors.keys())
        
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
        
        if type_ == "link" :
            return (x, y) not in (logics | door_hubs)

        return True

    def add_door_hub(self, x: int, y: int) :
        if (x, y) not in self.doors.keys() :
            self.doors[(x, y)] = Door(**{"X": x, "Y": y, "tiles": []})
    
    def add_gate(self, x: int, y: int, type_: str) :
        if (x, y) not in self.gates.keys() :
            self.gates[(x, y)] = Gate(**{"X": x, "Y": y, "type": type_, "dir": 'U'})
    
    def connect_door(self, tile_pos: tuple[int], tile_orient: str, hub_pos: tuple[int]) :
        for pos, door in self.doors.items() :
            if pos == hub_pos :
                door.tiles[tile_pos] = "N"
                return
        
        hx, hy = hub_pos
        tx, ty = tile_pos
        self.doors[hub_pos] = Door(**{"X": hx, "Y": hy, "tiles": [{"X": tx, "Y": ty, "orient": tile_orient}]})
    
    def remove_door_tile(self, x: int, y: int) :
        for door in self.doors.values() :
            if (x, y) in door.tiles.keys() :
                door.tiles.pop((x, y))
    
    def remove_door_hub(self, x: int, y: int) :
        if (x, y) in self.doors :
            self.doors.pop((x, y))
        self.remove_link(coords=(x, y))
            
    def remove_gate(self, x: int, y: int, type_: str) :
        if (x, y) in self.gates :
            self.gates.pop((x, y))
        
        self.remove_link(coords=(x, y))
    
    def is_link_start(self, x: int, y: int) :
        return (x, y) in self.buttons | self.targets | set(self.gates.keys())

    def is_link_end(self, x: int, y: int) :
        return (x, y) in set(self.doors.keys()) | set(self.gates.keys())
    
    def add_link(self, nodes: list[tuple[int]]) -> bool :
        l = len(nodes)
        data = {
            "nb_nodes": l,
            "nodes": [
                {"X": node[0], "Y": node[1]}
                for node in nodes
            ],
            "offsets": [0 for _ in range(l-1)]
        }

        if nodes[0] in (self.buttons | self.targets) :
            data["type_start"] = "Activator"
        elif nodes[0] in self.gates.keys() :
            data["type_start"] = "Gate"
        else :
            return False
        
        if nodes[-1] in self.gates.keys() :
            data["type_end"] = "Gate"
        elif nodes[-1] in self.doors.keys() :
            data["type_end"] = "Door"
        else :
            return False
        
        new_link = Link(**data)
        self.links[new_link.get_id()] = new_link
        return True
    
    def remove_link(self, link_id: str = None, coords: tuple[int] = None) :        
        if link_id is not None and link_id in self.links :
            self.links.pop(link_id)
        
        if coords is not None :
            to_remove = set()
            for link_id, link in self.links.items() :
                if coords in link.nodes :
                    to_remove.add(link_id)
            
            for link_id in to_remove :
                self.links.pop(link_id)

    def change_obj_dir(self, amount: int, type_: str, pos: tuple[int] = (-1, -1)) :
        if type_ == "Player" :
            i_dir = dirs.index(self.player["dir"])
        elif type_ == "Bullet" :
            i_dir = dirs.index(self.bullet["dir"])
        elif type_ == "Gate" and pos in self.gates :
            i_dir = dirs.index(self.gates[pos].dir)
        else :
            return
        
        i_dir = (i_dir + amount)%4
        
        if type_ == "Player" :
            self.player["dir"] = dirs[i_dir]
        elif type_ == "Bullet" :
            self.bullet["dir"] = dirs[i_dir]
        elif type_ == "Gate" :
            self.gates[pos].dir = dirs[i_dir]
    
    def change_door_orient(self, amount: int, pos: tuple[int]) :
        for door in self.doors.values() :
            if pos in door.tiles.keys() :
                i_or = orients.index(door.tiles[pos])
                i_or = (i_or + amount)%len(orients)
                door.tiles[pos] = orients[i_or]
                return