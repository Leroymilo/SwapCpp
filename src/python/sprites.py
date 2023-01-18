import pygame as pg

pg.init()

dirs = ['U', 'R', 'D', 'L']

delta = 32
size = (delta, delta)

def get_image(path: str, n_w: int = 1, n_h: int = 1) :
    image = pg.image.load(path)
    W, H = image.get_size()
    w, h = W//n_w, H//n_h

    if n_w > 1 :
        image = image.subsurface(0, 0, w, H)
    
    if (w, h) != size :
        image = pg.transform.scale(image, (delta, delta*n_h))
    
    if n_h == 1 :
        return image
    
    elif n_h == 4 :
        dict_im = {}
        for i in range(4) :
            dict_im[dirs[i]] = image.subsurface(0, i*delta, delta, delta)
        return dict_im

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
AND         = get_image("assets/Logic/AND.png", 5, 4)
OR          = get_image("assets/Logic/OR.png", 5, 4)
NO          = get_image("assets/Logic/NO.png", 5, 4)
gates = {"AND": AND, "OR": OR, "NO": NO}

door_hub    = get_image("assets/door_hub.png")


def for_tool_bar(image: pg.Surface) :
    return pg.transform.scale(image, (16, 16))

sprites = {
    "Wall"      : wall,
    "Grate"     : grate,
    "Goal"      : goal,
    "Player"    : player["U"],
    "Bullet"    : bullet["U"],
    "Box"       : box,
    "Button"    : interruptor,
    "Target"    : target,
    "AND Gate"  : AND["U"],
    "OR Gate"   : OR["U"],
    "NO Gate"   : NO["U"],
    "Door Tile" : door_tile,
    "Door Hub"  : door_hub,
}

sprites = {key: for_tool_bar(image) for key, image in sprites.items()}

sprites |= {"Connector" : "assets/connector.png"}