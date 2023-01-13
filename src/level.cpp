#include <iostream>
#include <fstream>
#include <iomanip>

#include <json/value.h>
#include <json/json.h>

#include "gameplay/level.hpp"


//Constructor and meta
Level::Level(){}

Level::Level(int number)
{
    // Level file name :
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(3) << number;
    std::string const str = "levels/level" + ss.str() + ".json";

    // Reading Json :
    std::ifstream file(str);
    Json::Reader reader;
    Json::Value json_data;
    reader.parse(file, json_data);

    // Making objects :
    win_tile = backGround.readJson(json_data["bg"]);
    Player = PlayerLike(json_data["entities"]["player"], "player");
    Player.is_alive = json_data["entities"]["player"]["alive"].asBool();
    bullet = PlayerLike(json_data["entities"]["bullet"], "bullet");
    bullet.is_alive = json_data["entities"]["bullet"]["alive"].asBool();
    boxes = Boxes(json_data["entities"]["nbBoxes"].asInt(), json_data["entities"]["Boxes"]);
    logic = Logic(json_data["logic"]);

    // Loading tiles sprites :
    bg_tiles['X'].loadFromFile("assets/Tiles/Wall.png");
    bg_tiles['x'].loadFromFile("assets/Tiles/Grate.png");
    bg_tiles['.'].loadFromFile("assets/Tiles/Floor.png");
    bg_tiles['W'].loadFromFile("assets/Tiles/Win.png");
}

std::string Level::get_pLike_state()
{
    std::string res = "";
    res.append(std::to_string(int(Player.is_alive)));
    res.push_back(Player.dir);
    res.append(std::to_string(int(bullet.is_alive)));
    res.push_back(bullet.dir);
    return res;
}


//Gameplay
bool Level::isWallForMost(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X' or backGround.getTile(coords) == 'x')
        return true;
    return logic.isWallForMost(coords);
}

bool Level::isWallForBullet(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X')
        return true;
    bool hasBox;
    boxes.get_box(coords, &hasBox);
    return hasBox || logic.isWallForBullet(coords);
}

bool Level::push(char direction, std::string* act)
{
    if (!Player.is_alive)
        return false;

    bool STEP = !(Player.dir == direction);
    Player.dir = direction;

    sf::Vector2i newCoords = Player.get_next_pos();

    //Checking if there's a wall blocking :
    bool isBlocked = isWallForMost(newCoords);

    //Checking if there's a box blocking :
    int nb_pushed = 0;
    if (!isBlocked)
    {
        bool boxBlocked;
        Entity* boxP = boxes.get_box(newCoords, &boxBlocked);
        if (boxBlocked)
        {
            isBlocked = boxPush(boxP, direction, &nb_pushed);
        }
    }
    act->append(std::to_string(nb_pushed));

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
    {
        Player.C = newCoords;
        if (newCoords == win_tile)
            won = true;
        return true;
    }
    return STEP;
}

bool Level::boxPush(Entity* pusher, char direction, int* nb_pushed)
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
            isBlocked = boxPush(boxP, direction, nb_pushed);
    }

    //Updating the coords if nothing is blocking:
    if (!isBlocked)
    {
        pusher->C = newCoords;
        (*nb_pushed)++;
    }

    //Pushing a PlayerLike if it's in front :
    if (bullet.is_alive && bullet.C == newCoords)
    {
        pLikePush(&bullet, direction);
        (*nb_pushed)++;
    }
    return isBlocked;
}

void Level::pLikePush(PlayerLike* pushed, char direction)
{
    sf::Vector2i newCoords = pushed->get_next_pos(direction);
    pushed->C = newCoords;
    if (!isWallForBullet(newCoords))
        pushed->dir = direction;
    //It's pushed anyway.
}

bool Level::swap(std::string* act)
{
    if (Player.is_alive && !bullet.is_alive)
    {
        if (isWallForBullet(Player.get_next_pos()))
            return false;
        bullet.is_alive = true;
        bullet.C = Player.get_next_pos();
        bullet.dir = Player.dir;
        act->push_back('H');
    }
    else if (Player.is_alive && bullet.is_alive)
    {
        if (isWallForMost(bullet.C))
            return false;
        sf::Vector2i tempC = Player.C;
        char tempdir = Player.dir;
        Player.C = bullet.C;
        bullet.C = tempC;
        Player.dir = bullet.dir;
        bullet.dir = tempdir;
        act->push_back('P');
        act->push_back(Player.dir);
    }
    else if (!Player.is_alive && bullet.is_alive)
    {
        if (isWallForMost(bullet.C))
            return false;
        Player.is_alive = true;
        bullet.is_alive = false;
        Player.C = bullet.C;
        Player.dir = bullet.dir;
        act->push_back('P');
        act->push_back(Player.dir);
    }
    else 
        return false;
    return true;
}

bool Level::wait()
{
    if (bullet.is_alive)
    {
        return true;
    }
    return false;
}

void Level::step(bool didSwap)
{
    //Checking if player and/or bullet get "killed" :
    Player.is_alive = !((bullet.is_alive && Player.C == bullet.C) || isWallForMost(Player.C)) && Player.is_alive;
    bullet.is_alive = !isWallForBullet(bullet.C) && bullet.is_alive;

    //Same for boxes :
    std::vector<bool> to_destroy;
    for (sf::Vector2i coords : boxes.get_boxes_pos())
        to_destroy.push_back(isWallForMost(coords));
    boxes.destroy(to_destroy);

    //Moving bullet :
    if (bullet.is_alive && !didSwap)
    {
        sf::Vector2i newC = bullet.get_next_pos();

        if (isWallForBullet(newC))
        {
            bullet.revert();
            newC = bullet.get_next_pos();
        }

        if (!isWallForBullet(newC))
            bullet.C = newC;
    }

    //Updating logic :
    std::vector<sf::Vector2i> heavy_pos = boxes.get_boxes_pos();
    if (Player.is_alive)
        heavy_pos.push_back(Player.C);

    std::vector<sf::Vector2i> arrow_pos;
    if (bullet.is_alive)
        arrow_pos.push_back(bullet.C);

    
    std::vector<sf::Vector2i> updated_activators = logic.update_activators(heavy_pos, arrow_pos, didSwap, &bullet.is_alive);
    logic.update(updated_activators);

    //Checking if player and/or bullet get "killed" :
    Player.is_alive = !((bullet.is_alive && Player.C == bullet.C) || isWallForMost(Player.C)) && Player.is_alive;
    bullet.is_alive = !isWallForBullet(bullet.C) && bullet.is_alive;

    //Same for boxes :
    to_destroy.clear();
    for (sf::Vector2i coords : boxes.get_boxes_pos())
        to_destroy.push_back(isWallForMost(coords));
    boxes.destroy(to_destroy);

    nbSteps++;
}

void Level::undo(std::list<std::string>* steps)
{
    std::string directions = "URDL";

    std::string last_move = steps->back();
    steps->pop_back();

    Player.is_alive = last_move[0]=='1';
    bullet.is_alive = last_move[2]=='1';
    boxes.undo();

    if (last_move[4] == 'H'){}

    else if (last_move[4] == 'P'){}

    else if (last_move[4] == 'W'){}

    else
    {
        int nb_pushed = std::stoi(last_move.substr(std::size_t(5), last_move.size()));
        
        //Pulling boxes and PlayerLikes :
        for (int i = 0; i < nb_pushed; i++)
        {
            sf::Vector2i box_pos = Player.get_next_pos();
            bool has_box;
            Entity* box = boxes.get_box(box_pos, &has_box);
            if (has_box && !(i == nb_pushed-1 && bullet.C == box_pos))
                box->C = Player.C;
            Player.C = box_pos;
        }
    }

    logic.undo();
    
    Player.prev_Cs.pop_back();
    Player.C = Player.prev_Cs.back();
    Player.prev_Cs.pop_back();

    bullet.prev_Cs.pop_back();
    bullet.C = bullet.prev_Cs.back();
    bullet.prev_Cs.pop_back();

    Player.dir = last_move[1];
    bullet.dir = last_move[3];

    step_end_logic();
    nbSteps--;
}

void Level::step_end_logic()
{
    Player.prev_Cs.push_back(Player.C);
    Player.prev_is_alive = Player.is_alive;
    bullet.prev_Cs.push_back(bullet.C);
    bullet.prev_is_alive = bullet.is_alive;

    boxes.step_end_logic();
    logic.step_end_logic();
}


//Display
void Level::resize_bg(sf::RenderWindow * windowP)   //Should get called every time the game's window is resized
{
    backGround.resize(sf::Vector2f(windowP->getSize()), bg_tiles);
}

void Level::displayBG(sf::RenderWindow * windowP, sf::Font font)
{
    windowP->clear(sf::Color(0, 0, 230));

    backGround.display(windowP);

    sf::Vector2u winSize = windowP->getSize();

    sf::Text stepDisp;
    stepDisp.setFont(font);
    std::stringstream ss;
    ss << "step : " << nbSteps;
    stepDisp.setString(ss.str());
    stepDisp.setPosition((winSize.x-stepDisp.getLocalBounds().width)/2, winSize.y-40);
    windowP->draw(stepDisp);
    //Use this method to display tips and text on the screen
}

void Level::display(sf::RenderWindow * windowP, sf::Font font, bool disp)
{
    windowP->clear(sf::Color(0, 0, 230));
    displayBG(windowP, font);

    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);

    logic.draw(C0, delta, windowP);

    if (Player.is_alive)
        Player.draw(C0, delta, windowP);
    else if (Player.prev_is_alive)
        Player.destroy(C0, delta, windowP, 4);

    if (bullet.is_alive)
        bullet.draw(C0, delta, windowP);
    else if (bullet.prev_is_alive)
        bullet.destroy(C0, delta, windowP, 4);
    
    boxes.draw(C0, delta, windowP);

    if (disp)
        windowP->display();
}

void Level::animate(sf::RenderWindow * windowP, sf::Font font)
{
    sf::Clock clock;
    int t1;

    sf::Vector2f windowSize = sf::Vector2f(windowP->getSize());

    sf::Vector2i C0;
    int delta;
    backGround.getDisplay(&C0, &delta);

    for (int i = 1; i <= 3; i++)//All animations are 4 frames long
    {
        t1 = clock.getElapsedTime().asMilliseconds();
        displayBG(windowP, font);

        //Animation of logic elements
        logic.anim(C0, delta, windowP, i);

        //Animation of the player
        if (Player.prev_is_alive && Player.is_alive)
            Player.anim(C0, delta, windowP, i);
        else if (Player.prev_is_alive)
            Player.destroy(C0, delta, windowP, i);
        else if (Player.is_alive)
            Player.draw(C0, delta, windowP);

        //Animation of the bullet
        if (bullet.prev_is_alive && bullet.is_alive)
            bullet.anim(C0, delta, windowP, i);
        else if (bullet.prev_is_alive)
            bullet.destroy(C0, delta, windowP, i);
        else if (bullet.is_alive)
            bullet.draw(C0, delta, windowP);

        //Animation of the boxes
        boxes.anim(C0, delta, windowP, i);

        windowP->display();
        while (clock.getElapsedTime().asMilliseconds()-t1 < 20) {}
    }
    display(windowP, font);
}

// Handling pause menu
int pause(Level* levelP, sf::RenderWindow* windowP, sf::Font font)
{
    Button continue_("continue", "Continue", Alignment(1, 0, 0, 3, 2, 0), windowP);
    Button exit_("exit", "Exit", Alignment(1, 0, 0, 3, 0, 0), windowP);
    
    // First draw :

    levelP->display(windowP, font, false);
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
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                windowP->setView(sf::View(view));
                levelP->resize_bg(windowP);
                levelP->display(windowP, font, false);
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
            levelP->display(windowP, font, false);
            continue_.draw(font);
            exit_.draw(font);
            windowP->display();
        }

    }

    return 0;
}

// Main gameplay loop with keyboard input handling
int run(int level_id, sf::RenderWindow* windowP, sf::Font font, int* nb_steps)
{
    //SFML stuff and level initialization :
    sf::Clock clock;
    Level level(level_id);
    level.resize_bg(windowP);

    //First draw :
    windowP->clear(sf::Color(0, 0, 120));
    level.display(windowP, font);
    
    //Input handling :
    int time = clock.getElapsedTime().asMilliseconds();
    int long_deltaT = 200;
    int short_deltaT = 100;
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
    std::list<std::string> steps;
    //A step can be : U, R, D, L, H (shot), P (swap), W or + (reset)
    std::list<Level> pre_resets;
    pre_resets.push_back(level);

    //Debug :
    int t0;

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
            
            else if (evnt.type == sf::Event::Resized)
            {
                sf::FloatRect view(0, 0, evnt.size.width, evnt.size.height);
                windowP->setView(sf::View(view));
                level.resize_bg(windowP);
                level.display(windowP, font);
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
                    level.display(windowP, font);
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
        if (pkey!=nbKeys && process_input)
        {
            int t = clock.getElapsedTime().asMilliseconds();
            step = false;
            didSwap = false;
            std::string act = level.get_pLike_state();
            if (pkey < 4)
            {
                act.push_back(directions[pkey]);
                step = level.push(directions[pkey], &act);
            }
            else if (pkey == 4)
            {
                step = level.swap(&act);
                didSwap = true;
            }
            else if (pkey == 5)
            {
                if (steps.size() > 0)
                {
                    if (steps.back() == "+")
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
                act.push_back('W');
            }
            else if (pkey == 7)
            {
                if (steps.size() > 0 && steps.back() != "+")
                {
                    pre_resets.push_back(level);
                    level = pre_resets.front();
                    level.resize_bg(windowP);
                    steps.push_back("+");
                }
            }

            if (step)
            {
                if (act != "")
                {
                    steps.push_back(act);
                }
                level.step(didSwap);
                t0 = clock.getElapsedTime().asMilliseconds();
                level.animate(windowP, font);
                level.step_end_logic();
            }
            else
                level.display(windowP, font);

            std::cout << "full step process time : " << clock.getElapsedTime().asMilliseconds()-t << "ms" << std::endl;
        }

        if (level.won)
        {
            *nb_steps = level.nbSteps;
            return 1;
        }
    }

    return 0;
}