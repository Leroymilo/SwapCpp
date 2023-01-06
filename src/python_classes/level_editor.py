import json

import wx

from src.python_classes.gen_level_editor import LevelEditor

blank_level = json.load(open("levels/blank_level.json"))

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

        self.tool_bar: wx.ToolBar

        self.tools.AppendItems(tools)

    def change_tool(self, event: wx.Event) :
        self.tool = self.tools.GetStringSelection()
        if self.tool not in tools :
            return
        path: str = tool_icon_paths[self.tool]
        bitmap = wx.Bitmap(path, type=wx.BITMAP_TYPE_PNG)
        self.tool_icon.SetBitmap(bitmap)