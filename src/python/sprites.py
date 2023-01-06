import pygame as pg

pg.init()

dirs = ['U', 'R', 'D', 'L']

delta = 32
size = (delta, delta)

def get_image(path: str) :
    return pg.transform.scale(pg.image.load(path), size)

floor   = get_image("assets/Tiles/Floor.png")
wall    = get_image("assets/Tiles/Wall.png")
grate   = get_image("assets/Tiles/Grate.png")
goal    = get_image("assets/Tiles/Win.png")

box     = get_image("assets/Entities/Box.png")
player  = {dir_: get_image(f"assets/Entities/player{dir_}.png") for dir_ in dirs}
bullet  = {dir_: get_image(f"assets/Entities/bullet{dir_}.png") for dir_ in dirs}

interruptor = get_image("assets/Logic/Interruptor0.png")
target      = get_image("assets/Logic/Target0.png")
door        = get_image("assets/Logic/Door0.png")
AND         = get_image("assets/Logic/AND0.png")
OR          = get_image("assets/Logic/OR0.png")
NOT         = get_image("assets/Logic/NO0.png")