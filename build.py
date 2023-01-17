from os.path import exists
from subprocess import run
from os import system
from shutil import copytree, copy
from sys import exit

cmd = {"lin": system, "win": run}
exec = {"lin": "swap.out", "win": "swap.exe"}
bad_exec = {"lin": "swap.exe", "win": "swap.out"}

os = input("OS? (lin/win)")
if os in {"win", "lin"} :
    if not exists(exec[os]) or exists(bad_exec[os]) :
        cmd[os]("make clear")
        cmd[os]("make " + os)
else :
    print("unknown OS :(")
    exit()

if exists("build") :
    cmd[os]("rm -r build")
cmd[os]("mkdir build")

cmd[os]("mkdir build/saves")
copy("saves/blank_save.json", "build/saves/blank_save.json")

cmd[os]("mkdir build/src")
copytree("src/include", "build/src/include")
copytree("src/lib", "build/src/lib")

copytree("assets", "build/assets")
copytree("levels", "build/levels")
copytree("objects", "build/objects")

if os == "win" :
    copy("swap.exe", "build/swap.exe")
else :
    copy("swap.out", "build/swap.out")