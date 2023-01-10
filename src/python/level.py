import json

class Level :
    def __init__(self, dir_) -> None:
        data = json.load(open(dir_))