#include"GameObjects.h"
#include"TextureManager.h"
#include "Levels.h"



//=================================================================================

void GameObject::render()
{
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopy(Game::renderer, spriteTexture, &srcRect, &destRect);
}//----


//=================================================================================


void Character::update(Level* pMap)
{
	switch (state)
	{
	case 0: // Idle
		break;
	case 1: // Roam
		roam(pMap);
		break;
	case 2: // Wander
		wander(pMap);
		break;
	case 3: // Chase Target
		chaseTarget(pMap);
		break;
	case 4: // Patrol
		patrol(pMap);
		break;
	}

}//---

//=================================================================================

Character::Character(int pX, int pY)
{
	currentX = pX;
	currentY = pY;
}//---
//=================================================================================
void  Character::setType(int pType)
{
	tileType = pType;
}//---

int Character::getType()
{
	return tileType;
}//-----

void Character::setSpeed(float pSpeed)
{
	speed = pSpeed;
}

void Character::setState(int pState)
{
	state = pState;
}

int Character::getState()
{
	return state;
}

void Character::setHeading(int pDirection)
{
	heading = pDirection;
}
int Character::getHeading()
{
	return heading;
}

void Character::setTarget(int pTargetX, int pTargetY)
{
	targetX = pTargetX;
	targetY = pTargetY;
}

//=================================================================================
void Character::chaseTarget(Level* pMap)
{
	if (!isBusy)
	{
		if (nextMoveTime < SDL_GetTicks())
		{
			// Check if we are close to target
			if (abs(targetX - currentX) < 3 && abs(targetY - currentY) < 3)
			{
				setState(4);  // set to idle / stop chasing
			}
			else // we are not close
			{
				// Basic Chase
				if (currentX < targetX)
					nextX = currentX + 1;
				else
					nextX = currentX - 1;

				if (currentY < targetY)
					nextY = currentY + 1;
				else
					nextY = currentY - 1;
			}

			// Check if next Position is Empty
			if (pMap->getTile(nextX, nextY) == 0)
			{
				// Clear Old position and set new
				pMap->setTile(currentX, currentY, 0);
				pMap->setTile(nextX, nextY, tileType);

				// Update Current Position
				currentX = nextX;
				currentY = nextY;
			}

			// Update Move coolDown
			nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;
		}
	}
}//---


//=================================================================================

void Character::roam(Level* pMap)
{// Move to Random Adjacent Tile
	if (!isBusy)
	{
		// Create Random Direction
		if (nextMoveTime < SDL_GetTicks())
		{
			// Direction: 0 up 2: right: 4: down 6: left 
			int direction = rand() % 8;

			switch (direction)
			{
			case 0: // Up
				nextX = currentX;
				nextY = currentY - 1;
				break;
			case 1: // up Right
				nextX = currentX + 1;
				nextY = currentY - 1;
				break;
			case 2: // Right
				nextX = currentX + 1;
				nextY = currentY;
				break;
			case 3: // Right Down
				nextX = currentX + 1;
				nextY = currentY + 1;
				break;
			case 4: // Down
				nextX = currentX;
				nextY = currentY + 1;
				break;
			case 5: // Down Left
				nextX = currentX - 1;
				nextY = currentY + 1;
				break;
			case 6: // Left
				nextX = currentX - 1;
				nextY = currentY;
				break;
			case 7: // Up Left
				nextX = currentX - 1;
				nextY = currentY - 1;
				break;
			}

			// Check if next Position is Empty
			if (pMap->getTile(nextX, nextY) == 0)
			{
				// Clear Old position and set new
				pMap->setTile(currentX, currentY, 0);
				pMap->setTile(nextX, nextY, tileType);

				// Update Current Position
				currentX = nextX;
				currentY = nextY;
			}

			// Update Move coolDown
			nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;
		}
	}
}//---
//=================================================================================

void Character::wander(Level* pMap)
{
	// Move to Next Tile based on  heading
	if (!isBusy)
	{
		if (nextMoveTime < SDL_GetTicks())
		{
			switch (getHeading())
			{
			case 0: // Up
				nextX = currentX;
				nextY = currentY - 1;
				break;
			case 1: // up Right
				nextX = currentX + 1;
				nextY = currentY - 1;
				break;
			case 2: // Right
				nextX = currentX + 1;
				nextY = currentY;
				break;
			case 3: // Right Down
				nextX = currentX + 1;
				nextY = currentY + 1;
				break;
			case 4: // Down
				nextX = currentX;
				nextY = currentY + 1;
				break;
			case 5: // Down Left
				nextX = currentX - 1;
				nextY = currentY + 1;
				break;
			case 6: // Left
				nextX = currentX - 1;
				nextY = currentY;
				break;
			case 7: // Up Left
				nextX = currentX - 1;
				nextY = currentY - 1;
				break;
			}

			// Check if next Position is Empty
			if (pMap->getTile(nextX, nextY) == 0)
			{
				// Clear Old position and set new
				pMap->setTile(currentX, currentY, 0);
				pMap->setTile(nextX, nextY, tileType);

				// Update Current Position
				currentX = nextX;
				currentY = nextY;
			}
			else
			{
				// New Heading
				setHeading(rand() % 8);
			}

			// Update Move coolDown
			if (getHeading() == 1 || getHeading() == 3 || getHeading() == 5 || getHeading() == 7) // Diaganol
			{
				// delay is root 2 longer (1.414)
				nextMoveTime = SDL_GetTicks() + 1000 * 1.414 / speed;
			}
			else
			{
				nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;
			}
		}
	}
}//---

void Character::patrol(Level* pMap)
{
	if (!isBusy)
	{

		if (nextMoveTime < SDL_GetTicks())
		{

			if ((abs(targetX - currentX) < 5 && abs(targetY - currentY) < 5))
			{
				roam(pMap);
					
			}
			else
			{
				// Basic Chase
				if (currentX < targetX)
					nextX = currentX + 1;
				else
					nextX = currentX - 1;

				if (currentY < targetY)
					nextY = currentY + 1;
				else
					nextY = currentY - 1;
			}


			// Check if next Position is Empty
			if (pMap->getTile(nextX, nextY) == 0)
			{
				// Clear Old position and set new
				pMap->setTile(currentX, currentY, 0);
				pMap->setTile(nextX, nextY, tileType);

				// Update Current Position
				currentX = nextX;
				currentY = nextY;

			}


			nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;
		}

	}
}
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
