#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"


class StudentWorld;
class Agent;

// Actor.h

class Actor : public GraphObject {
public:
    Actor(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int startDir, bool inPlay = true)
        : GraphObject(imageID, startX, startY), m_hitPoints(hitPoints), m_startDir(startDir), alive(true), m_inPlay(inPlay), m_world(world) {
        setDirection(startDir);
        setVisible(inPlay);
    }
    virtual void doSomething() = 0;
    bool isAlive() { return alive; }
    void setDead() { alive = false; }
    // NEW
    bool isInPlay() { return m_inPlay; }
    void setInPlay(bool inPlay) { m_inPlay = inPlay; setVisible(m_inPlay); }
    void decHitPoints(int amt) { m_hitPoints -= amt; }
    StudentWorld* getWorld() { return m_world; }
    virtual bool allowsAgentColocation() { return false; }
    virtual int getScore() { return -1; }
    virtual bool createsBots() {return false; }

    virtual bool allowsMarble() { return false; }    // pit
    virtual bool needsClearShot() { return false; }  // robot

    virtual bool canPushMarbles() { return false; }  // player

    // Does this actor count when a factory counts items near it?
    // thiefbot
    virtual bool countsInFactoryCensus() { return false; }

    // Does this actor stop peas from continuing?
    virtual bool stopsPea() { return false; }

    // Can this actor be damaged by peas?
    virtual bool isDamageable() { return false; }

    // Cause this Actor to sustain damageAmt hit points of damage.
    virtual void damage(int damageAmt) { return; }

    // Can this actor be pushed by a to location x,y?
    virtual bool bePushedBy(Agent* a, int x, int y) { return false; }

    // Can this actor be swallowed by a pit?
    // marbel
    virtual bool isSwallowable() { return false; }

    // Can this actor be picked up by a ThiefBot?
    // goodies
    virtual bool isStealable() { return false; }

    // How many hit points does this actor have left?
    virtual int getHitPoints() { return m_hitPoints; }

    // Set this actor's hit points to amt.
    virtual void setHitPoints(int amt) { m_hitPoints = amt; }

    // Make the actor sustain damage.  Return true if this kills the
    // actor, and false otherwise.
    virtual bool tryToBeKilled(int damageAmt);

    bool isLocatedAt(int x, int y) {
        return (getX() == x && getY() == y);
    }

private:
    StudentWorld* m_world;
    bool alive;
    bool m_inPlay;
    int m_hitPoints;
    int m_startDir;
};


class Agent : public Actor
{
public:
    Agent(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int startDir):Actor(world, startX, startY, imageID, hitPoints, startDir) { }
    
    // Move to the adjacent square in the direction the agent is facing
    // if it is not blocked, and return true.  Return false if the agent
    // can't move.
    bool moveIfPossible();
    
    // Return true if this agent can push marbles (which means it's the player).
    virtual bool canPushMarbles(){return false;}
    
    // Return true if this agent doesn't shoot unless there's an unobstructed
    // path to the player.
    virtual bool needsClearShot() = 0;
    
    // Return the sound effect ID for a shot from this agent.
    virtual int shootingSound() = 0;
    
    void shootPea(); 
    
    bool stopsPea(){return true;}
};

class Player : public Agent
{
public:
    Player(StudentWorld* world, int startX, int startY):Agent(world, startX, startY, IID_PLAYER, 20, 0), m_score(0), m_ammo(20){ }
    virtual void doSomething();
    virtual bool stopsPea() {return true;}
    virtual bool isDamageable() {return true;}
    virtual void damage(int damageAmt); 
    virtual bool canPushMarbles(){return true;}
    virtual bool needsClearShot() {return false;}
    virtual int shootingSound() {
        return SOUND_PLAYER_FIRE;
    }
    
    // Get player's health percentage
    int getHealthPct() { return getHitPoints()*100/20; }

    // Get player's amount of ammunition
    int getAmmo(){ return m_ammo; }
  
    // Restore player's health to the full amount.
    void restoreHealth() { setHitPoints(20); }
  
    // Increase player's amount of ammunition.
    void increaseAmmo(){m_ammo+=20;}


private:
    int m_score;
    int m_ammo;

};

class Robot: public Agent
{
public:
    Robot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score, int startDir);
    virtual void doSomething(); 
    virtual bool isDamageable(){return true;}
    virtual void damage(int damageAmt);
    virtual bool canPushMarbles() {return false;}
    virtual bool needsClearShot(){return true; }
    virtual int shootingSound() {return SOUND_ENEMY_FIRE; }
    virtual bool countsInFactoryCensus() {return false;} 
    virtual bool isShootingRobot(){return true;}
    virtual bool robotShoot(); 
    virtual void robotMove() = 0;
    virtual void robotDamage(){return;}

private:
    int m_score;
    int m_ticks;
    int m_curTicks;
    
};

class RageBot: public Robot
{
public:
    RageBot(StudentWorld* world, int startX, int startY, int startDir):Robot(world, startX, startY, IID_RAGEBOT, 10, 100, startDir){}
    virtual void robotMove();
};

class ThiefBot : public Robot
{
public:
    enum goodieType { NONE, EXTRLIFE, RESHP, AMMO };

    ThiefBot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score):Robot(world, startX, startY, imageID, hitPoints, score, right){  m_dist = randInt(1, 6);}
    virtual bool countsInFactoryCensus() {return true;}
    virtual void robotMove(); 
    virtual void robotDamage(); 
    
private:
    int m_dist;
    Actor* m_goodie;
    goodieType g_type;

};

class RegularThiefBot : public ThiefBot
{
public:
    RegularThiefBot(StudentWorld* world, int startX, int startY):ThiefBot(world, startX, startY, IID_THIEFBOT, 5, 10){}
    virtual bool isShootingRobot(){return false;}
};

class MeanThiefBot : public ThiefBot
{
public:
    MeanThiefBot(StudentWorld* world, int startX, int startY):ThiefBot(world, startX, startY, IID_MEAN_THIEFBOT, 8, 10){}
    virtual bool isShootingRobot(){return true;}
    
};

    
    
class Exit : public Actor {
public:
    Exit(StudentWorld* world, int startX, int startY) : Actor(world, startX, startY, IID_EXIT, 0, 0, false) {}

    virtual void doSomething();
    virtual bool allowsAgentColocation() { return true; }
    virtual bool stopsPea() { return false; }
};

class Wall : public Actor
{
public:
    Wall(StudentWorld* world, int startX, int startY): Actor(world,startX,startY, IID_WALL, 0, 0) { }
    virtual void doSomething() {return; }
    virtual bool stopsPea() {return true;}
};

class Marble : public Actor
{
public:
    Marble(StudentWorld* world, int startX, int startY): Actor(world, startX, startY, IID_MARBLE, 10, 0) {}
    
    virtual bool stopsPea() {return true;}
    
    virtual void doSomething();
    virtual bool isDamageable() {return true;}
    virtual void damage(int damageAmt){decHitPoints(damageAmt);}
    
    virtual bool isSwallowable() {return true;}
    
    virtual bool bePushedBy(Agent* a, int x, int y);

};

class Pit : public Actor
{
public:
    Pit(StudentWorld* world, int startX, int startY):Actor(world, startX, startY, IID_PIT, 0, 0){ }
    virtual void doSomething();
    virtual bool allowsMarble() {return true;}
    
};

class Pea : public Actor
{
public:
    Pea(StudentWorld* world, int startX, int startY, int startDir):Actor(world, startX, startY, IID_PEA, 0, startDir){ }
    virtual void doSomething();
    virtual bool allowsAgentColocation() {return true;}
};

class ThiefBotFactory : public Actor
{
public:
    enum ProductType { REGULAR, MEAN };

    ThiefBotFactory(StudentWorld* world, int startX, int startY, ProductType type):Actor(world, startX, startY, IID_ROBOT_FACTORY, none, 0), m_type(type){}
    virtual void doSomething();
    virtual bool stopsPea() {return true; }
    virtual bool createsBots() {return true; }
    
private:
    ProductType m_type;
};


class PickupableItem : public Actor
{
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID, int score):Actor(world, startX, startY, imageID, 0, 0), m_score (score){ }
    virtual void doSomething();
    virtual bool allowsAgentColocation() {return true;}
    virtual int getScore (){return m_score; }
    virtual bool stopsPea() {return false;}
private:
    int m_score;
};

class Crystal : public PickupableItem
{
public:
    Crystal(StudentWorld* world, int startX, int startY): PickupableItem(world,startX, startY, IID_CRYSTAL, 50){}
};

class Goodie : public PickupableItem
{
public:
    Goodie(StudentWorld* world, int startX, int startY, int imageID,
           int score) : PickupableItem(world, startX, startY, imageID, score) {
        stolen = false; 
    }
    virtual bool isStealable() { return true; }
    
      // Set whether this goodie is currently stolen.
    void setStolen(bool status) {
        stolen = status;
    }
    
private:
    bool stolen;
    
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(StudentWorld* world, int startX, int startY): Goodie(world, startX, startY, IID_EXTRA_LIFE, 1000){}
    
};

class RestoreHealthGoodie : public Goodie
{
public:
    RestoreHealthGoodie(StudentWorld* world, int startX, int startY): Goodie(world, startX, startY, IID_RESTORE_HEALTH, 500){}
    
};

class AmmoGoodie : public Goodie
{
public:
    AmmoGoodie(StudentWorld* world, int startX, int startY): Goodie(world, startX, startY, IID_AMMO, 100){}
};


#endif // ACTOR_H_
