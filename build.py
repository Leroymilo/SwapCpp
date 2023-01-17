from os.path import exists
from os import system
from shutil import copytree, copy
import sys

os = input("OS? (lin/win)")
if os == "win" :
    if not exists("swap.exe") or exists("swap.out") :
        system("make clear")
        system("make win")
elif os == "lin" :
    if not exists("swap.out") or exists("swap.exe") :
        system("make clear")
        system("make lin")
else :
    print("unknown OS :(")
    sys.exit()

if exists("build") :
    system("rm -r build")
system("mkdir build")

system("mkdir build/saves")
copy("saves/blank_save.json", "build/saves/blank_save.json")

system("mkdir build/src")
copytree("src/include", "build/src/include")
copytree("src/lib", "build/src/lib")

copytree("assets", "build/assets")
copytree("levels", "build/levels")
copytree("objects", "build/objects")

if os == "win" :
    copy("swap.exe", "build/swap.exe")
else :
    copy("swap.out", "build/swap.out")