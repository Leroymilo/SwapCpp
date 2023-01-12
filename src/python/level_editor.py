import json
import io

import wx
import pygame as pg

from src.python.gen_level_editor import LevelEditor, ResizeDlg
from src.python.sprites import *
from src.python.level import Level, Door

blank_level = "levels/example_level.json"

tools = ["Wall", "Grate", "Goal", "Player", "Bullet", "Box", "Button", "Target", "AND Gate", "OR Gate", "NO Gate", "Door Tile", "Door Hub", "Connector"]
tool_icon_paths = {
    "Wall"      : "assets/Tiles/Wall.png",
    "Grate"     : "assets/Tiles/Grate.png",
    "Goal"      : "assets/Tiles/Win.png",
    "Player"    : "assets/Entities/playerR.png",
    "Bullet"    : "assets/Entities/bulletR.png",
    "Box"       : "assets/Entities/Box.png",
    "Button"    : "assets/Logic/Interruptor0.png",
    "Target"    : "assets/Logic/Target0.png",
    "AND Gate"  : "assets/Logic/AND0.png",
    "OR Gate"   : "assets/Logic/OR0.png",
    "NO Gate"   : "assets/Logic/NO0.png",
    "Door Tile" : "assets/Logic/Door0.png",
    "Door Hub"  : "assets/door_hub.png",
    "Connector" : "assets/connector.png"
}

help_texts = {
    "Wall"      : "Right click to place, left click to remove. Placing removes other objects.",
    "Grate"     : "Right click to place, left click to remove. Placing removes other objects.",
    "Goal"      : "Right click to place. There can only be one, cannot be removed. Placing removes other objects (can serve as an eraser).",
    "Player"    : "Right click to place, left click to remove. There can only be one.",
    "Bullet"    : "Right click to place, left click to remove. There can only be one.",
    "Box"       : "Right click to place, left click to remove.",
    "Button"    : "Right click to place, left click to remove.",
    "Target"    : "Right click to place, left click to remove. Placing removes other objects.",
    "AND Gate"  : "Right click to place, left click to remove.",
    "OR Gate"   : "Right click to place, left click to remove.",
    "NO Gate"   : "Right click to place, left click to remove.",
    "Door Tile" : "Hold right click to place the tile, release to place a hud or connect to an existing hud. Left click to remove.",
    "Door Hub"  : "Right click to place, left click to remove (will remove connected tiles).",
    "Connector" : "Not implemented."
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
        self.status_bar: wx.StatusBar

        # Setting up the tools :
        self.tool = ""
        self.tools.AppendItems(tools)

        # Setting up level :
        self.level = Level(blank_level)
        self.surf = pg.Surface(self.level.get_surface(delta))

        # Setting up clicks :
        self.left_c = self.right_c = False
        self.prev_click = (-1, -1)

        # Setting up doors :
        self.door: Door = None
        
        # Setting up cables :
        self.cable_origin = None

        self.auto_path_check.Hide()
        self.display_level()

    def change_tool(self, event: wx.Event) :
        self.tool = self.tools.GetStringSelection()
        if self.tool not in tools :
            return
        path: str = tool_icon_paths[self.tool]
        bitmap = wx.Bitmap(path, type=wx.BITMAP_TYPE_PNG)
        self.tool_icon.SetBitmap(bitmap)

        if self.tool == "Connector" :
            self.auto_path_check.Show()
        else :
            self.auto_path_check.Hide()
        
        self.status_bar.PushStatusText(help_texts[self.tool], field = 0)
    
    def resize(self, event) :
        # Ask for new dimentions :
        W, H = self.level.size
        value = ResizeDlg(self, W, H).ShowModal()
        if value == 0 :
            return
        new_W, new_H = value // 100, value % 100

        # Update dimensions :
        self.size = (new_W, new_H)

        grid = self.level.grid
        new_grid = []
        for y in range(new_H) :
            if y >= H :
                new_grid.append(['.' for _ in range(new_W)])
                continue

            if new_W >= W :
                new_grid.append(grid[y] + ['.' for _ in range(new_W - W)])
            else :
                new_grid.append(grid[y][:new_W])
        self.level.grid = new_grid
        
        self.surf = pg.Surface((new_W * 32, new_H * 32))

        self.display_level()
    
    def display_level(self) :
        W, H = self.level.size

        # Draw :
        #background
        self.surf.fill(color=(0, 0, 0))
        for x in range(W) :
            for y in range(H) :
                tile = self.level.get_tile(x, y)

                if (x, y) == self.level.goal :
                    img = goal
                elif tile == 'X' :
                    img = wall
                elif tile == 'x' :
                    img = grate
                else :
                    img = floor
                
                self.surf.blit(img, (delta * x, delta * y))

        #logic
        for link in self.level.links :
            link.draw(self.surf, delta)

        for x, y in self.level.buttons :
            self.surf.blit(interruptor, (delta * x, delta * y))
        for x, y in self.level.targets :
            self.surf.blit(target, (delta * x, delta * y))
        
        for x, y in self.level.ands :
            self.surf.blit(AND, (delta * x, delta * y))
        for x, y in self.level.ors :
            self.surf.blit(OR, (delta * x, delta * y))
        for x, y in self.level.nos :
            self.surf.blit(NOT, (delta * x, delta * y))
        
        #doors
        for door_obj in self.level.doors :
            door_obj.draw_tiles(self.surf, delta)
        if self.door is not None :
            self.door.draw_tiles(self.surf, delta)
        
        for door_obj in self.level.doors :
            door_obj.draw_lines(self.surf, delta)
        if self.door is not None :
            self.door.draw_lines(self.surf, delta)

        for door_obj in self.level.doors :
            door_obj.draw_hub(self.surf, delta)
        if self.door is not None :
            self.door.draw_hub(self.surf, delta)
        
        #entities
        player_data = self.level.player
        if player_data["alive"] :
            img = player[player_data["dir"]]
            self.surf.blit(img, (delta * player_data["X"], delta * player_data["Y"]))
        
        bullet_data = self.level.bullet
        if bullet_data["alive"] :
            img = bullet[bullet_data["dir"]]
            self.surf.blit(img, (delta * bullet_data["X"], delta * bullet_data["Y"]))
        
        for x, y in self.level.boxes :
            self.surf.blit(box, (delta * x, delta * y))

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

        W, H = self.level.size
        x0, y0 = (disp_W - W * delta) // 2, (disp_H - H * delta) // 2
        x, y = (x - x0)//32, (y - y0)//32

        if not (0 <= x < W and 0 <= y < H) :
            self.display_error("Out of grid.")
            return
        
        if (x, y) == self.prev_click :
            return
        
        self.prev_click = (x, y)

        if self.right_c ^ self.left_c :
            self.update_level((x, y))
        
        if (self.tool == "Door Tile") and (self.door is not None) :
            tx, ty = self.door.tiles.pop()
            self.door.tiles.add((tx, ty))
            if self.right_c :
                self.door.pos = (x, y)
            else :
                if (self.level.can_place(x, y, "door_hub")) :
                    self.level.erase_any(tx, ty)
                    self.level.connect_door((tx, ty), (x, y))
                else :
                    self.display_error("Cannot place door hub here !")
                self.door = None

        self.display_level()


    def update_level(self, click: tuple[int]) :
        x, y = click

        tile = self.level.get_tile(x, y)

        if self.tool == "Wall" :
            if self.right_c :
                if (x, y) != self.level.goal :
                    self.level.erase_any(x, y)
                    self.level.set_tile(x, y, 'X')
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c and tile == 'X' :
                self.level.set_tile(x, y, '.')
        
        elif self.tool == "Grate" :
            if self.right_c :
                if (x, y) != self.level.goal :
                    self.level.erase_any(x, y)
                    self.level.set_tile(x, y, 'x')
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c and tile == 'x' :
                self.level.set_tile(x, y, '.')
        
        elif self.tool == "Goal" :
            if self.right_c :
                self.level.goal = x, y
                self.level.erase_any(x, y)
                self.level.grid[y][x] = '.'

        elif self.tool == "Player" :
            if self.right_c :
                if self.level.can_place(x, y, "player") :
                    self.level.player["X"], self.level.player["Y"] = x, y
                    self.level.player["alive"] = True
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c and (self.level.player["X"], self.level.player["Y"]) == (x, y) :
                self.level.player["X"], self.level.player["Y"] = 0, 0
                self.level.player["alive"] = False
        
        elif self.tool == "Bullet" :
            if self.right_c :
                if self.level.can_place(x, y, "bullet") :
                    self.level.bullet["X"], self.level.bullet["Y"] = x, y
                    self.level.bullet["alive"] = True
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c and (self.level.bullet["X"], self.level.bullet["Y"]) == (x, y) :
                self.level.bullet["X"], self.level.bullet["Y"] = 0, 0
                self.level.bullet["alive"] = False
            
        elif self.tool == "Box" :
            if self.right_c :
                if self.level.can_place(x, y, "box") :
                    self.level.boxes.add((x, y))
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c and (x, y) in self.level.boxes :
                self.level.boxes.remove((x, y))
        
        elif self.tool == "Button" :
            if self.right_c :
                if self.level.can_place(x, y, "button") :
                    self.level.buttons.add((x, y))
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c and (x, y) in self.level.buttons :
                self.level.buttons.remove((x, y))

        elif self.tool == "Target" :
            if self.right_c :
                if self.level.can_place(x, y, "target") :
                    self.level.erase_any(x, y)
                    self.level.targets.add((x, y))
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c and (x, y) in self.level.targets :
                self.level.targets.remove((x, y))
        
        elif self.tool.endswith("Gate") :
            type_ = self.tool.split(' ')[0]
            if self.right_c :
                if self.level.can_place(x, y, "gate") :
                    self.level.gates[type_].add((x, y))
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c and (x, y) in self.level.gates[type_] :
                self.level.gates[type_].remove((x, y))
        
        elif self.tool == "Door Tile" :
            if self.right_c and (self.door is None) :
                if self.level.can_place(x, y, "door_tile") :
                    self.door = Door({"X": x, "Y": y, "tiles": [{"X": x, "Y": y}]})
                else :
                    self.display_error("Cannot place door tile here !")
            elif self.left_c :
                self.level.remove_door_tile(x, y)
        
        elif self.tool == "Door Hub" :
            if self.right_c :
                if self.level.can_place(x, y, "door_hub") :
                    self.level.add_door_hub(x, y)
                else :
                    self.display_error("Cannot place here !")
                    return
            elif self.left_c :
                self.level.remove_door_hub(x, y)
        
        self.clear_error()


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
        self.process_click(event)
        self.prev_click = (-1, -1)

    def display_error(self, text: str) :
        self.error_timer.StartOnce(5000)
        self.status_bar.PushStatusText(text, field = 1)
    
    def clear_error(self) :
        self.error_timer.Stop()
        if self.status_bar.GetField(1).GetText() != "" :
            self.status_bar.PopStatusText(field = 1)

    def end_timer(self, event) :
        self.status_bar.PopStatusText(field = 1)