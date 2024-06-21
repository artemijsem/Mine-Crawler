#include"GameObjects.h"
#include"TextureManager.h"
#include"Levels.h"


// Things to Add
//  
// FS States
// GOAP


//GameObject::GameObject(const char* textureSheet, int xPos, int yPos)
//{
//	spriteTexture = TextureManager::LoadTexture(textureSheet);
//	x = xPos;
//	y = yPos;
//
//	// Source Tilesheet pos and size
//	srcRect.h = SPRITE_TEX_SIZE;
//	srcRect.w = SPRITE_TEX_SIZE;
//	srcRect.x = 0;
//	srcRect.y = 0;
//
//	// Target Sprite Size & pos
//	destRect.h = SPRITE_SIZE;
//	destRect.w = SPRITE_SIZE;
//
//}//---


//=================================================================================



void GameObject::render()
{
	destRect.x = x;
	destRect.y = y;

	// needs the w and h setting

	SDL_RenderCopy(Game::renderer, spriteTexture, &srcRect, &destRect);

}//----


//=================================================================================


void Character::update()
{
	//simple chase to target 
	if (targetX > x) nextX++;
	if (targetY > y) nextY++;
	if (targetX < x) nextX--;
	if (targetY < y) nextY--;

	
}//---

void Character::setTarget(int sentX, int sentY)
{
	targetX = sentX;
	targetY = sentY;
}

Character::Character(int pX, int pY)
{
	currentX = pX;
	currentY = pY;
}//---



//=================================================================================


Item::Item(const char* textureSheet, int pX, int pY, int pSpriteSize)
{
	spriteTexture = TextureManager::LoadTexture(textureSheet);

	// Source Tilesheet pos and size
	srcRect.h = SPRITE_TEX_SIZE;
	srcRect.w = SPRITE_TEX_SIZE;
	srcRect.x = 0;
	srcRect.y = 0;

	// Target Sprite Size & pos
	destRect.h = pSpriteSize;
	destRect.w = pSpriteSize;

}//---


//=================================================================================
