#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <sstream>
#include <iomanip>


GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_player = nullptr;
    bonusPoints = 1000;
    m_actors.clear();
}

void StudentWorld::display() {
    int score = getScore();
    int level = getLevel();
    unsigned int bonus = bonusPoints;
    int livesLeft = getLives();
    int health = m_player->getHealthPct();
    int ammo = m_player->getAmmo();

    std::ostringstream oss;
    oss << "Score: " << std::setw(7) << std::setfill('0') << score << "  "
        << "Level: " << std::setw(2) << std::setfill('0') << level << "  "
        << "Lives: " << std::setw(2) << std::setfill(' ') << livesLeft << "  "
        << "Health: " << std::setw(3) << std::setfill(' ') << health << "%  "
        << "Ammo: " << std::setw(3) << std::setfill(' ') << ammo << "  "
        << "Bonus: " << std::setw(4) << std::setfill(' ') << bonus;

    std::string s = oss.str();
    setGameStatText(s);
}

int StudentWorld::init()
{
    m_crystals = 0;
    m_level_finished = false;
    bonusPoints = 1000;
    Level lev(assetPath());
    string levelFile = "level0"+ to_string(getLevel()) + ".txt";
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        cerr << "Cannot find " << levelFile << " data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cerr << "Your level was improperly formatted" << endl;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level\n";
        for (int x = 0; x < 15; x++){
            for (int y = 0; y < 15; y++) {
                Level::MazeEntry ge = lev.getContentsOf(x,y);
                switch (ge)
                {
                    case Level::empty:
                        cout << x <<","<<y<<" is empty\n";
                        break;
                    case Level::exit:
                        cout << x <<","<<y<<" is where the exit is\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new Exit(this, x, y));
                        }
                        break;
                    case Level::player:
                        cout << x << "," << y << " is where the player starts\n";
                        if (isValidPos(x, y)) {
                            m_player = new Player(this, x, y);
                            }
                        break;
                    case Level::horiz_ragebot:
                        cout << x << "," << y << " starts with a horiz. RageBot\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new RageBot(this, x, y, RIGHT));
                        }
                        break;
                    case Level::vert_ragebot:
                        cout << x << "," << y << " starts with a vertical RageBot\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new RageBot(this, x, y, DOWN));
                        }
                        break;
                    case Level::thiefbot_factory:
                        cout << x << "," << y << " holds a ThiefBot factory\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new ThiefBotFactory(this, x, y, ThiefBotFactory::REGULAR));
                        }
                        break;
                    case Level::mean_thiefbot_factory:
                        cout << x << "," << y << " holds an enraged ThiefBot factory\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new ThiefBotFactory(this, x, y, ThiefBotFactory::MEAN));
                        }
                        break;
                    case Level::wall:
                        cout << "Location " << x << "," << y << " holds a wall\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new Wall(this, x, y));
                        }
                        break;
                    case Level::marble:
                        cout << "Location " << x << "," << y << " holds a marble\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new Marble(this, x, y));
                        }
                        break;

                    case Level::pit:
                        cout << "Location " << x << "," << y << " holds a pit\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new Pit(this, x, y));
                        }
                        break;
                    case Level::crystal:
                        cout << "Location " << x << "," << y << " holds a crystal\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new Crystal(this, x, y));
                            m_crystals++;
                        }
                        break;
                    case Level::restore_health:
                        cout << "Location " << x << "," << y << " holds a restore health goodie\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new RestoreHealthGoodie(this, x, y));
                        }
                        break;
                    case Level::extra_life:
                        cout << "Location " << x << "," << y << " holds a extra life goodie\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new ExtraLifeGoodie(this, x, y));
                        }
                        break;
                    case Level::ammo:
                        cout << "Location " << x << "," << y << " holds a ammo goodie\n";
                        if (isValidPos(x, y)) {
                            m_actors.push_back(new AmmoGoodie(this, x, y));
                        }
                        break;
                }
            }
        }
    }
    display(); 
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    
    display();
    list<Actor*> :: iterator it;
    
    for (it = m_actors.begin(); it != m_actors.end(); it++){
        if ((*it)->needsClearShot())
            continue;
        (*it)->doSomething();
    }

    list<Actor*> :: iterator it2;
    it2 = m_actors.begin();
    while (it2 != m_actors.end()){
        if ((*it2)->needsClearShot()){
            (*it2)->doSomething();
        }
        it2++;
    }
        
    m_player->doSomething();
    
    if (bonusPoints > 0)
        bonusPoints--;
    
    list<Actor*> :: iterator it3;
    it3 = m_actors.begin();
    while (it3 != m_actors.end())
    {
        list<Actor*> :: iterator temp = it3;
        it3++;
        
        if(!(*temp)->isAlive())
        {
            delete *temp;
            m_actors.erase(temp);
        }
    }
    if (!m_player->isAlive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    if (m_level_finished)
    {
        playSound(SOUND_FINISHED_LEVEL);
        increaseScore(2000+bonusPoints);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    // the player hasn’t completed the current level and hasn’t died, so continue playing the current level
    return GWSTATUS_CONTINUE_GAME;

}

void StudentWorld::cleanUp()
{
    if (m_player != nullptr) {
        
        delete m_player;
        m_player = nullptr;
    }
   
    if (m_actors.size() == 0)
        return;
    
    list<Actor*> :: iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end()){
        
        list<Actor*> :: iterator temp = it;
        it++;
        delete *temp;
        m_actors.erase(temp);
    }
  
}

bool StudentWorld::isValidPos(double x, double y) {
    for (Actor* a : m_actors) {
        // Check if the coordinates (x, y) are within the bounding box of the actor
        if (x >= a->getX() && x < a->getX() + 1 &&
            y >= a->getY() && y < a->getY() + 1) {
            // There is an actor at the specified coordinates
            return false;
        }
    }
    // No actor found at the specified coordinates
    return true;
}

bool StudentWorld::overlapping(double x1, double y1, double x2, double y2) {
    if (x1 == x2 && y1 == y2)
        return true;
    return false;
}

bool StudentWorld::canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy){
    // Use a range-based for loop to iterate over the list of actors
    for (Actor* a : m_actors) {
        // Skip actors that can be walked over, like collectibles
        if(a==agent||a->allowsAgentColocation())
            continue;
        
        // Actor occupies space, can't move there UNLESS player can push marble
        if (a->isLocatedAt(x+dx, y+dy)) {
            // If actor is a marble, try pushing it if the agent is allowed to
            if (a->isSwallowable() && a->bePushedBy(agent, x + 2*dx, y + 2*dy)) {
                return true;  // we can assume that nothing else will be on a marble tile
            }
            return false;
        }
    }

    return true;
}

bool StudentWorld:: existsClearShotToPlayer(int x, int y, int dx, int dy, int dir){
    int pX = m_player->getX(), pY = m_player->getY();
    
    //check if player in same row/column
    if (pX != x && pY!= y)
        return false;
    
    //incorrect direction
    
    if (pX == x && (dir == 0 || dir == 180))
        return false;
    if (pY == y && (dir == 90 || dir == 270))
        return false;
        
    if (pX == x){
        if ((dy >0 && pY<y) || (dy <0 && pY > y))
            return false;
    }
    
    if (pY == y){
        if ((dx >0 && pX<x) || (dx <0 && pX > x))
            return false;
    }
    
    //actors in the way
    
    list<Actor*> :: iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end()){
        if ((*it)->stopsPea() || (*it)->isDamageable() ){
            if ((*it)->getX() == x && (((*it)->getY() < pY && (*it)->getY() > y)|| ((*it)->getY() > pY && (*it)->getY() < y))) {
                return false;
            }
            if ((*it)->getY() == y && (((*it)->getX() < pX && (*it)->getX() > x) || ((*it)->getX() > pX && (*it)->getX() < x))) {
                return false;
            }
        }
        it++;
    }
    return true;
}

bool StudentWorld::canMarbleMoveTo(int x, int y){
    // Check if the specified coordinates are within the bounds of the game world
    if (!isValidPos(x, y)) {
        if (actorHere(x,y)->allowsMarble())
            return true;
        return false;
    }
    
    // Check if there are any obstacles (e.g., walls) at the specified position
    for (const auto& actor : m_actors) {
        // Skip actors that don't block marbles
        if (!actor->allowsMarble()) {
            continue;
        }
        
        // Check if the actor is located at the specified position
        if (actor->getX() == x && actor->getY() == y) {
            return false; // Marble cannot move to a position occupied by an obstacle
        }
    }

    // If no obstacles are blocking the specified position, return true
    return true;
}



bool StudentWorld::doFactoryCensus(int x, int y, int distance, int& count) const {
    count = 0;
    int minX = x - distance;
    int maxX = x + distance;
    int minY = y - distance;
    int maxY = y + distance;

    for (Actor* a : m_actors) {
        if (a->countsInFactoryCensus()) {
            //actor is within distance of 3
            if (a->getX() >= minX && a->getX() <= maxX && a->getY() >= minY && a->getY() <= maxY) {
                count++;
            }
            //actor same position as factory
            if (a->getX() == x && a->getY() == y) {
                return false;
            }
        }
    }
    return true;
}

bool StudentWorld::isPlayerColocatedWith(int x, int y){
    return overlapping(m_player->getX(), m_player->getY(), x,y);
}

bool StudentWorld::damageSomething(Actor* a, int damageAmt){
    list<Actor*> :: iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end()){
        if (overlapping((*it)->getX(), (*it)->getY(), a->getX(), a->getY())){
            if ((*it)->stopsPea()){
                if ((*it)->isDamageable()){
                    (*it)->damage(damageAmt);
                }
                if ((*it)->createsBots()){
                    list<Actor*> :: iterator it2;
                    it2 = it++;
                    while (it2 != m_actors.end()){
                        if (overlapping((*it2)->getX(), (*it2)->getY(), a->getX(), a->getY())){
                            if ((*it2)->countsInFactoryCensus()){
                                    (*it2)->damage(damageAmt);
                            }
                        }
                        it2++;
                    }
                }
                return true;
            }
        }
        it++;
    }
    
    if (overlapping(m_player->getX(), m_player->getY(), a->getX(), a->getY())){
        m_player->damage(damageAmt);
        return true;
    }
    return false;
}

bool StudentWorld::marbleHere(int x, int y){
    list<Actor*> :: iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end()){
        if ((*it)->isSwallowable())
            if ((*it)->getX() == x && (*it)->getY() ==y){
                swallowSwallowable(*it);
                return true;
            }
        it++;
    }
    return false;
}


bool StudentWorld::swallowSwallowable(Actor* a){
    if (a->isSwallowable()){
            a->setDead();
            return true;
        }
    return false;
}

Actor* StudentWorld::getColocatedStealable(int x, int y){
    list<Actor*> :: iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end()){
        if (x == (*it)->getX() && y == (*it)->getY() && (*it)->isStealable())
            return (*it);
        it++;
    }
    return nullptr;
}

Actor* StudentWorld::actorHere(int x, int y){
    list<Actor*> :: iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end()){
        if ((*it)->getX() == x && (*it)->getY() == y)
            return (*it);
        it++;
    }
    return nullptr;
}

bool StudentWorld::isActorHere(int x, int y){
    if (actorHere(x, y) != nullptr)
        return true;
    return false;
}

bool StudentWorld::playerHere(int x, int y){
    if (m_player->getX() == x && m_player->getY() == y)
        return true;
    return false;
}

bool StudentWorld :: thiefbotHere(int x, int y) {
    list<Actor*> :: iterator it;
    it = m_actors.begin();

    while (it != m_actors.end()){
        if (overlapping((*it)->getX(), (*it)->getY(), x, y))
            if ((*it)->countsInFactoryCensus())
                return true;
        it++;
    }

    return false;
}

bool StudentWorld :: peaHere(int x, int y) {
    list<Actor*> :: iterator it;
    it = m_actors.begin();

    while (it != m_actors.end()){
        if (overlapping((*it)->getX(), (*it)->getY(), x, y))
            if ((*it)->stopsPea())
                return true;
        it++;
    }

    return false;
}



bool StudentWorld::anyCrystals(){
    
    if (m_crystals == 0)
        return true;
    
    return false;
}

void StudentWorld:: decCrystals() {
    if (m_crystals > 0){
        m_crystals --;
        return;
    }
}
