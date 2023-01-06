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
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"Swap! Level Editor", pos = wx.DefaultPosition, size = wx.Size( 819,714 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

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

		self.status_bar = self.CreateStatusBar( 1, wx.STB_ELLIPSIZE_END|wx.STB_SHOW_TIPS|wx.STB_SIZEGRIP, wx.ID_ANY )
		self.tool_bar = self.CreateToolBar( wx.TB_DEFAULT_STYLE, wx.ID_ANY )
		self.tool_bar_text = wx.StaticText( self.tool_bar, wx.ID_ANY, u"Tool :", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.tool_bar_text.Wrap( -1 )

		self.tool_bar.AddControl( self.tool_bar_text )
		toolsChoices = []
		self.tools = wx.Choice( self.tool_bar, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, toolsChoices, 0 )
		self.tools.SetSelection( 0 )
		self.tool_bar.AddControl( self.tools )
		self.tool_icon = wx.StaticBitmap( self.tool_bar, wx.ID_ANY, wx.NullBitmap, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.tool_bar.AddControl( self.tool_icon )
		self.tool_bar.Realize()

		sizer = wx.BoxSizer( wx.VERTICAL )

		self.display = wx.StaticBitmap( self, wx.ID_ANY, wx.NullBitmap, wx.DefaultPosition, wx.DefaultSize, 0 )
		sizer.Add( self.display, 1, wx.ALL|wx.EXPAND, 5 )


		self.SetSizer( sizer )
		self.Layout()

		self.Centre( wx.BOTH )

		# Connect Events
		self.Bind( wx.EVT_MENU, self.open_level, id = self.open_item.GetId() )
		self.Bind( wx.EVT_MENU, self.new_level, id = self.new_item.GetId() )
		self.Bind( wx.EVT_MENU, self.save_level, id = self.save_item.GetId() )
		self.Bind( wx.EVT_MENU, self.resize, id = self.resize_item.GetId() )
		self.tools.Bind( wx.EVT_CHOICE, self.change_tool )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def open_level( self, event ):
		event.Skip()

	def new_level( self, event ):
		event.Skip()

	def save_level( self, event ):
		event.Skip()

	def resize( self, event ):
		event.Skip()

	def change_tool( self, event ):
		event.Skip()


