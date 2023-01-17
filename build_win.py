from subprocess import run
from os.path import exists
from shutil import copytree, copy

if not exists("swap.exe") :
    run("make clear")
    run("make win")

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
copy("swap.exe", "build/swap.exe")