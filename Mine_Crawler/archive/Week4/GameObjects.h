#pragma once
#include "Levels.h"
//=================================================================================
class GameObject
{
public:
	int x, y;
	int tileType = 0;
	bool isActive = false;
	SDL_Texture* spriteTexture = nullptr;
	SDL_Rect srcRect = { 0,0,0,0 };
	SDL_Rect destRect = { 0,0,0,0 };
	SDL_Renderer* renderer = nullptr;
	void render();	

	bool getIsActive() { return (isActive); };
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

	// Getters & Setters
	void setType(int pType);
	int getType();
	void setSpeed(float pSpeed);
	void setState(int pState);
	int getState();
	void setHeading(int pDir);
	int getHeading();
	void setTarget(int x, int y);
	void setPosition(int x, int y);
	int getResources();
	void addResource(int pResAmount);
	

private:
		
	int state = 1;	 // states 0: Idle 1: Roam 2: Wander: 3:chase Target
	int resources = 0;
	bool isBusy = false;
	float speed = 1;
	float heading = 0;
	float nextMoveTime = 0;
	// Board Positions
	int targetX = 55, targetY = 5, nextX = 0, nextY = 0;
	int currentX = 0, currentY = 0;		
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
	void createUnits(Level* pMap, int pSpawnAmount, int pInitialState) ;
	void update(Level* pMap, int targetX, int targetY, bool MousePressed);
	void manageSquad(Level* pMap);	
	int getResources();
	void addResource(int pResAmount);
	const char* getName();
	int getActiveUnits();
	void addActiveUnits(int pUnitsAdded);
	const int getMaxUnits();

private:
	const char* name = "team";
	int tileType = 0; // number on Board Array
	const int maxUnits = 50;
	int activeUnits = 0;
	int resources = 30;
	int goal = 0; // explore | chase | defend |   balanced
	int	homeX = -10, homeY = -10;
	float nextUpdateTime = 0;
	float updateDelay = 3; // time in seconds Game squad states are checked
	float nextSpeedUpdate = 0;
	float speedUpdateDelay = 10;
	float nextSpawnTime = 5000;
	float spawnUpdateDelay = 3;

	// create 50 units for this squad
	Character* units[50] = {};
};