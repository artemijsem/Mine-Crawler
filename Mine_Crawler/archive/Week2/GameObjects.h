#pragma once
#include "Game.h"

//=================================================================================

class GameObject
{
public:
	int x, y;
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
	
	// texture passed and postion to set initially
	Character(int pX, int pY);

	void update();
	void setTarget(int sentX, int sentY);

private:
	bool isBusy = false;


	// Board Positions
	int targetX = 0, targetY = 0, nextX = 0, nextY = 0;
	int currentX = 0, currentY = 0;
	
	float speed = 1;
	float nextMoveTime = 0;
};

//=================================================================================


class Item : public GameObject
{
public:
	Item(const char* pTextureSheet, int pX, int pY, int pSpriteSize);

private:

};

//=================================================================================

