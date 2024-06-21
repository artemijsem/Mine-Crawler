#pragma once
#include "Levels.h"
#include "AI.h"
#include"Game.h"
//=================================================================================
class GameObject
{
public:
	float x = -1, y = -1;
	int tileType = 0;
	bool isActive = false;
	SDL_Texture* spriteTexture = nullptr;
	SDL_Rect srcRect = { 0,0,0,0 };
	SDL_Rect destRect = { 0,0,0,0 };
	SDL_Renderer* renderer = nullptr;
	void render();
	bool getIsActive() { return (isActive); };
	
	Vector2 pos;
};


//=================================================================================
class Character : public GameObject
{
public:
	Character(int pX, int pY); // Set Inital Pos
	void update(Level* pMap);
	void roam(Level* pMap);
	void wander(Level* pMap);
	void chaseTarget(Level* pMap);
	void avoid(Level* pMap);
	void attackTarget(Level* pMap);
	void drawHPBar(Level* pMap);
	void moveToTarget(Level* pMap);

	// Getters & Setters
	void setType(int pType);
	int getType();
	void setSpeed(float pSpeed);
	void setState(int pState);
	int getState();
	void setHeading(int pDir);
	int getHeading();
	void setTarget(Level* pMap, int x, int y);
	void setPosition(int x, int y);
	int getResources();
	void addResource(int pResAmount);	
	int getCurrentX() { return currentX; };
	int getCurrentY() { return currentY; };
	void setUnitID(int pID) { unitID = pID; }
	int getUnitID() { return unitID; };
	int getTargetID() { return targetID; }
	void setTargetID(int pInt) { targetID = pInt; }
	void setHealth(float healthChange) { health += healthChange; };
	float getHealth() { return health; };
	void setArmedState(bool newArmedSrate) { isActive = newArmedSrate; };
	float getTargetDistance() { return targetDistance; };
	void setTargetDistance(float pDist) { targetDistance = pDist; }
	float getChaseRange() { return chaseRange; }
	float getAttackRange() { return attackRange; }
	

	// Group
	void setLastState(int pState) { lastState = pState; }
	int getLastState() { return lastState; }
	bool getIsLeader() { return isGroupLeader; }
	void setAsLeader(bool pState) { isGroupLeader = pState; }
	void setGroup(int pGroup) { group = pGroup; }
	int getGroup() { return group; }

	// Paths

private:
	int unitID = -1;
	int state = 1;	 // states 0: Idle 1: Roam 2: Wander: 3:chase Target
	int resources = 0;
	bool isBusy = false;
	float speed = 2;
	int heading = 0;
	float nextMoveTime = 0;		
	int targetX = 25, targetY = 5, nextX = 0, nextY = 0;
	int currentX = 0, currentY = 0;
		 
	// combat
	bool isInRangeOfEnemy = false;
	float chaseRange = 12;
	float attackRange = 8;

	int targetID = 0;
	float targetDistance = 1000;
	bool isArmed = true;
	float health = 100;
	float hpMax = 100;
	float attack = 30;

	// Groups
	int lastState = 0;
	int group = 0;
	bool isGroupLeader = false;
	int groupLeader = -1;
	float chaseStopDistance = 2;

	// Path finding
	std::list<Vector2> pathToTarget;
	std::list<int> wpsToTarget;
	int currentWP = 0;
	int targetWP = 0;
	Vector2 nextwayPoint = {};
	Vector2 mainTarget = {};


};


//=================================================================================

class Attack : public GameObject
{
public:
	Attack(const char* pTextureSheet, int pX, int pY, int pSpriteSize);
	void spawn(int pX, int pY, float pAngle, Level* pMap);
	void update(Level* pMap);
	float getRange() { return range; }
	float getDamage() { return damage; }

private:
	Vector2 velocity = {};
	float damage = 30;
	float angle = 0;
	float range = 20;
	float speed = 6;
	float timeToDeath = 0;
};


//=================================================================================

class Item : public GameObject
{
public:
	Item(const char* pTextureSheet, int pX, int pY, int pSpriteSize);	
private:
};

//=================================================================================
class Squad
{
public:
	Squad(const char* name, int type, int homeX, int homeY);
	void createUnits(Level* pMap, int pSpawnAmount, int pInitialState);
	void update(Level* pMap);
	void manageSquad(Level* pMap);
	int getResources();
	void addResource(int pResAmount);
	const char* getName();
	int getActiveUnits();
	void addActiveUnits(int pUnitsAdded);
	int findUnit(int pTileX, int pTileY);
	void setTarget(Level* pMap, int pUnitID, int pTileX, int pTileY);
	void drawUnitHP(Level*pMap );


	// Unit getters and Setters
	float getUnitHealth(int unitID) { return (units[unitID]->getHealth()); };
	std::list<int> getActiveUnitsIDs();
	Vector2 getUnitPosition(int UnitID);
	void setUnitTarget(Level* pMap , int pUnitID, int pTargetUniID) {units[pUnitID]->setTargetID(pTargetUniID);	}
	int getUnitTarget(int pUnitID)  { return  units[pUnitID]->getTargetID(); }
	void setUnitTargetDistance(int pUnitID, float pDist) {	units[pUnitID]->setTargetDistance(pDist);}
	void modifyHealth(int pUnit, float pDamage) { units[pUnit]->setHealth(pDamage); }
	void setUnitState(int pUnitID, int pState) { units[pUnitID]->setState(pState); }

	float getUnitChaseRange(int pUnitID) {return units[pUnitID]->getChaseRange(); }
	float getUnitAttackRange(int pUnitID) { return units[pUnitID]->getAttackRange(); }

	// Groups 
	
	void setGroup1Goal(int pGoal) { group1Goal = pGoal; }
	int getGroup1Goal() { return group1Goal;}
	void assignGroup1(std::list<int> pUnits);
	std::list<int> getGroup1() { return group1; }
	
	void setGroupLeader(int pUnitID, bool pState) { units[pUnitID]->setAsLeader(pState); }
	void setUnitGroup(int pUnitID, int pGroup) { units[pUnitID]->setGroup(pGroup); }
	int getUnitGroup(int pUnitID) { return ( units[pUnitID]->getGroup() ); }

	Vector2 group1LeaderPos = {};

private:
	const char* name = "team";
	int squadID = 0;
	int tileType = 0; // number on Board Array
	const int maxUnits = 50;
	int activeUnits = 0;
	int resources = 30;
	int goal = 0; // explore | chase | defend |   balanced
	int	homeX = -10, homeY = -10;
	float nextUpdateTime = 0;
	float updateDelay = 3; // time in seconds Game squad states are checked 
	
	// create 50 units for this squad
	Character* units[50] = {};

	// Sub Groups of squad
	std::list<int> group1 = {};
	std::list<int> group2 = {};	
	
	int group1Goal = 0;
	int group2Goal = 0;

	int group1Formation = 1;		
};