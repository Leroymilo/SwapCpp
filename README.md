# Swap! C++ | How to Build / Edit levels

## Build game

### Windows

You need :
- the latest version of g++
- g++ in your system path
- make (to run the makefile)
- make in your system path

Run the command `make win` in your terminal.
You can now run swap.exe !

### Linux

disclaimer : I basically don't know what I'm doing on Linux, so please don't insult me. If you understand what I did, I won't refuse a lecture though.

You need :
- the latest version of g++
- make (I think) (to run the makefile)

All libs (SFML and jsonCpp) are included in the project's src. I know it shouldn't but it wouldn't work on my fedora without them.

## Use level editor

### Windows

You need :
- python 3.10.8 (wxPython does not work in 3.11 iIrc)

Run the following commands in this order :
- `python -m venv env`
- `.\env\Scripts\activate`
- `pip install -r requirements.txt`
- `python main.py` (this should launch the editor)

### Linux (good luck)

It's basically windows only because of the UI library wxPython. Theorically, wxPython is cross-platform, but `pip install wx` does not work on Linux, and some of the widgets are completely broken. So if you really want to edit levels on linux, good luck. Also, some of the pathing might be broken in the script itself (the old '/' vs '\\' issue), but you should be able to fix it.

You need :
- python 3.10.8 (wxPython does not work in 3.11 iIrc)

Run the following commands in this order :
- `python3 -m venv env`
- `source ./env/bin/activate`
- `pip install -r requirements.txt` (this will not fully work because of wxPython)
- `python3 main.py` (this should launch the editor)

### Edit levels without python

If you want to make an executable instead of having to run a python script, you might want to look into [pyinstaller](https://pyinstaller.org/en/stable/).