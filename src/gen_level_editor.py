# -*- coding: utf-8 -*-

###########################################################################
## Python code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
## http://www.wxformbuilder.org/
##
## PLEASE DO *NOT* EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc

###########################################################################
## Class LevelEditor
###########################################################################

class LevelEditor ( wx.Frame ):

	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"Swap! Level Editor", pos = wx.DefaultPosition, size = wx.Size( 819,571 ), style = wx.DEFAULT_FRAME_STYLE|wx.MAXIMIZE|wx.TAB_TRAVERSAL )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

		global_sizer = wx.BoxSizer( wx.VERTICAL )

		self.global_panel = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		v_sizer = wx.BoxSizer( wx.VERTICAL )

		self.m_bitmap1 = wx.StaticBitmap( self.global_panel, wx.ID_ANY, wx.NullBitmap, wx.DefaultPosition, wx.DefaultSize, 0 )
		v_sizer.Add( self.m_bitmap1, 1, wx.ALL|wx.EXPAND, 5 )

		self.help_text = wx.StaticText( self.global_panel, wx.ID_ANY, u"Right click to place, left click to remove.", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.help_text.Wrap( -1 )

		v_sizer.Add( self.help_text, 0, wx.ALL, 5 )


		self.global_panel.SetSizer( v_sizer )
		self.global_panel.Layout()
		v_sizer.Fit( self.global_panel )
		global_sizer.Add( self.global_panel, 1, wx.EXPAND |wx.ALL, 5 )


		self.SetSizer( global_sizer )
		self.Layout()
		self.menu_bar = wx.MenuBar( 0 )
		self.file_menu = wx.Menu()
		self.open_item = wx.MenuItem( self.file_menu, wx.ID_ANY, u"Open", wx.EmptyString, wx.ITEM_NORMAL )
		self.file_menu.Append( self.open_item )

		self.new_item = wx.MenuItem( self.file_menu, wx.ID_ANY, u"New", wx.EmptyString, wx.ITEM_NORMAL )
		self.file_menu.Append( self.new_item )

		self.save_item = wx.MenuItem( self.file_menu, wx.ID_ANY, u"Save"+ u"\t" + u"CTRL+S", wx.EmptyString, wx.ITEM_NORMAL )
		self.file_menu.Append( self.save_item )

		self.menu_bar.Append( self.file_menu, u"File" )

		self.edit_menu = wx.Menu()
		self.resize_item = wx.MenuItem( self.edit_menu, wx.ID_ANY, u"Resize Level", wx.EmptyString, wx.ITEM_NORMAL )
		self.edit_menu.Append( self.resize_item )

		self.menu_bar.Append( self.edit_menu, u"Edit" )

		self.SetMenuBar( self.menu_bar )

		self.tool_bar = self.CreateToolBar( wx.TB_TEXT|wx.TB_VERTICAL, wx.ID_ANY )
		self.wall_tool = self.tool_bar.AddTool( wx.ID_ANY, u"Wall", wx.Bitmap( u"../assets/Tiles/Wall.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_RADIO, wx.EmptyString, wx.EmptyString, None )

		self.grate_tool = self.tool_bar.AddTool( wx.ID_ANY, u"Grate", wx.Bitmap( u"../assets/Tiles/Grate.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_RADIO, wx.EmptyString, wx.EmptyString, None )

		self.goal_tool = self.tool_bar.AddTool( wx.ID_ANY, u"Goal", wx.Bitmap( u"../assets/Tiles/Win.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_RADIO, wx.EmptyString, wx.EmptyString, None )

		self.player_tool = self.tool_bar.AddTool( wx.ID_ANY, u"Player", wx.Bitmap( u"../assets/Entities/playerR.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_RADIO, wx.EmptyString, wx.EmptyString, None )

		self.box_tool = self.tool_bar.AddTool( wx.ID_ANY, u"Box", wx.Bitmap( u"../assets/Entities/Box.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_RADIO, wx.EmptyString, wx.EmptyString, None )

		self.button_tool = self.tool_bar.AddTool( wx.ID_ANY, u"Button", wx.Bitmap( u"../assets/Logic/Interruptor0.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_RADIO, wx.EmptyString, wx.EmptyString, None )

		self.door_tool = self.tool_bar.AddTool( wx.ID_ANY, u"Door", wx.Bitmap( u"../assets/Logic/Door0.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_RADIO, wx.EmptyString, wx.EmptyString, None )

		self.and_tool = self.tool_bar.AddTool( wx.ID_ANY, u"AND Gate", wx.Bitmap( u"../assets/Logic/AND0.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_NORMAL, wx.EmptyString, wx.EmptyString, None )

		self.or_tool = self.tool_bar.AddTool( wx.ID_ANY, u"OR Gate", wx.Bitmap( u"../assets/Logic/OR0.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_NORMAL, wx.EmptyString, wx.EmptyString, None )

		self.not_tool = self.tool_bar.AddTool( wx.ID_ANY, u"NOT Gate", wx.Bitmap( u"../assets/Logic/NO0.png", wx.BITMAP_TYPE_ANY ), wx.NullBitmap, wx.ITEM_NORMAL, wx.EmptyString, wx.EmptyString, None )

		self.link_tool = self.tool_bar.AddTool( wx.ID_ANY, u"Link", wx.NullBitmap, wx.NullBitmap, wx.ITEM_RADIO, wx.EmptyString, wx.EmptyString, None )

		self.tool_bar.Realize()


		self.Centre( wx.BOTH )

		# Connect Events
		self.m_bitmap1.Bind( wx.EVT_LEFT_UP, self.left_click )
		self.m_bitmap1.Bind( wx.EVT_RIGHT_UP, self.right_click )
		self.Bind( wx.EVT_MENU, self.open_level, id = self.open_item.GetId() )
		self.Bind( wx.EVT_MENU, self.new_level, id = self.new_item.GetId() )
		self.Bind( wx.EVT_MENU, self.save_level, id = self.save_item.GetId() )
		self.Bind( wx.EVT_MENU, self.resize, id = self.resize_item.GetId() )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def left_click( self, event ):
		event.Skip()

	def right_click( self, event ):
		event.Skip()

	def open_level( self, event ):
		event.Skip()

	def new_level( self, event ):
		event.Skip()

	def save_level( self, event ):
		event.Skip()

	def resize( self, event ):
		event.Skip()


