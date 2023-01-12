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

		self.status_bar = self.CreateStatusBar( 2, wx.STB_ELLIPSIZE_END|wx.STB_SHOW_TIPS|wx.STB_SIZEGRIP, wx.ID_ANY )
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
		self.auto_path_check = wx.CheckBox( self.tool_bar, wx.ID_ANY, u"Auto path :", wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_RIGHT )
		self.auto_path_check.SetValue(True)
		self.auto_path_check.Hide()

		self.tool_bar.AddControl( self.auto_path_check )
		self.tool_bar.Realize()

		sizer = wx.BoxSizer( wx.VERTICAL )

		self.display = wx.StaticBitmap( self, wx.ID_ANY, wx.NullBitmap, wx.DefaultPosition, wx.DefaultSize, 0 )
		sizer.Add( self.display, 1, wx.ALL|wx.EXPAND, 5 )


		self.SetSizer( sizer )
		self.Layout()
		self.error_timer = wx.Timer()
		self.error_timer.SetOwner( self, wx.ID_ANY )

		self.Centre( wx.BOTH )

		# Connect Events
		self.Bind( wx.EVT_MENU, self.open_level, id = self.open_item.GetId() )
		self.Bind( wx.EVT_MENU, self.new_level, id = self.new_item.GetId() )
		self.Bind( wx.EVT_MENU, self.save_level, id = self.save_item.GetId() )
		self.Bind( wx.EVT_MENU, self.resize, id = self.resize_item.GetId() )
		self.tools.Bind( wx.EVT_CHOICE, self.change_tool )
		self.display.Bind( wx.EVT_LEFT_DOWN, self.left_down )
		self.display.Bind( wx.EVT_LEFT_UP, self.left_up )
		self.display.Bind( wx.EVT_MOTION, self.mouse_move )
		self.display.Bind( wx.EVT_RIGHT_DOWN, self.right_down )
		self.display.Bind( wx.EVT_RIGHT_UP, self.right_up )
		self.Bind( wx.EVT_TIMER, self.end_timer, id=wx.ID_ANY )

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

	def left_down( self, event ):
		event.Skip()

	def left_up( self, event ):
		event.Skip()

	def mouse_move( self, event ):
		event.Skip()

	def right_down( self, event ):
		event.Skip()

	def right_up( self, event ):
		event.Skip()

	def end_timer( self, event ):
		event.Skip()


###########################################################################
## Class ResizeDlg
###########################################################################

class ResizeDlg ( wx.Dialog ):

	def __init__( self, parent ):
		wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"Choose new dimentions", pos = wx.DefaultPosition, size = wx.Size( 223,142 ), style = wx.DEFAULT_DIALOG_STYLE )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

		v_sizer = wx.BoxSizer( wx.VERTICAL )

		h_sizer_height = wx.BoxSizer( wx.HORIZONTAL )

		self.height_text = wx.StaticText( self, wx.ID_ANY, u"Height :", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.height_text.Wrap( -1 )

		h_sizer_height.Add( self.height_text, 1, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )

		self.height_spin = wx.SpinCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.SP_ARROW_KEYS, 3, 20, 3 )
		h_sizer_height.Add( self.height_spin, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )


		v_sizer.Add( h_sizer_height, 0, wx.EXPAND, 5 )

		h_sizer_width = wx.BoxSizer( wx.HORIZONTAL )

		self.width_text = wx.StaticText( self, wx.ID_ANY, u"Width :", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.width_text.Wrap( -1 )

		h_sizer_width.Add( self.width_text, 1, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )

		self.width_spin = wx.SpinCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.SP_ARROW_KEYS, 3, 40, 3 )
		h_sizer_width.Add( self.width_spin, 0, wx.ALL, 5 )


		v_sizer.Add( h_sizer_width, 0, wx.EXPAND, 5 )

		self.confirm_button = wx.Button( self, wx.ID_ANY, u"Confirm", wx.DefaultPosition, wx.DefaultSize, 0 )
		v_sizer.Add( self.confirm_button, 0, wx.ALL|wx.ALIGN_RIGHT, 5 )


		self.SetSizer( v_sizer )
		self.Layout()

		self.Centre( wx.BOTH )

		# Connect Events
		self.confirm_button.Bind( wx.EVT_BUTTON, self.confirm )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def confirm( self, event ):
		event.Skip()


