#pragma once
//#include "Game.h"
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
	void patrol(Level* pMap);

	// Getters & Setters
	void setType(int pType);
	int getType();
	void setSpeed(float pSpeed);
	void setState(int pState);
	int getState();
	void setHeading(int pDir);
	int getHeading();
	void setTarget(int x, int y);

private:
	
	// states 0: Idle 1: Roam 2: Wander
	int state = 1;	
	// Movement
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

