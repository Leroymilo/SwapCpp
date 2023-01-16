import pygame as pg

pg.init()

dirs = ['U', 'R', 'D', 'L']

delta = 32
size = (delta, delta)

def get_image(path: str, n_w: int = 1, n_h: int = 1) :
    image = pg.image.load(path)
    W, H = image.get_size()
    w, h = W//n_w, H//n_h

    if (n_w, n_h) != (1, 1) :
        image = image.subsurface(0, 0, w, h)
    
    if (w, h) != size :
        image = pg.transform.scale(image, size)

    return image

floor   = get_image("assets/Tiles/Floor.png")
wall    = get_image("assets/Tiles/Wall.png")
grate   = get_image("assets/Tiles/Grate.png")
goal    = get_image("assets/Tiles/Win.png")

box     = get_image("assets/Entities/Box.png")
player  = get_image("assets/Entities/player.png", 1, 4)
bullet  = get_image("assets/Entities/bullet.png", 1, 4)

interruptor = get_image("assets/Logic/Interruptor0.png")
target      = get_image("assets/Logic/Target0.png")
door_tile   = get_image("assets/Logic/Door0.png")
AND         = get_image("assets/Logic/AND0.png")
OR          = get_image("assets/Logic/OR0.png")
NOT         = get_image("assets/Logic/NO0.png")

door_hub    = get_image("assets/door_hub.png")