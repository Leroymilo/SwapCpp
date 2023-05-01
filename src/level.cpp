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

Level::Level(std::string file_name)
{
    // Reading Json :
    std::ifstream file("levels/" + file_name + ".json");
    Json::Reader reader;
    Json::Value json_data;
    reader.parse(file, json_data);

    // Loading level flags :
    Json::Value json_flags = json_data["flags"];
    int i = 0;
    for (Json::Value::iterator itr = json_flags.begin(); itr != json_flags.end(); itr++)
    {
        std::string key = json_flags.getMemberNames()[i];
        flags[key] = json_flags[key].asBool();
        flag_textures[key].loadFromFile("assets/" + key + ".png");
        flag_sprites[key].setTexture(flag_textures[key], true);
        i ++;
    }

    // Loading tiles sprites :
    bg_tiles['X'].loadFromFile("assets/Tiles/Wall.png");
    bg_tiles['T'].loadFromFile("assets/Tiles/Thorns.png");
    bg_tiles['.'].loadFromFile("assets/Tiles/Floor.png");
    bg_tiles['E'].loadFromFile("assets/Tiles/Win.png");

    // Making objects :
    win_tile = backGround.create(json_data["bg"], bg_tiles);
    player = SwapperEntity(json_data["entities"]["player"], "player", flags["has_ghost"]);
    
    Json::Value boxes_data = json_data["entities"]["statues"];
    statues.resize(boxes_data.size());
    for (int i = 0; i < statues.size(); i++)
        statues[i] = BaseEntity(boxes_data[i], "statue");

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
    perf_steps = json_data["perf_steps"].asInt();

    process_logic(false);
    validate_step();
}

Level::Level(int number, bool solved) : Level(make_level_name(number))
{
    this->solved = solved;
}


//Gameplay
bool Level::get_statue(sf::Vector2i coords, BaseEntity *&statue_p)
{
    for (auto & statue : statues) {
        if (statue.pos == coords) {
            statue_p = &statue;
            return true;
        }
    }
    return false;
}

bool Level::get_physf(sf::Vector2i coords, MovingEntity *&physf_p)
{
    if (coords == player.physf.pos) {
        physf_p = &(player.physf);
        return true;
    }
    return false;
}

bool Level::get_ghost(sf::Vector2i coords, MovingEntity *&ghost_p)
{
    if (coords == player.ghost.pos) {
        ghost_p = &(player.ghost);
        return true;
    }
    return false;
}

bool Level::is_wall_for_physf(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X' or backGround.getTile(coords) == 'T')
        return true;
    
    return logic.is_wall_for_physf(coords);
}

bool Level::is_wall_for_ghost(sf::Vector2i coords)
{
    if (backGround.getTile(coords) == 'X')
        return true;
    
    BaseEntity *statue_p;
    if (get_statue(coords, statue_p) && statue_p->alive)
        return true;
    
    return logic.is_wall_for_ghost(coords);
}

bool Level::base_push(BaseEntity *pusher, Direction dir)
{
    sf::Vector2i next_pos = pusher->get_next_pos(dir);

    // Checking if there's a wall blocking :
    bool can_push = !is_wall_for_physf(next_pos);

    if (!can_push) return false;

    // Checking if there's a any statue to push :
    BaseEntity *statue_p;
    if (get_statue(next_pos, statue_p))
        can_push = base_push(statue_p, dir);

    if (!can_push) return false;

    // Pushing any physf :
    MovingEntity *physf_p;
    if (get_physf(next_pos, physf_p)) {
        if (physf_p->alive <= pusher->alive)
            physf_push(physf_p, dir);
    }

    // Pushing any ghost (must not be broken) :
    if (pusher->alive)
    {
        MovingEntity *ghost_p;
        if (get_ghost(next_pos, ghost_p)) {
            if (ghost_p->alive)
                ghost_push(ghost_p, dir);
        }
    }

    // Updating the coords
    pusher->pos = next_pos;
    
    return true;
}

bool Level::physf_push(MovingEntity *pusher, Direction dir)
{
    sf::Vector2i next_pos = pusher->get_next_pos(dir);

    if (pusher->alive) pusher->dir = dir;
    
    // Checking if there's a wall blocking :
    bool can_push = !is_wall_for_physf(next_pos);

    if (!can_push) return false;

    // Checking if there's a any statue to push :
    BaseEntity *statue_p;
    if (get_statue(next_pos, statue_p))
        can_push = base_push(statue_p, dir);

    if (!can_push) return false;

    // Updating the coords
    pusher->pos = next_pos;

    return true;
}

bool Level::ghost_push(MovingEntity *pusher, Direction dir)
{
    sf::Vector2i next_pos = pusher->get_next_pos(dir);

    if (pusher->alive) pusher->dir = dir;
    
    // Checking if there's a wall blocking :
    bool can_push = !is_wall_for_ghost(next_pos);

    if (!can_push) return false;

    // Trying to push an eventual ghost
    MovingEntity *ghost_p;
    if (get_ghost(next_pos, ghost_p)) {
        if (ghost_p->alive)
            can_push = ghost_push(ghost_p, dir);
    }

    if (!can_push) return false;

    // Updating the coords
    pusher->pos = next_pos;

    return true;
}

bool Level::player_push(Direction dir)
{
    if (!player.physf.alive)
        return false;

    bool change_dir = !(player.physf.dir == dir);
    player.physf.dir = dir;

    return physf_push(&(player.physf), dir) || change_dir;
}

bool Level::swap()
{
    if (!flags["can_swap"]) return false;

    // Swapping
    if (player.ghost.alive) {
        if (is_wall_for_physf(player.ghost.pos))
            return false;
        player.swap();
    }

    // Summoning
    else if (player.physf.alive) {
        if (is_wall_for_ghost(player.physf.get_next_pos()))
            return false;
        player.summon();
    }

    else return false;

    return true;
}

bool Level::wait()
{
    // Waiting is only possible if there are non-playable alive MovingEntities in the level
    if (player.ghost.alive)
    {
        return true;
    }
    return false;
}

void Level::process_logic(bool did_swap)
{
    std::unordered_set<sf::Vector2i, VectorHasher> physf_pos;
    std::unordered_set<sf::Vector2i, VectorHasher> ghost_pos;

    if (player.physf.alive)
        physf_pos.insert(player.physf.pos);
    for (BaseEntity &statue : statues) {
        if (statue.alive)
            physf_pos.insert(statue.pos);
    }
    
    if (player.ghost.alive)
        ghost_pos.insert(player.ghost.pos);

    std::list<sf::Vector2i> updated_activators = logic.update_activators(
        physf_pos, ghost_pos, did_swap, &(player.ghost.alive)
    );

    logic.update(updated_activators);
}

void Level::step(bool did_swap)
{
    // Ghost movement :
    if (player.ghost.alive && !did_swap && !is_wall_for_ghost(player.ghost.pos))
    {
        sf::Vector2i new_pos = player.ghost.get_next_pos();

        if (is_wall_for_ghost(new_pos))
        {
            player.ghost.revert();
            new_pos = player.ghost.get_next_pos();
        }

        if (is_wall_for_ghost(new_pos))
            player.ghost.revert();
        else
            player.ghost.pos = new_pos;
    }

    // Checking if player physf get shot :
    if (player.ghost.alive && player.physf.alive)
    {
        player.physf.alive = player.physf.pos != player.ghost.pos;
        if (player.physf.get_prev_pos() == player.ghost.pos && player.physf.pos == player.ghost.get_prev_pos() && !did_swap)
        {
            // physf crossed its ghost
            player.physf.alive = false;
        }
    }

    process_logic(did_swap);

    // Checking if player and/or ghost get crushed :
    player.physf.alive = player.physf.alive && !is_wall_for_physf(player.physf.pos);
    player.ghost.alive = player.ghost.alive && !is_wall_for_ghost(player.ghost.pos);

    //Same for boxes :
    for (BaseEntity &statue : statues) {
        statue.alive = statue.alive && !is_wall_for_physf(statue.pos);
    }

    nbSteps++;
}

void Level::undo(std::list<char>* steps)
{
    steps->pop_back();

    player.undo();
    for (BaseEntity &statue : statues) {
        statue.undo();
    }
    logic.undo();

    nbSteps--;
}

void Level::validate_step()
{
    player.validate_step();
    for (BaseEntity &statue : statues) {
        statue.validate_step();
    }
    logic.validate_step();

    if (player.physf.alive && player.physf.pos == win_tile)
        won = true;
}


//Display
void Level::resize_bg(sf::RenderWindow * windowP)   //Should get called every time the game's window is resized
{
    sf::Vector2f winSize(windowP->getSize());

    int Y0 = 0;
    sf::FloatRect bounds;

    // bounds = sf::Text(name, font.get_font(), 30).getLocalBounds();
    // Y0 += bounds.height + bounds.top;
    // winSize.y -= Y0;

    bounds = sf::Text("Ip", font.get_font(), 28).getLocalBounds();
    winSize.y -= bounds.height + bounds.top;
    // A literal string is used because the number of steps changing makes the whole text bump around if used.

    for (int i = hint_lines-1; i >= 0; i--)
    {
        sf::FloatRect bounds = sf::Text(hint[i], font.get_font(), 24).getLocalBounds();
        winSize.y -= bounds.height + bounds.top;
    }
    for (int i = dlg_lines-1; i >= 0; i--)
    {
        sf::FloatRect bounds = sf::Text(dlg[i], font.get_font(), 24).getLocalBounds();
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
    // sf::Text nameDisp(name, font.get_font(), 30);
    // bounds = nameDisp.getLocalBounds();
    // nameDisp.setPosition((winSize.x-bounds.width-bounds.left)/2, 0);
    // windowP->draw(nameDisp);

    // Displaying level background :
    backGround.display(windowP);

    // Displaying number of steps :
    std::string step_text = "step : " + std::to_string(nbSteps);
    if (solved) {step_text += "/" + std::to_string(perf_steps);}
    sf::Text stepDisp(step_text, font.get_font(), 28);
    bounds = stepDisp.getLocalBounds();
    stepDisp.setPosition((winSize.x-bounds.width-bounds.left)/2, max_y-bounds.height-bounds.top);
    windowP->draw(stepDisp);
    bounds = sf::Text("Ip", font.get_font(), 28).getLocalBounds();
    max_y -= bounds.height + bounds.top;
    // This is because stepDisp changes height when the step number changes, so I put something that only depends on the font.get_font() used.

    // Displaying level text :
    for (int i = hint_lines-1; i >= 0; i--)
    {
        sf::Text helpDisp(hint[i], font.get_font(), 24);
        helpDisp.setStyle(sf::Text::Italic);
        bounds = helpDisp.getLocalBounds();
        helpDisp.setPosition((winSize.x-bounds.width-bounds.left)/2, max_y-bounds.height-bounds.top);
        windowP->draw(helpDisp);
        max_y -= bounds.height + bounds.top;
    }
    for (int i = dlg_lines-1; i >= 0; i--)
    {
        sf::Text helpDisp(dlg[i], font.get_font(), 24);
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

    for (std::string flag_key : flag_order)
    {
        if (flags[flag_key])
        {
            flag_sprites[flag_key].setPosition(sf::Vector2f(C0));
            windowP->draw(flag_sprites[flag_key]);
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
    for (BaseEntity &statue : statues) {
        statue.draw(C0, delta, windowP, frame);
    }
    player.draw(C0, delta, windowP, frame);

    if (disp)
        windowP->display();
}

void Level::display(sf::RenderWindow * windowP, bool disp)
{
    animate(windowP, 4, disp);
}

// Handling pause menu
int pause(Level* levelP, sf::RenderWindow* windowP)
{
    Button continue_("continue", "Continue", Alignment(1, 0, 0, 3, 2, 0), windowP);
    Button exit_("exit", "Exit", Alignment(1, 0, 0, 3, 0, 0), windowP);
    
    // First draw :

    levelP->display(windowP, false);
    continue_.draw();
    exit_.draw();
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
                continue_.draw();
                exit_.draw();
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
                continue_.draw();
                exit_.draw();
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
            continue_.draw();
            exit_.draw();
            windowP->display();
        }

    }

    return 0;
}

// Main gameplay loop with keyboard input handling
int run(int level_id, bool solved, sf::RenderWindow* windowP, int* nb_steps)
{
    //SFML stuff and level initialization :
    sf::Clock clock;
    Level level(level_id, solved);
    level.resize_bg(windowP);

    //First draw :
    windowP->clear(sf::Color(0, 0, 120));
    level.display(windowP);
    
    //Input handling :
    int time = clock.getElapsedTime().asMilliseconds();
    int long_deltaT = 240;
    int short_deltaT = 200;
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
    bool did_swap = false;
    std::list<char> steps;
    //A step can be : U, R, D, L, H (shot), P (swap), W or + (reset)
    std::list<Level> pre_resets;
    pre_resets.push_back(level);

    //Animation handling :
    bool animating = false;
    int anim_deltaT = 40;
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
                    int res = pause(&level, windowP);

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
            did_swap = false;

            if (pkey < 4)
            {
                Direction dir = static_cast<Direction>(pkey);
                step = level.player_push(dir);
                if (step) steps.push_back(DIR2CHAR.at(dir));
            }

            else if (pkey == 4)
            {
                step = level.swap();
                if (step)
                {
                    steps.push_back('P');
                    did_swap = true;
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
                level.step(did_swap);
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