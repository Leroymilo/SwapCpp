import pygame as pg

pg.init()

dirs = ['U', 'R', 'D', 'L']
orients = ['N', 'H', 'V']

delta = 64
size = (delta, delta)

def get_image(path: str, n_w: int = 1, n_h: int = 1) :
    image = pg.image.load(path)
    W, H = image.get_size()
    w, h = W//n_w, H//n_h
    
    if (w, h) != size :
        image = pg.transform.scale(image, (delta*n_w, delta*n_h))
    
    if n_h == 1 and n_w == 1 :
        return image
    
    elif n_w == 8 : # player
        dict_im = [{}, {}]
        for alive in {0, 1} :
            for i in range(4) :
                dict_im[alive][dirs[i]] = image.subsurface(alive * 4 * delta, i * delta, delta, delta)
        return dict_im

    elif n_w == 5 and n_h == 1 : # statue and activators
        dict_im = [None, None]
        for alive in {0, 1} :
            dict_im[alive] = image.subsurface(alive * 4 * delta, 0, delta, delta)
        return dict_im

    elif n_h == 4 : # gates
        dict_im = {}
        for i in range(4) :
            dict_im[dirs[i]] = image.subsurface(0, i*delta, delta, delta)
        return dict_im
    
    elif n_h == 3 : # Doors
        dict_im = {}
        for i in range(3) :
            dict_im[orients[i]] = image.subsurface(0, i*delta, delta, delta)
        return dict_im
    
    else :
        return image.subsurface(0, 0, delta, delta)


floor   = get_image("assets/Tiles/Floor.png")
wall    = get_image("assets/Tiles/Wall.png")
thorns   = get_image("assets/Tiles/Thorns.png")
goal    = get_image("assets/Tiles/Win.png")

statue     = get_image("assets/Entities/statue.png", 5, 1)
player  = get_image("assets/Entities/player_physf.png", 8, 4)
ghost  = get_image("assets/Entities/player_ghost.png", 8, 4)

interruptor = get_image("assets/Logic/Button.png", 5, 1)
target      = get_image("assets/Logic/Target.png", 5, 1)
door_tile   = get_image("assets/Logic/Door.png", 5, 3)
AND         = get_image("assets/Logic/AND.png", 5, 4)
OR          = get_image("assets/Logic/OR.png", 5, 4)
NO          = get_image("assets/Logic/NO.png", 5, 4)
gates = {"AND": AND, "OR": OR, "NO": NO}

door_hub    = get_image("assets/door_hub.png")


def for_tool_bar(image: pg.Surface) :
    return pg.transform.scale(image, (16, 16))

sprites = {
    "Wall"      : wall,
    "Thorns"     : thorns,
    "Goal"      : goal,
    "Player"    : player[1]["U"],
    "Ghost"    : ghost[1]["U"],
    "Statue"    : statue[1],
    "Button"    : interruptor[0],
    "Target"    : target[0],
    "AND Gate"  : AND["U"],
    "OR Gate"   : OR["U"],
    "NO Gate"   : NO["U"],
    "Door Tile" : door_tile["H"],
    "Door Hub"  : door_hub,
}

sprites = {key: for_tool_bar(image) for key, image in sprites.items()}

sprites |= {"Connector" : "assets/connector.png"}