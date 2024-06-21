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
	void flee(Level* pMap);
	//void attackTarget(Level* pMap);
	void drawHPBar(Level* pMap);
	void moveToTarget(Level* pMap);
	void manageHealth();

	// Getters & Setters
	void setName(std::string pName) { name = pName; }
	std::string getName() { return name; };
	void setType(int pType);
	int getType();
	void setSpeed(float pSpeed);
	void setState(int pState);
	int getState();
	void setHeading(int pDir);
	int getHeading();
	void setTarget(Level* pMap, int x, int y);
	void setPosition(int x, int y);
	int getKeys();
	void addKey(int pAmount);	
	int getCurrentX() { return currentX; };
	int getCurrentY() { return currentY; };
	Vector2 getCurrentPos() { return currentPos; };
	int getHomeX() { return homeX; };
	void setHomeX(int pHomeX) { homeX = pHomeX; }
	int getHomeY() { return homeY; };
	void setHomeY(int pHomeY) { homeY = pHomeY; }
	void setUnitID(int pID) { unitID = pID; }
	int getUnitID() { return unitID; };
	int getTargetID() { return targetID; }
	void setTargetID(int pInt) { targetID = pInt; }
	int getTargetX() { return targetX; }
	int getTargetY() { return targetY; }
	void setHealth(float healthChange) { health += healthChange; };
	float getHealth() { return health; };
	void setMaxHealth(float pMaxHealth) { hpMax = pMaxHealth; };
	float getMaxHealth() { return hpMax; };
	void setArmedState(bool newArmedSrate) { isActive = newArmedSrate; };
	float getTargetDistance() { return targetDistance; };
	void setTargetDistance(float pDist) { targetDistance = pDist; }
	float getChaseRange() { return chaseRange; }
	float getAttackRange() { return attackRange; }
	void setAttackRange(float pAttackRange);
	void setAttack(float pAttack);
	float getAttack() { return attack; }
	void setMorale(float pMorale);
	void addMorale(float pMorale);
	float getMorale() { return morale; }
	void setIsBusy(bool pBool) { isBusy = pBool; }
	bool getIsBusy() { return isBusy; }
	

	// Paths
	void followPath(Level* pMap);
	void setWPList(std::list<int> pList) { wpsToTarget = pList; };


private:
	std::string name = "text";
	int unitID = -1;
	int state = 1;
	int lastState = 0;
	int keys = 0;
	bool isBusy = false;
	float speed = 2;
	int heading = 0;
	float nextMoveTime = 0;		
	int targetX = 25, targetY = 5, nextX = 0, nextY = 0;
	int currentX = 0, currentY = 0;
	int homeX = 0, homeY = 0;
	Vector2 currentPos = {  currentX, currentY };
		 
	// combat
	bool isInRangeOfEnemy = false;
	float chaseRange = 12;
	float attackRange = 8;
	float chaseStopDistance = 2;
	float fleeStopDistance = 20;
	float morale = 0;

	int targetID = 0;
	float targetDistance = 1000;
	bool isArmed = true;
	float health = 100;
	float hpMax = 100;
	float attack = 30;


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
	void spawn(int pX, int pY, float pAngle, Level* pMap, float pDamage, int pSquadType);
	void update(Level* pMap);
	float getRange() { return range; }
	float getDamage() { return damage; }
	int getSquadType() { return squadType; }
	void setRange(float pRange);
	void setSpeed(float pSpeed);
	void setDamage(float pDamage);
	void renderEX();

private:
	Vector2 velocity = {};
	float damage = 30;
	float angle = 0;
	float range = 20;
	float speed = 5;
	float timeToDeath = 5;
	int squadType = 0;
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
	Squad(const char* name, int type);
	void createUnits(Level* pMap, int pSpawnAmount, int pInitialState, int pTileType, int pSpawnX, int pSpawnY);
	void update(Level* pMap);
	void manageSquad(Level* pMap);
	int getKeys();
	void addKey(int pAmount);
	const char* getName();
	int getActiveUnits();
	void addActiveUnits(int pUnitsAdded);
	int findUnit(int pTileX, int pTileY);
	void setTarget(Level* pMap, int pUnitID, int pTileX, int pTileY);
	void drawUnitHP(Level*pMap );
	Character* getUnit(int pUnitID);

	// Unit getters and Setters
	float getUnitHealth(int unitID) { return (units[unitID]->getHealth()); };
	float getUnitMaxHealth(int unitID) { return units[unitID]->getMaxHealth(); };
	std::list<int> getActiveUnitsIDs();
	Vector2 getUnitPosition(int UnitID);
	void setUnitTarget(Level* pMap, int pUnitID, int pTargetUniID, Vector2 pTargetPos) { units[pUnitID]->setTargetID(pTargetUniID); units[pUnitID]->setTarget(pMap, pTargetPos.x, pTargetPos.y); }
	int getUnitTarget(int pUnitID)  { return  units[pUnitID]->getTargetID(); }
	int getUnitTargetX(int pUnitID) { return units[pUnitID]->getTargetX(); }
	int getUnitTargetY(int pUnitID) { return units[pUnitID]->getTargetY(); }
	void setUnitTargetDistance(int pUnitID, float pDist) {	units[pUnitID]->setTargetDistance(pDist);}
	void modifyHealth(int pUnit, float pDamage) { units[pUnit]->setHealth(pDamage); }
	void setUnitState(int pUnitID, int pState) { units[pUnitID]->setState(pState); }
	float getUnitChaseRange(int pUnitID) {return units[pUnitID]->getChaseRange(); }
	float getUnitAttackRange(int pUnitID) { return units[pUnitID]->getAttackRange(); }
	int getUnitState(int pUnitID) { return units[pUnitID]->getState(); }
	float getUnitMorale(int pUnitID) { return units[pUnitID]->getMorale(); };
	bool getUnitIsBusy(int pUnitID) { return units[pUnitID]->getIsBusy(); };
	int getUnitHomeX(int pUnitID) { return units[pUnitID]->getHomeX(); };
	int getUnitHomeY(int pUnitID) { return units[pUnitID]->getHomeY(); };
	std::string getUnitName(int pUnitID) { return units[pUnitID]->getName(); };
	float getUnitAttack(int pUnitID) { return units[pUnitID]->getAttack(); };
	std::string getUnitStateString(int pUnitID);





private:
	const char* name = "team";
	int squadID = 0;
	int tileType = 0; // number on Board Array
	const int maxUnits = 50;
	int activeUnits = 0;
	int keys = 0;
	int goal = 0; // explore | chase | defend |   balanced
	int IDoffset = 0;
	float nextUpdateTime = 0;
	float updateDelay = 3; // time in seconds Game squad states are checked 
	
	// create 50 units for this squad
	Character* units[100] = {};
	
};