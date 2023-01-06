import wx

from src.gen_level_editor import LevelEditor

app = wx.App()
frame = LevelEditor(None)
frame.Show()
# frame.Maximize()
app.MainLoop()