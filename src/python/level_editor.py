import os

import wx
import pygame as pg

from src.python.gen_level_editor import LevelEditor, SizeDlg, CheckSaveDlg, NameDlg
from src.python.sprites import *
from src.python.level import Level, Door

main_path = "src".join(__file__.split("src")[:-1])
blank_level = "levels/blank_level.json"

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

base_editor_title: str

#=========================================================================================================================================================
# SizeDlg heirs :

class SizeDlg(SizeDlg) :
    def __init__(self, parent, cur_W: int, cur_H: int) :
        super().__init__(parent)
        self.height_spin.SetValue(cur_H)
        self.width_spin.SetValue(cur_W)
    
    def confirm(self, event) :
        value = 100 * self.width_spin.GetValue() + self.height_spin.GetValue()
        self.EndModal(value)
    
    def close(self, event) :
        self.EndModal(0)

class ResizeDlg(SizeDlg) :
    def __init__(self, parent, cur_W: int, cur_H: int):
        super().__init__(parent, cur_W, cur_H)
        self.SetTitle("Choose new dimentions")

#=========================================================================================================================================================
# CheckSaveDlg :

class CheckSaveDlg(CheckSaveDlg) :
    def __init__(self, parent) :
        super().__init__(parent)
    
    def yes(self, event) :
        self.EndModal(1)
    
    def no(self, event) :
        self.EndModal(-1)
    
    def cancel(self, event) :
        self.EndModal(0)
    
    def close(self, event) :
        self.EndModal(0)

#=========================================================================================================================================================
# CheckSaveDlg :

class NameDlg(NameDlg) :
    def __init__(self, parent: LevelEditor, cur_name = None) :
        super().__init__(parent)
        if cur_name is not None :
            self.text_ctrl.SetValue(str(cur_name))
    
    def confirm(self, event) :
        name: str = self.text_ctrl.GetValue()

        if name == "" :
            wx.MessageDialog(self, "Name cannot be empty !").ShowModal()
            return
        
        if not name.endswith(".json") :
            name += ".json"
        
        if os.path.exists("levels/" + name) :
            res = wx.MessageDialog(
                self,
                "This level already exists, do you want to replace it?",
                style=wx.YES_NO|wx.CENTRE
            ).ShowModal()
            if res == 5104 :
                return

        parent: LevelEditor = self.GetParent()
        parent.set_level_name(name)
        self.EndModal(0)

#=========================================================================================================================================================
# LevelEditor :

class LevelEditor(LevelEditor) :
    def __init__(self, parent):
        global base_editor_title
        super().__init__(parent)
        self.level_name = None
        self.edited = False
        self.status_bar: wx.StatusBar
        base_editor_title = self.GetTitle()

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
    
    def check_save(self) :
        """Returns 1 if saved or save skipped, 0 if canceled"""
        if not self.edited :
            return 1
        
        res = CheckSaveDlg(self).ShowModal()

        if res == -1 :
            return 1
        
        if res == 0 :
            return 0

        return self.save_level(None)
    
    def close(self, event: wx.Event) :
        if self.check_save() :
            event.Skip()
    
    def set_level_name(self, new_name: str) :
        self.level_name = new_name
        self.SetTitle(base_editor_title + " : *" + self.level_name)
    
    #=========================================================================================================================================================
    # Display :

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
    
    #=========================================================================================================================================================
    # Level edition methods

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
        
        else :
            self.display_error("Unknown tool somehow...")

        if self.level_name is None :
            self.SetTitle(base_editor_title + " : *unnamed_level.json")
        else :
            self.SetTitle(base_editor_title + " : *" + self.level_name)
        self.edited = True
        self.clear_error()

    #=========================================================================================================================================================
    # Click processing :

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

    #=========================================================================================================================================================
    # Status bar error display :

    def display_error(self, text: str) :
        self.error_timer.StartOnce(5000)
        self.status_bar.PushStatusText(text, field = 1)
    
    def clear_error(self) :
        self.error_timer.Stop()
        if self.status_bar.GetField(1).GetText() != "" :
            self.status_bar.PopStatusText(field = 1)

    def end_timer(self, event) :
        self.status_bar.PopStatusText(field = 1)

    #=========================================================================================================================================================
    # File menu :

    def save_level(self, event, save_as=False) :
        """Returns 1 if saved, 0 if canceled"""
        if (self.level_name is None) or save_as :
            res = NameDlg(self, self.level_name).ShowModal()
            if res == 5101 :
                return 0
        
        self.level.save("levels/" + self.level_name)
        
        self.edited = False
        self.SetTitle(base_editor_title + " : " + self.level_name)
        self.display_error(f"Level saved as \"{self.level_name}\" !")
        return 1
    
    def save_level_as(self, event) :
        return self.save_level(None, True)

    def new_level(self, event) :
        if not self.check_save() : return

        value = SizeDlg(self, 7, 7).ShowModal()
        if value == 0 :
            return
        new_W, new_H = value // 100, value % 100

        self.level = Level(blank_level)
        self.level.size = (new_W, new_H)
        self.surf = pg.Surface((new_W * delta, new_H * delta))

        self.level.grid = [['X' for _ in range(new_W)]] + \
            [['X'] + ['.' for _ in range(new_W-2)] + ['X'] for _ in range(new_H-2)] + \
            [['X' for _ in range(new_W)]]
        
        self.level.goal = (new_W//2, 1)
        self.level.player["X"] = (new_W-1)//2
        self.level.player["Y"] = new_H-2
        self.display_level()
    
    def open_level(self, event) :
        if not self.check_save() : return

        level_path: str = wx.FileSelector(
            message="Choose level to open",
            default_path=main_path + "levels\\",
            default_filename="example_level.json",
            default_extension=".json",
            parent=self
        )

        if level_path == "" :
            return
        
        level_path = level_path.removeprefix(main_path)

        self.level = Level(level_path)
        W, H = self.level.size
        self.surf = pg.Surface((W * delta, H * delta))
        self.display_level()

        level_name = level_path.removeprefix("levels/")
        level_name = level_name.removeprefix("levels\\")
        self.level_name = level_name
    
    #=========================================================================================================================================================
    # Edit menu :

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
        
        self.surf = pg.Surface((new_W * delta, new_H * delta))

        self.display_level()