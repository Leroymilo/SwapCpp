import wx

from src.python.level_editor import LevelEditor

app = wx.App()
frame = LevelEditor(None)
frame.Show()
frame.Maximize()
app.MainLoop()