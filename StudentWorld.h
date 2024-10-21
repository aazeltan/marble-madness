#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include"Actor.h"
#include <list>
#include <vector>
using namespace std;

const int RIGHT = 0;
const int LEFT = 180;
const int UP = 90;
const int DOWN = 270;
class Agent;
class Actor;
class Player;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool isValidPos(double x, double y);
    void display(); 
    
    bool addPeas(double x, double y, int dir);
    bool overlapping(double x1, double y1, double x2, double y2); 
    
    // Can agent move to x,y?  (dx and dy indicate the direction of motion)
    bool canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy) ;
    
    // Can a marble move to x,y?
    bool canMarbleMoveTo(int x, int y);
    
    // Is the player on the same square as an Actor?
    bool isPlayerColocatedWith(int x, int y);

    // Try to cause damage to something at a's location.  (a is only ever going to be a pea.)  Return true if something stops a -- something at this location prevents a pea from continuing.
    bool damageSomething(Actor* a, int damageAmt);

    bool marbleHere(int x, int y); 
    
    // Swallow any swallowable object at a's location.  (a is only ever going to be a pit.)
    bool swallowSwallowable(Actor* a);

    // If a pea were at x,y moving in direction dx,dy, could it hit the player without encountering any obstructions?
    bool existsClearShotToPlayer(int x, int y, int dx, int dy, int dir);

    /* If a factory is at x,y, how many items of the type that should be counted are in the rectangle bounded by x-distance,y-distance and x+distance,y+distance?  Set count to that number and return true,
     unless an item is on the factory itself, in which case return falseand don't care about count.  (The items counted are only ever going ThiefBots.)*/
    bool doFactoryCensus(int x, int y, int distance, int& count) const;

         // If an item that can be stolen is at x,y, return a pointer to it;
         // otherwise, return a null pointer.  (Stealable items are only ever
         // going be goodies.)
    Actor* getColocatedStealable(int x, int y); 
    
    Actor* actorHere(int x, int y); 
    
    bool isActorHere(int x, int y); 
    
    bool playerHere(int x, int y); 
    
    bool thiefbotHere(int x, int y); 
    
    bool peaHere(int x, int y); 
    


//Restore player's health to the full amount.
    void restorePlayerHealth(){
        m_player->restoreHealth();}

         // Increase the amount of ammunition the player has
    void increaseAmmo(){m_player->increaseAmmo();}
    
    void increaseLife() {incLives(); }

    // Are there any crystals left on this level?
    bool anyCrystals();

    // Reduce the count of crystals on this level by 1.
    void decCrystals(); 
    
    // Indicate that the player has finished the level.
    void setLevelFinished(){
        m_level_finished = true;
    }
    
    void addActor(Actor* actor){
        m_actors.push_back(actor);
    }

private:
    Player* m_player = nullptr;
    list<Actor*> m_actors;
    int bonusPoints; 
    int m_crystals;
    bool m_level_finished;
    
};

#endif // STUDENTWORLD_H_
