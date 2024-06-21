#include"GameObjects.h"
#include"TextureManager.h"



//=================================================================================

void GameObject::render()
{
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopy(Game::renderer, spriteTexture, &srcRect, &destRect);
}//----

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


void Character::update(Level* pMap)
{
	// Call Behaviours based on state 

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
		chaseTarget2(pMap);
		break;
	case 4: // RandomMove
		avoid(pMap);
		break;

	}

}//---

//=================================================================================
// Constructor
Character::Character(int pX, int pY)
{
	currentX = pX;
	currentY = pY;
}//---


//=================================================================================

void Character::setPosition(int pX, int pY)
{
	currentX = pX;
	currentY = pY;
}//---


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

int Character::getTargetX()
{
	return targetX;
}

int Character::getTargetY()
{
	return targetY;
}

int  Character::getResources() 
{
	return resources;
}

void Character::addResource(int pResAmount)
{ 
	resources += pResAmount; 
}




//=================================================================================


void Character::avoid(Level* pMap)
{
	if (!isBusy)
	{
		// Create Random Direction
		if (nextMoveTime < SDL_GetTicks())
		{

			std::cout << "\n trying to avoid";

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


			// set back to chase;
			state = 3;
		}
	}





}//---




void Character::chaseTarget2(Level* pMap)
{
	if (!isBusy)
	{
		if (nextMoveTime < SDL_GetTicks())
		{
			// Check if we are close to target
			if (abs(targetX - currentX) < 3 && abs(targetY - currentY) < 3)
			{
				setState(0);  // set to idle / stop chasing
			}
			else // we are not close
			{
				// Use Trig to calculate the next tile in a straight line to target 

				// Calculate angle to target
				int dX = targetX - currentX;
				int dY = targetY - currentY;
				float angle = atan2(dY, dX); // this is in radians

				// Calcuate next X & Y				
				nextX = currentX + round(1.4 * cos(angle));
				nextY = currentY + round(1.4 * sin(angle));
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

				if (pMap->getTile(nextX, nextY) > 4 && pMap->getTile(nextX, nextY) != tileType)
				{
					// Clear Old position and set new
					pMap->setTile(currentX, currentY, 0);
					pMap->setTile(nextX, nextY, tileType);

					// Update Current Position
					currentX = nextX;
					currentY = nextY;

					state = 666;

					
				

					
				}

				// avoid somehow
				if (state != 0) state = 4;



			}

			// Update Move coolDown
			nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;
		}		
	}
}//---



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
				setState(0);  // set to idle / stop chasing
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

			// Check if next Position is Empty or Resource
			if (pMap->getTile(nextX, nextY) == 0 || pMap->getTile(nextX, nextY) == 3)
			{
				if (pMap->getTile(nextX, nextY) == 3)
				{
					std::cout << "\n resource found";
					resources++;
				}

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





//=================================================================================

// Squad Methods

Squad::Squad(const char* pName, int pType, int pHomeX, int pHomeY)// Constructor
{
	// Set display Name
	name = pName;
	tileType = pType;
	homeX = pHomeX;
	homeY = pHomeY;

	// Create unit Object but do not activate them
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		units[i] = new Character(-10, -10); // Set Position off the board
	}

	std::cout << "\n Squad Created\n";
	std::cout << name << " Tile type =" << tileType << std::endl;
}//----

void Squad::createUnits(Level* pMap, int pSpawnAmount, int pInitialState)
{
	if (pSpawnAmount < maxUnits)
	{
		// Create the initial starting units staring at Home X Y
		for (int i = 0; i < pSpawnAmount; i++)
		{
			// Create Instance & starting positions and spacing
			int xPos = homeX + i;
			int yPos = homeY + i;

			// Set initial values
			units[i]->setUnitID(tileType * 100 + i + 1);
			units[i]->setType(tileType); // map tile type 
			units[i]->setSpeed(3);
			units[i]->setState(pInitialState);
			units[i]->setHeading(rand() % 8); // random initial Direction

			// is the spawn postion empty and do we have resources 
			if (pMap->getTile(xPos, yPos) == 0)
			{
				if (resources > 0)
				{
					pMap->setTile(xPos, yPos, tileType);
					units[i]->isActive = true;
					units[i]->setPosition(xPos, yPos);
					std::cout << units[i]->getUnitID() << " unit spawned  ";

					// Update Squad Stats
					activeUnits++;
					resources--;
				}
				else
				{
					std::cout << " \n Not enough resources to create a new unit";
				}
			}
			else
			{
				std::cout << "Tile occupied, unit: " << i << " not created" << std::endl;
				units[i]->isActive = false;
			}
		}
	}
}//---

//=================================================================================


//  Squad behaviours
void Squad::manageSquad()
{
	// Only check these states every delay time
	if (SDL_GetTicks64() > nextUpdateTime)
	{
		nextUpdateTime += updateDelay * 1000;

		// Loop through all units
		for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
		{
			if (units[i]->getIsActive()) // only check active units
			{

				// ------------- Resources
				//check Unit Resources and Add to squad resources if any found
				int resourcesFound = units[i]->getResources();
				if (resourcesFound > 0)
				{
					resources += resourcesFound; // add to the sqaud resource
					units[i]->addResource(-resourcesFound);
					units[i]->setState(0);
				}
			}

			// ---------------------------- Check for idle units
			if (units[i]->getState() == 0)
			{
			//	std::cout << std::endl << getName() << " UNIT: " << i << " is idle";
			}


			// ---------------------------- XXXXXXXX 
			// something else only needed to be checked periodlically
		}
	}
	
}//---



//=================================================================================

void Squad::update(Level* pMap, int pTargetX, int pTargetY, bool pLMouse)
{
	manageSquad();

	// Update Active units
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		if (units[i]->getIsActive()) units[i]->update(pMap);
	}
}//-----


//=================================================================================
// Squad Getters and Setters 


void Squad::setTarget(int pUnitID, int pTileX, int pTileY)
{
	std::cout << "\n setting unit: " << pUnitID << " to " << pTileX << " " << pTileY;

	if (units[pUnitID]->getIsActive())
	{
		units[pUnitID]->setState(3); // set to chase
		units[pUnitID]->setTarget(pTileX, pTileY);
	}
}


int Squad::getResources()
{
	return resources;
}

void Squad::addResource(int pResAmount)
{
	resources += pResAmount;
}
const char* Squad::getName()
{
	return name;
}




int Squad::getActiveUnits()
{
	return activeUnits;
}
void Squad::addActiveUnits(int pUnitsAdded)
{
	activeUnits += pUnitsAdded;
}


//=================================================================================
// Squad Getters and Setters 
int Squad::findUnit(int pTileX, int pTileY)
{
	int unitFound = -1;

	// Update Active units
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		if (units[i]->getIsActive())
		{
			if (units[i]->getCurrentX() == pTileX && units[i]->getCurrentY() == pTileY)
			{
				unitFound = units[i]->getUnitID(); // use getter for unit ID
				break;
			}
		}
	}

	return (unitFound);
}

Character* Squad::getUnit(int pUnitID)
{
	return units[pUnitID];
}
