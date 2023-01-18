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

		self.save_as_item = wx.MenuItem( self.file_menu, wx.ID_ANY, u"Save as", wx.EmptyString, wx.ITEM_NORMAL )
		self.file_menu.Append( self.save_as_item )

		self.menu_bar.Append( self.file_menu, u"File" )

		self.edit_menu = wx.Menu()
		self.resize_item = wx.MenuItem( self.edit_menu, wx.ID_ANY, u"Resize Level", wx.EmptyString, wx.ITEM_NORMAL )
		self.edit_menu.Append( self.resize_item )

		self.level_text_item = wx.MenuItem( self.edit_menu, wx.ID_ANY, u"Change Level Text", wx.EmptyString, wx.ITEM_NORMAL )
		self.edit_menu.Append( self.level_text_item )

		self.allow_swap_item = wx.MenuItem( self.edit_menu, wx.ID_ANY, u"Allow Swaps", wx.EmptyString, wx.ITEM_CHECK )
		self.edit_menu.Append( self.allow_swap_item )
		self.allow_swap_item.Check( True )

		self.menu_bar.Append( self.edit_menu, u"Edit" )

		self.SetMenuBar( self.menu_bar )

		self.status_bar = self.CreateStatusBar( 2, wx.STB_ELLIPSIZE_END|wx.STB_SHOW_TIPS|wx.STB_SIZEGRIP, wx.ID_ANY )
		self.tool_bar = self.CreateToolBar( wx.TB_DEFAULT_STYLE, wx.ID_ANY )
		self.tool_bar.AddSeparator()

		self.tool_bar_text = wx.StaticText( self.tool_bar, wx.ID_ANY, u"Tool : ", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.tool_bar_text.Wrap( -1 )

		self.tool_bar.AddControl( self.tool_bar_text )
		toolsChoices = []
		self.tools = wx.Choice( self.tool_bar, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, toolsChoices, 0 )
		self.tools.SetSelection( 0 )
		self.tool_bar.AddControl( self.tools )
		self.tool_icon = wx.StaticBitmap( self.tool_bar, wx.ID_ANY, wx.NullBitmap, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.tool_bar.AddControl( self.tool_icon )
		self.tool_bar.AddSeparator()

		self.auto_path_check = wx.CheckBox( self.tool_bar, wx.ID_ANY, u"Auto path :", wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_RIGHT )
		self.auto_path_check.Hide()

		self.tool_bar.AddControl( self.auto_path_check )
		self.tool_bar.AddSeparator()

		self.sel_link_text = wx.StaticText( self.tool_bar, wx.ID_ANY, u"Selected link : ", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.sel_link_text.Wrap( -1 )

		self.sel_link_text.Hide()

		self.tool_bar.AddControl( self.sel_link_text )
		sel_link_choiceChoices = []
		self.sel_link_choice = wx.Choice( self.tool_bar, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, sel_link_choiceChoices, 0 )
		self.sel_link_choice.SetSelection( 0 )
		self.sel_link_choice.Hide()

		self.tool_bar.AddControl( self.sel_link_choice )
		self.tool_bar.AddSeparator()

		self.sel_seg_text = wx.StaticText( self.tool_bar, wx.ID_ANY, u"Segment : ", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.sel_seg_text.Wrap( -1 )

		self.sel_seg_text.Hide()

		self.tool_bar.AddControl( self.sel_seg_text )
		self.sel_seg_spin = wx.SpinCtrl( self.tool_bar, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_RIGHT|wx.SP_ARROW_KEYS|wx.SP_WRAP, 0, 10, 0 )
		self.sel_seg_spin.Hide()

		self.tool_bar.AddControl( self.sel_seg_spin )
		self.tool_bar.AddSeparator()

		self.offset_text = wx.StaticText( self.tool_bar, wx.ID_ANY, u"Offset : ", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.offset_text.Wrap( -1 )

		self.tool_bar.AddControl( self.offset_text )
		self.offset_spin = wx.SpinCtrl( self.tool_bar, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.SP_ARROW_KEYS|wx.SP_WRAP, -7, 8, 0 )
		self.offset_spin.Hide()

		self.tool_bar.AddControl( self.offset_spin )
		self.tool_bar.AddSeparator()

		self.del_link_button = wx.Button( self.tool_bar, wx.ID_ANY, u"Delete", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.del_link_button.Hide()

		self.tool_bar.AddControl( self.del_link_button )
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
		self.Bind( wx.EVT_CHAR_HOOK, self.process_key )
		self.Bind( wx.EVT_CLOSE, self.close )
		self.Bind( wx.EVT_MENU, self.open_level, id = self.open_item.GetId() )
		self.Bind( wx.EVT_MENU, self.new_level, id = self.new_item.GetId() )
		self.Bind( wx.EVT_MENU, self.save_level, id = self.save_item.GetId() )
		self.Bind( wx.EVT_MENU, self.save_level_as, id = self.save_as_item.GetId() )
		self.Bind( wx.EVT_MENU, self.resize, id = self.resize_item.GetId() )
		self.Bind( wx.EVT_MENU, self.change_level_text, id = self.level_text_item.GetId() )
		self.tools.Bind( wx.EVT_CHOICE, self.change_tool )
		self.sel_link_choice.Bind( wx.EVT_CHOICE, self.change_link )
		self.sel_seg_spin.Bind( wx.EVT_SPINCTRL, self.change_segment )
		self.offset_spin.Bind( wx.EVT_SPINCTRL, self.change_offset )
		self.del_link_button.Bind( wx.EVT_BUTTON, self.delete_link )
		self.display.Bind( wx.EVT_LEFT_DOWN, self.left_down )
		self.display.Bind( wx.EVT_LEFT_UP, self.left_up )
		self.display.Bind( wx.EVT_MOTION, self.mouse_move )
		self.display.Bind( wx.EVT_MOUSEWHEEL, self.mwheel )
		self.display.Bind( wx.EVT_RIGHT_DOWN, self.right_down )
		self.display.Bind( wx.EVT_RIGHT_UP, self.right_up )
		self.Bind( wx.EVT_TIMER, self.end_timer, id=wx.ID_ANY )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def process_key( self, event ):
		event.Skip()

	def close( self, event ):
		event.Skip()

	def open_level( self, event ):
		event.Skip()

	def new_level( self, event ):
		event.Skip()

	def save_level( self, event ):
		event.Skip()

	def save_level_as( self, event ):
		event.Skip()

	def resize( self, event ):
		event.Skip()

	def change_level_text( self, event ):
		event.Skip()

	def change_tool( self, event ):
		event.Skip()

	def change_link( self, event ):
		event.Skip()

	def change_segment( self, event ):
		event.Skip()

	def change_offset( self, event ):
		event.Skip()

	def delete_link( self, event ):
		event.Skip()

	def left_down( self, event ):
		event.Skip()

	def left_up( self, event ):
		event.Skip()

	def mouse_move( self, event ):
		event.Skip()

	def mwheel( self, event ):
		event.Skip()

	def right_down( self, event ):
		event.Skip()

	def right_up( self, event ):
		event.Skip()

	def end_timer( self, event ):
		event.Skip()


###########################################################################
## Class LvlTxtDlg
###########################################################################

class LvlTxtDlg ( wx.Dialog ):

	def __init__( self, parent ):
		wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"Change level text", pos = wx.DefaultPosition, size = wx.Size( 318,137 ), style = wx.DEFAULT_DIALOG_STYLE )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

		v_sizer = wx.BoxSizer( wx.VERTICAL )

		self.txt = wx.StaticText( self, wx.ID_ANY, u"Type the text that will be displayed in game here :", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.txt.Wrap( -1 )

		v_sizer.Add( self.txt, 0, wx.ALL, 5 )

		self.txt_ctrl = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_CENTER|wx.TE_MULTILINE )
		v_sizer.Add( self.txt_ctrl, 1, wx.ALL|wx.EXPAND, 5 )

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


###########################################################################
## Class SizeDlg
###########################################################################

class SizeDlg ( wx.Dialog ):

	def __init__( self, parent ):
		wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"Choose dimentions", pos = wx.DefaultPosition, size = wx.Size( 220,145 ), style = wx.DEFAULT_DIALOG_STYLE )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

		v_sizer = wx.BoxSizer( wx.VERTICAL )

		h_sizer_height = wx.BoxSizer( wx.HORIZONTAL )

		self.height_text = wx.StaticText( self, wx.ID_ANY, u"Height :", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.height_text.Wrap( -1 )

		h_sizer_height.Add( self.height_text, 1, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )

		self.height_spin = wx.SpinCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.SP_ARROW_KEYS, 3, 20, 7 )
		h_sizer_height.Add( self.height_spin, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )


		v_sizer.Add( h_sizer_height, 0, wx.EXPAND, 5 )

		h_sizer_width = wx.BoxSizer( wx.HORIZONTAL )

		self.width_text = wx.StaticText( self, wx.ID_ANY, u"Width :", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.width_text.Wrap( -1 )

		h_sizer_width.Add( self.width_text, 1, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )

		self.width_spin = wx.SpinCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.SP_ARROW_KEYS, 3, 40, 7 )
		h_sizer_width.Add( self.width_spin, 0, wx.ALL, 5 )


		v_sizer.Add( h_sizer_width, 0, wx.EXPAND, 5 )

		self.confirm_button = wx.Button( self, wx.ID_ANY, u"Confirm", wx.DefaultPosition, wx.DefaultSize, 0 )
		v_sizer.Add( self.confirm_button, 0, wx.ALL|wx.ALIGN_RIGHT, 5 )


		self.SetSizer( v_sizer )
		self.Layout()

		self.Centre( wx.BOTH )

		# Connect Events
		self.Bind( wx.EVT_CLOSE, self.close )
		self.confirm_button.Bind( wx.EVT_BUTTON, self.confirm )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def close( self, event ):
		event.Skip()

	def confirm( self, event ):
		event.Skip()


###########################################################################
## Class CheckSaveDlg
###########################################################################

class CheckSaveDlg ( wx.Dialog ):

	def __init__( self, parent ):
		wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"Unsaved edits", pos = wx.DefaultPosition, size = wx.Size( -1,-1 ), style = wx.DEFAULT_DIALOG_STYLE )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

		v_sizer = wx.BoxSizer( wx.VERTICAL )

		self.text = wx.StaticText( self, wx.ID_ANY, u"You have unsaved edits, do you want to save?", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.text.Wrap( -1 )

		v_sizer.Add( self.text, 1, wx.ALL, 5 )

		h_sizer = wx.BoxSizer( wx.HORIZONTAL )

		self.yes_button = wx.Button( self, wx.ID_ANY, u"Yes", wx.DefaultPosition, wx.DefaultSize, 0 )
		h_sizer.Add( self.yes_button, 0, wx.ALL|wx.ALIGN_BOTTOM, 5 )

		self.cancel_button = wx.Button( self, wx.ID_ANY, u"Cancel", wx.DefaultPosition, wx.DefaultSize, 0 )
		h_sizer.Add( self.cancel_button, 0, wx.ALL|wx.ALIGN_BOTTOM, 5 )

		self.no_button = wx.Button( self, wx.ID_ANY, u"No", wx.DefaultPosition, wx.DefaultSize, 0 )
		h_sizer.Add( self.no_button, 0, wx.ALL|wx.ALIGN_BOTTOM, 5 )


		v_sizer.Add( h_sizer, 1, wx.EXPAND, 5 )


		self.SetSizer( v_sizer )
		self.Layout()
		v_sizer.Fit( self )

		self.Centre( wx.BOTH )

		# Connect Events
		self.Bind( wx.EVT_CLOSE, self.close )
		self.yes_button.Bind( wx.EVT_BUTTON, self.yes )
		self.cancel_button.Bind( wx.EVT_BUTTON, self.cancel )
		self.no_button.Bind( wx.EVT_BUTTON, self.no )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def close( self, event ):
		event.Skip()

	def yes( self, event ):
		event.Skip()

	def cancel( self, event ):
		event.Skip()

	def no( self, event ):
		event.Skip()


###########################################################################
## Class NameDlg
###########################################################################

class NameDlg ( wx.Dialog ):

	def __init__( self, parent ):
		wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"Choose a name for your level", pos = wx.DefaultPosition, size = wx.Size( -1,-1 ), style = wx.DEFAULT_DIALOG_STYLE )

		self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )

		v_sizer = wx.BoxSizer( wx.VERTICAL )

		h_sizer = wx.BoxSizer( wx.HORIZONTAL )

		self.text_ctrl = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, wx.TE_PROCESS_ENTER )
		h_sizer.Add( self.text_ctrl, 1, wx.ALL, 5 )

		self.ext_text = wx.StaticText( self, wx.ID_ANY, u".json", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.ext_text.Wrap( -1 )

		h_sizer.Add( self.ext_text, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )


		v_sizer.Add( h_sizer, 0, wx.EXPAND, 5 )

		self.help_text = wx.StaticText( self, wx.ID_ANY, u"Only levels named 'levelXXX' will be read by the game.", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.help_text.Wrap( -1 )

		v_sizer.Add( self.help_text, 0, wx.ALL, 5 )

		self.help_text2 = wx.StaticText( self, wx.ID_ANY, u"Save as 'blank_level' to replace default level.", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.help_text2.Wrap( -1 )

		v_sizer.Add( self.help_text2, 0, wx.ALL, 5 )

		self.confirm_button = wx.Button( self, wx.ID_ANY, u"Confirm", wx.DefaultPosition, wx.DefaultSize, 0 )
		v_sizer.Add( self.confirm_button, 0, wx.ALL|wx.ALIGN_RIGHT, 5 )


		self.SetSizer( v_sizer )
		self.Layout()
		v_sizer.Fit( self )

		self.Centre( wx.BOTH )

		# Connect Events
		self.text_ctrl.Bind( wx.EVT_TEXT_ENTER, self.confirm )
		self.confirm_button.Bind( wx.EVT_BUTTON, self.confirm )

	def __del__( self ):
		pass


	# Virtual event handlers, override them in your derived class
	def confirm( self, event ):
		event.Skip()



