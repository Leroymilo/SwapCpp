import json
import io

import wx
import pygame as pg

from src.python.gen_level_editor import LevelEditor, ResizeDlg
from src.python.sprites import *
from src.python.level import Level

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


class ResizeDlg(ResizeDlg) :
    def __init__(self, parent, cur_W: int, cur_H: int) :
        super().__init__(parent)
        self.height_spin.SetValue(cur_H)
        self.width_spin.SetValue(cur_W)
    
    def confirm(self, event) :
        value = 100 * self.width_spin.GetValue() + self.height_spin.GetValue()
        self.EndModal(value)
    
    def Close(self, force=False):
        self.EndModal(0)
        return super().Close(force)


class LevelEditor(LevelEditor) :
    def __init__(self, parent):
        super().__init__(parent)

        # Setting up the tools :
        self.tool = ""
        self.tools.AppendItems(tools)

        # Setting up level :
        self.level = blank_level
        self.surf = pg.Surface((self.level["bg"]["W"] * 32, self.level["bg"]["H"] * 32))

        # Setting up clicks :
        self.left_c = self.right_c = False
        self.prev_click = (-1, -1)

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
        W, H = self.level["bg"]["W"], self.level["bg"]["H"]
        value = ResizeDlg(self, W, H).ShowModal()
        if value == 0 :
            return
        new_W, new_H = value // 100, value % 100

        # Update dimensions :
        self.level["bg"]["W"] = new_W
        self.level["bg"]["H"] = new_H

        grid = self.level["bg"]["BG"]
        new_grid = []
        for y in range(new_H) :
            if y >= H :
                new_grid.append(['.' for _ in range(new_W)])
                continue

            if new_W >= W :
                new_grid.append(grid[y] + ['.' for _ in range(new_W - W)])
            else :
                new_grid.append(grid[y][:new_W])
        self.level["bg"]["BG"] = new_grid
        
        self.surf = pg.Surface((new_W * 32, new_H * 32))

        self.display_level()
    
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

        # TODO : cables 

        # Display :
        pg.image.save(self.surf, "assets/temp.png", "PNG")
        bmp = wx.Bitmap(wx.Image("assets/temp.png"))
        self.display.SetBitmap(bmp)
        self.Layout()

        # It would be very nice if saving an image was avoidable,
        # but I couldn't make wxPython read a io.BytesIO for some reason.
    
    def process_click(self, event: wx.MouseEvent) :
        x, y = event.GetPosition()
        disp_W, disp_H = self.display.GetSize()

        W, H = self.level["bg"]["W"], self.level["bg"]["H"]
        x0, y0 = (disp_W - W * delta) // 2, (disp_H - H * delta) // 2
        x, y = (x - x0)//32, (y - y0)//32

        if not (0 <= x < W and 0 <= y < H) :
            return
        
        if (x, y) == self.prev_click :
            return
        
        self.prev_click = (x, y)
        self.update_level((x, y))

    def update_level(self, click: tuple[int]) :
        x, y = click

        grid = self.level["bg"]["BG"]
        tile = grid[y][x]

        activators: list[dict[str, int | str]] = self.level["logic"]["activators"]
        gates: list[dict[str, int]] = self.level["logic"]["gates"]
        doors: list[dict[str, int | list]] = self.level["logic"]["doors"]
        gate_dict = {"AND": '&', "OR": '|', "NO": '!'}

        if self.tool == "Wall" :
            if self.right_c :
                grid[y][x] = 'X'
            elif self.left_c and tile == 'X' :
                grid[y][x] = '.'
        
        elif self.tool == "Grate" :
            if self.right_c :
                grid[y][x] = 'x'
            elif self.left_c and tile == 'x' :
                grid[y][x] = '.'
        
        elif self.tool == "Goal" :
            if self.right_c :
                self.level["bg"]["EndX"], self.level["bg"]["EndY"] = x, y

        elif self.tool == "Player" :
            player = self.level["entities"]["player"]
            if self.right_c :
                player["X"], player["Y"] = x, y
                player["alive"] = True
            elif self.left_c and (player["X"], player["Y"]) == (x, y) :
                player["X"], player["Y"] = 0, 0
                player["alive"] = False
        
        elif self.tool == "Bullet" :
            bullet = self.level["entities"]["bullet"]
            if self.right_c :
                bullet["X"], bullet["Y"] = x, y
                bullet["alive"] = True
            elif self.left_c and (bullet["X"], bullet["Y"]) == (x, y) :
                bullet["X"], bullet["Y"] = 0, 0
                bullet["alive"] = False
            
        elif self.tool == "Box" :
            boxes: list[dict[str, int]] = self.level["entities"]["Boxes"]
            cur_box = {"X": x, "Y": y}
            if self.right_c and cur_box not in boxes :
                boxes.append(cur_box)
            elif self.left_c and cur_box in boxes :
                boxes.remove(cur_box)
        
        elif self.tool == "Button" :
            cur_button = {"type": "I", "X": x, "Y": y}
            if self.right_c and cur_button not in activators :
                activators.append(cur_button)
            elif self.left_c and cur_button in activators :
                activators.remove(cur_button)

        elif self.tool == "Target" :
            cur_target = {"type": "T", "X": x, "Y": y}
            if self.right_c and cur_target not in activators :
                activators.append(cur_target)
            elif self.left_c and cur_target in activators :
                activators.remove(cur_target)
        
        elif self.tool.endswith("Gate") :
            cur_gate = {"type": gate_dict[self.tool.split(' ')[0]], "X": x, "Y": y}
            if self.right_c and cur_gate not in gates :
                gates.append(cur_gate)
            elif self.left_c and cur_gate in gates :
                gates.remove(cur_gate)

            
        self.display_level()


    def mouse_move(self, event: wx.MouseEvent) :
        if self.right_c ^ self.left_c :
            self.process_click(event)
    
    def left_down(self, event: wx.MouseEvent) :
        self.left_c = True
        self.process_click(event)
    
    def right_down(self, event: wx.MouseEvent) :
        self.right_c = True
        self.process_click(event)
    
    def left_up(self, event) :
        self.left_c = False
        self.prev_click = (-1, -1)
    
    def right_up(self, event) :
        self.right_c = False
        self.prev_click = (-1, -1)