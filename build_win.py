from subprocess import run
from os.path import exists
from shutil import copytree, copy
import sys

os = input("OS? (lin/win)")
if os == "win" :
    if not exists("swap.exe") or exists("swap.out") :
        run("make clear")
        run("make win")
elif os == "lin" :
    if not exists("swap.out") or exists("swap.exe") :
        run("make clear")
        run("make lin")
else :
    print("unknown OS :(")
    sys.exit()

if exists("build") :
    run("rm -r build")
run("mkdir build")

run("mkdir build/saves")
copy("saves/blank_save.json", "build/saves/blank_save.json")

run("mkdir build/src")
copytree("src/include", "build/src/include")
copytree("src/lib", "build/src/lib")

copytree("assets", "build/assets")
copytree("levels", "build/levels")
copytree("objects", "build/objects")

if os == "win" :
    copy("swap.exe", "build/swap.exe")
else :
    copy("swap.out", "build/swap.out")