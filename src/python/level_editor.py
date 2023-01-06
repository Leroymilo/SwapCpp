import json
import io

import wx
import pygame as pg

from src.python.gen_level_editor import LevelEditor
from src.python.sprites import *

blank_level = json.load(open("levels/example_level.json"))

tools = ["Wall", "Grate", "Goal", "Player", "Bullet", "Box", "Button", "Target", "Door", "AND Gate", "OR Gate", "NO Gate", "Connector"]
tool_icon_paths = {
    "Wall"      : "assets/Tiles/Wall.png",
    "Grate"     : "assets/Tiles/Grate.png",
    "Goal"      : "assets/Tiles/Win.png",
    "Player"    : "assets/Entities/playerR.png",
    "Bullet"    : "assets/Entities/bulletR.png",
    "Box"       : "assets/Entities/Box.png",
    "Button"    : "assets/Logic/Interruptor0.png",
    "Target"    : "assets/Logic/Target0.png",
    "Door"      : "assets/Logic/Door0.png",
    "AND Gate"  : "assets/Logic/AND0.png",
    "OR Gate"   : "assets/Logic/OR0.png",
    "NO Gate"   : "assets/Logic/NO0.png",
    "Connector" : "assets/connector.png"
}

class LevelEditor(LevelEditor) :
    def __init__(self, parent):
        super().__init__(parent)

        # Setting up the tools :
        self.tool = ""
        self.tools.AppendItems(tools)

        # Setting up level :
        self.level = blank_level
        self.surf = pg.Surface((self.level["bg"]["W"] * 32, self.level["bg"]["H"] * 32))

        self.display_level()

    def change_tool(self, event: wx.Event) :
        self.tool = self.tools.GetStringSelection()
        if self.tool not in tools :
            return
        path: str = tool_icon_paths[self.tool]
        bitmap = wx.Bitmap(path, type=wx.BITMAP_TYPE_PNG)
        self.tool_icon.SetBitmap(bitmap)
    
    def resize(self, event) :
        # Ask for new dimentions :

        # Update dimensions :
        #self.level["bg"]["H"] = new_H
        #self.level["bg"]["W"] = new_W
        #self.surf = pg.Surface((new_W * 32, new_H * 32))
        pass
    
    def display_level(self) :
        W, H = self.level["bg"]["W"], self.level["bg"]["H"]

        # Draw :
        #background
        self.surf.fill(color=(0, 0, 0))
        for x in range(W) :
            for y in range(H) :
                tile = self.level["bg"]["BG"][y][x]

                if (x, y) == (self.level["bg"]["EndX"], self.level["bg"]["EndY"]) :
                    img = goal
                elif tile == 'X' :
                    img = wall
                elif tile == 'x' :
                    img = grate
                else :
                    img = floor
                
                self.surf.blit(img, (delta * x, delta * y))
        
        #entities
        player_data = self.level["entities"]["player"]
        if player_data["alive"] :
            img = player[player_data["dir"]]
            self.surf.blit(img, (delta * player_data["X"], delta * player_data["Y"]))
        
        bullet_data = self.level["entities"]["bullet"]
        if bullet_data["alive"] :
            img = bullet[bullet_data["dir"]]
            self.surf.blit(img, (delta * bullet_data["X"], delta * bullet_data["Y"]))
        
        for box_data in self.level["entities"]["Boxes"] :
            x, y = box_data["X"], box_data["Y"]
            self.surf.blit(box, (delta * x, delta * y))

        #logic
        for activator in self.level["logic"]["activators"] :
            x, y, type_ = activator["X"], activator["Y"], activator["type"]

            if type_ == 'I' :
                self.surf.blit(interruptor, (delta * x, delta * y))
            elif type_ == 'T' :
                self.surf.blit(target, (delta * x, delta * y))
        
        for gate in self.level["logic"]["gates"] :
            x, y, type_ = gate["X"], gate["Y"], gate["type"]

            if type_ == '&' :
                self.surf.blit(AND, (delta * x, delta * y))
            elif type_ == '|' :
                self.surf.blit(OR, (delta * x, delta * y))
            elif type_ == '!' :
                self.surf.blit(NOT, (delta * x, delta * y))
        
        for door_data in self.level["logic"]["doors"] :
            for tile in door_data["tiles"] :
                x, y = tile["X"], tile["Y"]
                self.surf.blit(door, (delta * x, delta * y))

        # Display :
        bitmap = io.BytesIO()
        pg.image.save(self.surf, "assets/temp.png", "PNG")
        bmp = wx.Bitmap(wx.Image("assets/temp.png"))
        self.display.SetBitmap(bmp)

        # It would be very nice if saving an image was avoidable,
        # but I couldn't make wxPython read a io.BytesIO for some reason.