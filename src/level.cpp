#include <iostream>
#include <fstream>
#include <iomanip>

#include <json/value.h>
#include <json/json.h>

#include "gameplay/level.hpp"


std::string make_level_name(int nb)
{
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(3) << nb;
    return "level" + ss.str();
}


//Constructor and meta
Level::Level(){}

Level::Level(std::string file_name, sf::Font font) : font(font)
{
    // Reading Json :
    std::ifstream file("levels/" + file_name + ".json");
    Json::Reader reader;
    Json::Value json_data;
    reader.parse(file, json_data);

    // Loading tiles sprites :
    bg_tiles['X'].loadFromFile("assets/Tiles/Wall.png");
    bg_tiles['x'].loadFromFile("assets/Tiles/Grate.png");
    bg_tiles['.'].loadFromFile("assets/Tiles/Floor.png");
    bg_tiles['W'].loadFromFile("assets/Tiles/Win.png");

    // Making objects :
    win_tile = backGround.create(json_data["bg"], bg_tiles);
    Player = PlayerLike(json_data["entities"]["player"], "player");
    ghost = PlayerLike(json_data["entities"]["bullet"], "bullet");
    boxes = Boxes(json_data["entities"]["nbBoxes"].asInt(), json_data["entities"]["Boxes"]);
    logic = Logic(json_data["logic"]);

    // Loading text :
    if (json_data.isMember("hint"))
    {
        hint_lines = json_data["hint"].size();
        hint.resize(hint_lines);
        for (int i = 0; i < hint_lines; i++)
        {
            hint[i] = json_data["hint"][i].asCString();
        }
    }
    if (json_data.isMember("dialogue"))
    {
        dlg_lines = json_data["dialogue"].size();
        dlg.resize(dlg_lines);
        for (int i = 0; i < dlg_lines; i++)
        {
            dlg[i] = json_data["dialogue"][i].asCString();
        }
    }

    // Loading stuff :
    name = json_data["name"].asCString();
    Json::Value json_flags = json_data["flags"];
    int i = 0;
    for (Json::Value::iterator itr = json_flags.begin(); itr != json_flags.end(); itr++)
    {
        std::string key = json_flags.getMemberNames()[i];
        flags[key] = json_flags[key].asBool();
        flag_icons[key].loadFromFile("assets/" + key + ".png");
        i ++;
    }
    perf_steps = json_data["perf_steps"].asInt();
    ordered_flags = {"has_ghost", "can_swap"};

    process_logic(false);
    validate_step();
}

Level::Level(int number, bool solved, sf::Font font) : Level(make_level_name(number), font)
{
    this->solved = solved;
}

std::string Level::get_pLike_state()
{
    std::string res = "";
    res.append(std::to_string(int(Player.alive)));
    res.push_back(Player.dir);
    res.append(std::to_string(int(ghost.alive)));
    res.push_back(ghost.dir);
    return res;
}


//Gameplay
bool Level::isWallForMost(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X' or backGround.getTile(coords) == 'x')
        return true;
    return logic.isWallForMost(coords);
}

bool Level::isWallForGhost(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X')
        return true;
    bool hasBox;
    boxes.get_box(coords, &hasBox);
    return hasBox || logic.isWallForGhost(coords);
}

bool Level::push(char direction)
{
    if (!Player.alive)
        return false;

    bool STEP = !(Player.dir == direction);
    Player.dir = direction;

    sf::Vector2i newCoords = Player.get_next_pos();

    //Checking if there's a wall blocking :
    bool isBlocked = isWallForMost(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity* boxP = boxes.get_box(newCoords, &boxBlocked);
        if (boxBlocked)
        {
            isBlocked = boxPush(boxP, direction);
        }
    }

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
    {
        Player.pos = newCoords;
        return true;
    }
    return STEP;
}

bool Level::boxPush(Entity* pusher, char direction)
{
    sf::Vector2i newCoords = pusher->get_next_pos(direction);
    //Checking if there's a wall blocking :
    bool isBlocked = isWallForMost(newCoords);

    //Checking if there's a box blocking :
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity* boxP = boxes.get_box(newCoords, &boxBlocked);
        if (boxBlocked)
            isBlocked = boxPush(boxP, direction);
    }

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
        pusher->pos = newCoords;

    //Pushing a PlayerLike if it's in front :
    if (ghost.alive && ghost.pos == newCoords)
        pLikePush(&ghost, direction);
    
    return isBlocked;
}

void Level::pLikePush(PlayerLike* pushed, char direction)
{
    sf::Vector2i newCoords = pushed->get_next_pos(direction);
    pushed->pos = newCoords;
    if (!isWallForGhost(newCoords))
        pushed->dir = direction;
    //It's pushed anyway.
}

bool Level::swap()
{
    if (!flags["can_swap"]) return false;

    if (Player.alive && !ghost.alive)
    {
        if (isWallForGhost(Player.get_next_pos()))
            return false;
        ghost.alive = true;
        ghost.pos = Player.get_next_pos();
        ghost.dir = Player.dir;
    }

    else if (Player.alive && ghost.alive)
    {
        if (isWallForMost(ghost.pos))
            return false;
        sf::Vector2i tempC = Player.pos;
        char tempdir = Player.dir;
        Player.pos = ghost.pos;
        ghost.pos = tempC;
        Player.dir = ghost.dir;
        ghost.dir = tempdir;
    }

    else if (!Player.alive && ghost.alive)
    {
        if (isWallForMost(ghost.pos))
            return false;
        Player.alive = true;
        ghost.alive = false;
        Player.pos = ghost.pos;
        Player.dir = ghost.dir;
    }

    else 
        return false;
    
    return true;
}

bool Level::wait()
{
    if (ghost.alive)
    {
        return true;
    }
    return false;
}

void Level::process_logic(bool didSwap)
{
    std::list<sf::Vector2i> heavy_pos = boxes.get_boxes_pos();
    if (Player.alive)
        heavy_pos.push_back(Player.pos);

    std::list<sf::Vector2i> ghost_pos;
    if (ghost.alive)
        ghost_pos.push_back(ghost.pos);

    
    std::list<sf::Vector2i> updated_activators = logic.update_activators(heavy_pos, ghost_pos, didSwap, &ghost.alive);
    logic.update(updated_activators);
}

void Level::step(bool didSwap)
{
    //Ghost crushing and movement :
    ghost.alive = !isWallForGhost(ghost.pos) && ghost.alive;
    if (ghost.alive && !didSwap)
    {
        sf::Vector2i newC = ghost.get_next_pos();

        if (isWallForGhost(newC))
        {
            ghost.revert();
            newC = ghost.get_next_pos();
        }

        if (isWallForGhost(newC))
            ghost.revert();
        else
            ghost.pos = newC;
    }

    //Checking if player get shot :
    if (ghost.alive && Player.alive)
    {
        Player.alive = Player.pos != ghost.pos;
        if (Player.get_prev_pos() == ghost.pos && Player.pos == ghost.get_prev_pos() && !didSwap)
        {
            // Player crossed ghost
            Player.alive = false;
        }
    }

    process_logic(didSwap);

    //Checking if player and/or ghost get crushed :
    Player.alive = !isWallForMost(Player.pos) && Player.alive;
    ghost.alive = !isWallForGhost(ghost.pos) && ghost.alive;

    //Same for boxes :
    std::vector<bool> to_destroy;
    for (sf::Vector2i coords : boxes.get_boxes_pos())
    {
        to_destroy.push_back(isWallForMost(coords));
    }
    boxes.destroy(to_destroy);

    nbSteps++;
}

void Level::undo(std::list<char>* steps)
{
    steps->pop_back();

    Player.undo();
    ghost.undo();
    boxes.undo();
    logic.undo();

    nbSteps--;
}

void Level::validate_step()
{
    Player.validate_step();
    ghost.validate_step();
    boxes.validate_step();
    logic.validate_step();

    if (Player.alive && Player.pos == win_tile)
        won = true;
}


//Display
void Level::resize_bg(sf::RenderWindow * windowP)   //Should get called every time the game's window is resized
{
    sf::Vector2f winSize(windowP->getSize());

    int Y0 = 0;
    sf::FloatRect bounds;

    // bounds = sf::Text(name, font, 30).getLocalBounds();
    // Y0 += bounds.height + bounds.top;
    // winSize.y -= Y0;

    bounds = sf::Text("Ip", font, 28).getLocalBounds();
    winSize.y -= bounds.height + bounds.top;
    // A literal string is used because the number of steps changing makes the whole text bump around if used.

    for (int i = hint_lines-1; i >= 0; i--)
    {
        sf::FloatRect bounds = sf::Text(hint[i], font, 24).getLocalBounds();
        winSize.y -= bounds.height + bounds.top;
    }
    for (int i = dlg_lines-1; i >= 0; i--)
    {
        sf::FloatRect bounds = sf::Text(dlg[i], font, 24).getLocalBounds();
        winSize.y -= bounds.height + bounds.top;
    }

    backGround.resize(winSize, Y0);
}

void Level::displayBG(sf::RenderWindow * windowP)
{
    windowP->clear(sf::Color(20, 20, 25));

    sf::Vector2u winSize = windowP->getSize();
    float max_y = winSize.y;
    sf::FloatRect bounds;

    // Displaying level name :
    // sf::Text nameDisp(name, font, 30);
    // bounds = nameDisp.getLocalBounds();
    // nameDisp.setPosition((winSize.x-bounds.width-bounds.left)/2, 0);
    // windowP->draw(nameDisp);

    // Displaying level background :
    backGround.display(windowP, font);

    // Displaying number of steps :
    std::string step_text = "step : " + std::to_string(nbSteps);
    if (solved) {step_text += "/" + std::to_string(perf_steps);}
    sf::Text stepDisp(step_text, font, 28);
    bounds = stepDisp.getLocalBounds();
    stepDisp.setPosition((winSize.x-bounds.width-bounds.left)/2, max_y-bounds.height-bounds.top);
    windowP->draw(stepDisp);
    bounds = sf::Text("Ip", font, 28).getLocalBounds();
    max_y -= bounds.height + bounds.top;
    // This is because stepDisp changes height when the step number changes, so I put something that only depends on the font used.

    // Displaying level text :
    for (int i = hint_lines-1; i >= 0; i--)
    {
        sf::Text helpDisp(hint[i], font, 24);
        helpDisp.setStyle(sf::Text::Italic);
        bounds = helpDisp.getLocalBounds();
        helpDisp.setPosition((winSize.x-bounds.width-bounds.left)/2, max_y-bounds.height-bounds.top);
        windowP->draw(helpDisp);
        max_y -= bounds.height + bounds.top;
    }
    for (int i = dlg_lines-1; i >= 0; i--)
    {
        sf::Text helpDisp(dlg[i], font, 24);
        helpDisp.setStyle(sf::Text::Bold);
        bounds = helpDisp.getLocalBounds();
        helpDisp.setPosition((winSize.x-bounds.width-bounds.left)/2, max_y-bounds.height-bounds.top);
        windowP->draw(helpDisp);
        max_y -= bounds.height + bounds.top;
    }

    // Displaying flags :
    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);
    delta = delta/2;
    C0.y -= delta;
    sf::RectangleShape tile(sf::Vector2f(delta, delta));

    for (std::string flag_key : ordered_flags)
    {
        if (flags[flag_key])
        {
            tile.setTexture(&flag_icons[flag_key]);
            tile.setPosition(sf::Vector2f(C0));
            windowP->draw(tile);
            C0.x += delta;
        }
    }
}

void Level::animate(sf::RenderWindow * windowP, int frame, bool disp)
{
    displayBG(windowP);

    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);

    logic.anim(C0, delta, windowP, frame);

    Player.anim(C0, delta, windowP, frame);

    ghost.anim(C0, delta, windowP, frame);
    
    boxes.anim(C0, delta, windowP, frame);

    if (disp)
        windowP->display();
}

void Level::display(sf::RenderWindow * windowP, bool disp)
{
    animate(windowP, 4, disp);
}

// Handling pause menu
int pause(Level* levelP, sf::RenderWindow* windowP, sf::Font font)
{
    Button continue_("continue", "Continue", Alignment(1, 0, 0, 3, 2, 0), windowP);
    Button exit_("exit", "Exit", Alignment(1, 0, 0, 3, 0, 0), windowP);
    
    // First draw :

    levelP->display(windowP, false);
    continue_.draw(font);
    exit_.draw(font);
    windowP->display();

    while (windowP->isOpen())
    {
        sf::Event evnt;
        if (windowP->pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                windowP->close();
                return 0;
            }

            else if (evnt.type == sf::Event::GainedFocus)
            {
                levelP->display(windowP, false);
                continue_.draw(font);
                exit_.draw(font);
                windowP->display();
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                windowP->setView(sf::View(view));
                levelP->resize_bg(windowP);
                levelP->display(windowP, false);
                continue_.reshape();
                exit_.reshape();
                continue_.draw(font);
                exit_.draw(font);
                windowP->display();
            }

            else if (evnt.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    return 1;
                }
            }
        }

        if (continue_.clicked())
        {
            return 1;
        }
        else if (exit_.clicked())
        {
            return 0;
        }

        if (continue_.update() || exit_.update())
        {
            levelP->display(windowP, false);
            continue_.draw(font);
            exit_.draw(font);
            windowP->display();
        }

    }

    return 0;
}

// Main gameplay loop with keyboard input handling
int run(int level_id, bool solved, sf::RenderWindow* windowP, sf::Font font, int* nb_steps)
{
    //SFML stuff and level initialization :
    sf::Clock clock;
    Level level(level_id, solved, font);
    level.resize_bg(windowP);

    //First draw :
    windowP->clear(sf::Color(0, 0, 120));
    level.display(windowP);
    
    //Input handling :
    int time = clock.getElapsedTime().asMilliseconds();
    int long_deltaT = 200;
    int short_deltaT = 160;
    bool long_press = false;
    bool process_input = false;
    int nbKeys = 8;
    sf::Keyboard::Key keys[] = {sf::Keyboard::Key::Up, sf::Keyboard::Key::Right, sf::Keyboard::Key::Down, sf::Keyboard::Key::Left, 
    sf::Keyboard::Key::Space, sf::Keyboard::Key::BackSpace, sf::Keyboard::Key::Return, sf::Keyboard::Key::Add};
    std::string keysNames[] = {"Up", "Right", "Down", "Left", "Swap", "Undo", "Wait", "Restart"};//Unused, just a description of the actions.
    bool keysStates[] = {false, false, false, false, false, false, false, false};
    int pkey = nbKeys;

    //Gameplay variables :
    bool step = false;
    bool didSwap = false;
    std::string directions = "URDL";
    std::list<char> steps;
    //A step can be : U, R, D, L, H (shot), P (swap), W or + (reset)
    std::list<Level> pre_resets;
    pre_resets.push_back(level);

    //Animation handling :
    bool animating = false;
    int anim_deltaT = 30;
    int anim_start = 0;
    int frame = 0;

    while (windowP->isOpen())
    {
        sf::Event evnt;
        if (windowP->pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                windowP->close();
                return 0;
            }

            else if (evnt.type == sf::Event::GainedFocus)
            {
                level.display(windowP);
            }
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                windowP->setView(sf::View(view));
                level.resize_bg(windowP);
                level.display(windowP);
            }

            else if (evnt.type == sf::Event::KeyPressed)
            {
                for (int i = 0; i < nbKeys; i++)
                {
                    if (sf::Keyboard::isKeyPressed(keys[i]))
                        keysStates[i] = true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    int res = pause(&level, windowP, font);

                    if (res == 0)
                    {
                        return 0;
                    }
                    level.display(windowP);
                }
            }
            else if (evnt.type == sf::Event::KeyReleased)
            {
                for (int i = 0; i < nbKeys; i++)
                {
                    if (!sf::Keyboard::isKeyPressed(keys[i]))
                        keysStates[i] = false;
                }
            }
        }

    //Manage input delay and changed keys :
        process_input = false;

        //Key was kept pressed :
        if (pkey!=nbKeys && keysStates[pkey])
        {
            if (!long_press && clock.getElapsedTime().asMilliseconds() >= time + long_deltaT)
            {
                process_input = true;
                long_press = true;
                time = clock.getElapsedTime().asMilliseconds();
            }

            else if (long_press && clock.getElapsedTime().asMilliseconds() >= time + short_deltaT)
            {
                process_input = true;
                time = clock.getElapsedTime().asMilliseconds();
            }
        }

        //Key was unpressed :
        else
        {
            pkey = nbKeys;
            long_press = false;
            for(int i = 0; i < nbKeys; i++)
            {
                if (keysStates[i])
                {
                    pkey = i;
                    process_input = true;
                    time = clock.getElapsedTime().asMilliseconds();
                    break;
                }
            }
        }
        
    //Apply inputs :
        if (pkey!=nbKeys && process_input && !level.won)
        {
            int t = clock.getElapsedTime().asMilliseconds();
            step = false;
            didSwap = false;

            if (pkey < 4)
            {
                step = level.push(directions[pkey]);
                if (step) steps.push_back(directions[pkey]);
            }

            else if (pkey == 4)
            {
                step = level.swap();
                if (step)
                {
                    steps.push_back('P');
                    didSwap = true;
                }
            }

            else if (pkey == 5)
            {
                if (steps.size() > 0)
                {
                    if (steps.back() == '+')
                    {
                        level = pre_resets.back();
                        pre_resets.pop_back();
                        steps.pop_back();
                    }
                    else
                        level.undo(&steps);
                }
            }

            else if (pkey == 6)
            {
                step = level.wait();
                steps.push_back('W');
            }

            else if (pkey == 7)
            {
                if (level.nbSteps > 0)
                {
                    pre_resets.push_back(level);
                    level = pre_resets.front();
                    level.process_logic(false);
                    level.resize_bg(windowP);
                    steps.push_back('+');
                }
            }


            if (step)
            {
                level.step(didSwap);
                level.validate_step();

                animating = true;
                anim_start = clock.getElapsedTime().asMilliseconds();
                frame = 0;
            }
            else
                level.display(windowP);
        }

        // handling of animation between steps :
        int cur_time = clock.getElapsedTime().asMilliseconds();
        if (animating && cur_time >= anim_start + frame * anim_deltaT)
        {
            frame++;
            level.animate(windowP, frame);
            if (frame == 4)
            {
                frame = 0;
                animating = false;
            }

            /*
            There are 4 frames in a step, numbered from 1 to 4.
            Passing frame=0 in the animate function will draw the previous state,
            passing frame=4 will draw the current state, which is also the last draw of the animation.

            When making in-step animated sprite (animation plays during the step transition),
            the left-most column of the sprite sheet should be the "standing" one, while the 3 other columns make a cycle.

            For elements moving during the in-step animation,
            the drawing position is calculated by linear interpolation between frame 0 and 4,
            making frames 1, 2 and 3 respectively at exactly 25%, 50% and 75% of a step.
            */
        }

        if (frame == 0 && level.won)
        {
            *nb_steps = level.nbSteps;
            if (level.nbSteps > level.perf_steps)
                return 1;
            else return 2;
        }
    }

    return 0;
}