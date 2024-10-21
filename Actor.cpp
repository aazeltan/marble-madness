#include "Actor.h"
#include "StudentWorld.h"

// Function to try to kill the actor by reducing its hit points
bool Actor::tryToBeKilled(int damageAmt) {
    decHitPoints(damageAmt);
    if (getHitPoints() <= 0) {
        setHitPoints(0);
        setDead();
        return true;
    }
    return false;
}

// Function to move the agent if possible
bool Agent::moveIfPossible(){
    int curX = getX();
    int curY = getY();
    
    int newX = curX;
    int newY = curY;
    switch (getDirection()) {
        case up:
            newY++;
            break;
        case down:
            newY--;
            break;
        case left:
            newX--;
            break;
        case right:
            newX++;
            break;
        default:
            // Invalid direction
            return false;
    }
    // Check if the agent can push marbles or if there's a player at the new position
    if ((!canPushMarbles() && !getWorld()->playerHere(newX, newY)) || canPushMarbles()){
        if(getWorld()->canAgentMoveTo(this,curX,curY,newX-curX, newY-curY)){
            moveTo(newX,newY);
            return true;
        }
    }
    return false;
}

// Function to shoot a pea in the direction the agent is facing
void Agent::shootPea(){
    int new_x = getX();
    int new_y = getY();
    switch (getDirection())
    {
        case right:
            new_x += 1;
            break;
        case left:
            new_x -= 1;
            break;
        case up:
            new_y += 1;
            break;
        case down:
            new_y -= 1;
            break;
        default:
            break;
    }
    getWorld()->playSound(shootingSound());
    getWorld()->addActor(new Pea(getWorld(), new_x, new_y, getDirection()));
}

// Function to handle player actions
void Player::doSomething(){
    if (!isAlive())
        return;
    
    int ch = 0 ;
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
            // Handle player movement and shooting actions
            case KEY_PRESS_LEFT:
                setDirection(left);
                moveIfPossible();
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                moveIfPossible();
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                moveIfPossible();
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                moveIfPossible();
                break;
            case KEY_PRESS_SPACE:
                if (m_ammo>0){
                    shootPea();
                    m_ammo --;
                }
                break;
            case KEY_PRESS_ESCAPE:
                setDead();
        }
    }
    return;
}

// Function to handle damage taken by the player
void Player::damage(int damageAmt){
    if (tryToBeKilled(damageAmt)) {
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
    else {
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    }
};

// Constructor for the Robot class
Robot::Robot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score, int startDir): Agent(world, startX, startY, imageID, hitPoints, startDir), m_score(score){
    
    // Calculate ticks based on the current level
    m_ticks = (28 - getWorld()->getLevel())/4;
    if (m_ticks<3) m_ticks = 3;
    m_curTicks = 0;
}

// Function to handle Robot actions
void Robot::doSomething(){
    // If the robot is not alive, do nothing
    if (!isAlive())
        return;
    
    // Increment the current ticks count
    m_curTicks ++;
    // If current ticks count is not equal to the ticks required, return
    if (m_curTicks != m_ticks){
        return;
    }
    
    // If the robot cannot shoot, try moving; otherwise, shoot
    if (!robotShoot()){robotMove();}

    // Reset the current ticks count
    m_curTicks = 0;
}

// Function to handle damage taken by the robot
void Robot::damage(int damageAmt){
    // Check if the robot can be killed with the given damage amount
    if (tryToBeKilled(damageAmt)) {
        // Mark the robot as dead and play robot die sound
        setDead();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        // Depending on the robot type, perform specific actions (either do nothing or drop a goodie)
        robotDamage(); //either do nothing (ragebot) or drop goodie (thiefbot)
        // Increase player score by the robot's score
        getWorld()->increaseScore(m_score);
    }
    else {
        // Play robot impact sound if robot is damaged but not killed
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    }
}

// Function for robots to shoot a pea in their direction
bool Robot::robotShoot(){
    // If the robot cannot shoot, return false
    if (!isShootingRobot())
        return false;
    
    // Calculate the change in x and y coordinates based on the robot's direction
    int dx =  0;
    int dy = 0;
        
    switch (getDirection()) {
        case up:
            dy++;
            break;
        case down:
            dy--;
            break;
        case left:
            dx--;
            break;
        case right:
            dx++;
            break;
        default:
            break;
    }
    
    // Check if there's a clear shot to the player and shoot if possible
    if(getWorld()->existsClearShotToPlayer(getX(), getY(),dx, dy, getDirection())){
        shootPea();
        return true;
    }
    return false;
}

// Function to handle movement of RageBot
void RageBot::robotMove(){
    // If moving is not possible, set direction to opposite; otherwise, move
    if (!moveIfPossible())
        setDirection((getDirection() + 180) % 360);
}

// Function to handle movement of ThiefBot
void ThiefBot::robotMove() {
    int r = randInt(1, 10);
    // Check if there's a stealable item colocated with the ThiefBot and attempt to steal
    Actor* stealable = getWorld()->getColocatedStealable(getX(), getY());
    if (stealable != nullptr && g_type == ThiefBot::NONE && r== 1) {

        m_goodie = stealable;

        int score = stealable->getScore();
        if (score == 1000)
            g_type = ThiefBot::EXTRLIFE;
        if (score == 500)
            g_type = ThiefBot::RESHP;
        if (score == 100)
            g_type = ThiefBot::AMMO;

        // Make the stealable item die
        stealable->setDead();
        // Play munch sound
        getWorld()->playSound(SOUND_ROBOT_MUNCH);
        return;
    }
    
    // Check if the ThiefBot has not yet moved distanceBeforeTurning squares in its current direction
    if (m_dist != 0 && moveIfPossible()) {
        m_dist--;
        return;
    }
    
    // Reset the distance and direction flags
    m_dist = randInt(1, 6);
    
    // Try moving in a random direction until the ThiefBot moves or exhausts all options
    for (int i = 0; i < 4; ++i) {
        int d = 90 * randInt(0, 3);
        setDirection(d);
        if (moveIfPossible()) {
            return; // Successfully moved, so exit the function
        }
    }
}

// Function to handle ThiefBot damage actions
void ThiefBot::robotDamage(){
    // Depending on the type of goodie stolen, drop the corresponding goodie
    switch (g_type) {
    case NONE:
        break;
    case EXTRLIFE:
        getWorld()->addActor(new ExtraLifeGoodie(getWorld(), getX(), getY()));
        break;
    case RESHP:
        getWorld()->addActor(new RestoreHealthGoodie(getWorld(), getX(), getY()));
        break;
    case AMMO:
        getWorld()->addActor(new AmmoGoodie(getWorld(), getX(), getY()));
        break;
    }
}

// Function to handle Exit actions
void Exit::doSomething() {
    // If the exit is not in play, check if any crystals are present
    if (!isInPlay()) {
        if (getWorld()->anyCrystals()) {
            // If crystals are present, reveal the exit and play the reveal exit sound
            setInPlay(true);
            getWorld()->playSound(SOUND_REVEAL_EXIT);
        }
        return;
    } else {
        // If the exit is in play and player is colocated with it, finish the level
        if (getWorld()->playerHere(getX(), getY())) {
            getWorld()->setLevelFinished();
        }
    }
}

// Function to handle Pit actions
void Pit::doSomething(){
    // If the pit is not alive, do nothing
    if (!isAlive())
        return;
    
    // If a marble is present at the pit's location, mark the pit as dead
    if (getWorld()->marbleHere(getX(), getY()))
        setDead();
}

// Function to handle actions of a Pea
void Pea::doSomething(){
    // If the pea is not alive, do nothing
    if (!isAlive())
        return;
    
    // Check if the pea damages something and handle accordingly
    if (getWorld()->damageSomething(this, 2))
    {
        // If the pea damages something, mark it as dead and invisible
        setDead();
        setVisible(false);
        return;
    }
    
    // Calculate new coordinates for the pea based on its direction
    int curX = getX(), curY = getY();
    int newX = curX, newY = curY;
    
    switch (getDirection())
    {
        case right:
            newX += 1;
            break;
        case left:
            newX -= 1;
            break;
        case up:
            newY += 1;
            break;
        case down:
            newY -= 1;
            break;
        default:
            break;
    }
    
    // Check if there's another pea or player at the new position
    if (getWorld()->peaHere(newX, newY) || getWorld()->playerHere(newX, newY)){
        // If so, make the pea invisible and move it to the new position without damaging
        setVisible(false);
        moveTo(newX, newY);
        return;
    }
    
    // Move the pea to the new position
    moveTo(newX, newY);
}

// Function to handle actions of Marbles
void Marble::doSomething(){
    // If the marble is not alive, do nothing
    if (!isAlive())
        return;
    
    // If the marble's hit points reach 0 or below, mark it as dead
    if (getHitPoints() <= 0)
        setDead();
}

// Function to handle pushing of a marble by an agent
bool Marble::bePushedBy(Agent* a, int x, int y) {
    // If the agent cannot push marbles, return false
    if (!a->canPushMarbles())
        return false;
    
    // If the marble cannot move to the new location, do nothing
    if (!getWorld()->canMarbleMoveTo(x, y))
        return false;

    // Push the marble to the new location
    moveTo(x, y);
    return true;
}

// Function to handle actions of ThiefBotFactory
void ThiefBotFactory::doSomething(){
    int num;
    
    // Check the number of thiefbots present in the factory's vicinity
    if (!getWorld()->doFactoryCensus(getX(), getY(), 3, num)){
        return;
    }
    
    // If the number of thiefbots is less than 3, create a new thiefbot with a random chance
    if (num < 3) {
        int r = randInt(1,50);
        if (r == 1){
            // Create a new thiefbot depending on the factory's type (mean or regular)
            if (m_type == MEAN){
                getWorld()->addActor(new MeanThiefBot(getWorld(), getX(),getY()));
            }else if (m_type == REGULAR){
                getWorld()->addActor(new RegularThiefBot(getWorld(), getX(), getY()));
            }
            // Play the robot born sound
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
}

// Function to handle actions of PickupableItems (e.g., ExtraLife, RestoreHealth, Ammo, Crystals)
void PickupableItem::doSomething(){
    // If the item is not alive, do nothing
    if (!isAlive())
        return;
    
    // If the player is colocated with the item, perform corresponding actions based on the item's score
    if (getWorld()->isPlayerColocatedWith(getX(), getY())){
        // Increase player score and play got goodie sound
        getWorld()->increaseScore(m_score);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        // Perform actions based on the item's score
        if (m_score == 1000)
            getWorld()->increaseLife();
        if (m_score==500)
            getWorld()->restorePlayerHealth();
        if (m_score == 100)
            getWorld()->increaseAmmo();
        if (m_score == 50)
            getWorld()->decCrystals();
        // Mark the item as dead
        setDead();
    }
}

