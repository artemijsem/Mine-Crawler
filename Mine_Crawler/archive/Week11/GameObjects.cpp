#include"GameObjects.h"
#include"TextureManager.h"



//=================================================================================

void GameObject::render()
{
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopy(Game::renderer, spriteTexture, &srcRect, &destRect);
}//----


void Character::drawHPBar(Level* pMap)
{
	if (isActive)
	{
		float currentUnitHealth = getHealth();
		int currentMapSize = pMap->getMapTileSize();
		int xOffSet = pMap->getStartX();
		int yOffset = pMap->getStartY() - currentMapSize / 4;

		// Placement and size of HP Bar above Unit
		destRect.h = 3;
		destRect.w = currentMapSize * currentUnitHealth / 100;
		destRect.x = getCurrentX() * currentMapSize + xOffSet;
		destRect.y = getCurrentY() * currentMapSize + yOffset;

		// Colour of HP Bar
		if (currentUnitHealth >= 50) // Green
			SDL_SetRenderDrawColor(Game::renderer, 0, 200, 00, 255);

		else if (currentUnitHealth >= 25 && currentUnitHealth < 50) // Yellow
			SDL_SetRenderDrawColor(Game::renderer, 250, 250, 0, 255);

		else // Red
			SDL_SetRenderDrawColor(Game::renderer, 250, 0, 0, 255);

		// Draw Rect 
		SDL_RenderDrawRect(Game::renderer, &destRect);
	}
}//---




 //=================================================================================
Attack::Attack(const char* textureSheet, int pX, int pY, int pSpriteSize)
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


void Attack::spawn(int pX, int pY, float pAngle, Level* pMap)
{
	isActive = true;
	x = pX;
	y = pY;
	angle = pAngle;

	float newRange = range / 50 * pMap->getMapTileSize();
	float newSpeed = speed / 50 * pMap->getMapTileSize();

	velocity.x = sin(angle * M_PI / 180) * newSpeed;
	velocity.y = -cos(angle * M_PI / 180) * newSpeed;

	timeToDeath = SDL_GetTicks() + (newRange / newSpeed) * 1000;
}//---


void Attack::update(Level* pMap)
{
	if (isActive)
	{
		// check time left alive
		if (timeToDeath < SDL_GetTicks()) isActive = false;

		// Update Pos
		x += velocity.x;
		y += velocity.y;
	}
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
		moveToTarget(pMap);
		break;
	case 4: // Avoid  / RandomMove
		avoid(pMap);
		break;
	case 5: // Attack
		// if no target find nearest 
		//Atacking
		break;
	case 6: // Harvest
		// 
		break;
	case 7: // Deliver Resource
		// 
		break;
	case 8: // Recharge
		// Atacking
		break;
	case 9: // Find resource
		// Atacking
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



//=================================================================================


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
			// Add a Random number between -1 to 1 to currentX,Y  
			nextX = currentX + (rand() % 3 - 1);
			nextY = currentY + (rand() % 3 - 1);
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
			// set back to last State;
			state = lastState;
		}
	}
}//---


//=================================================================================

void Character::setTarget(Level* pMap, int pTargetX, int pTargetY)
{
	// ensure Target is empty or valid
	if (pMap->getTile(pTargetX, pTargetY) == 0 || pMap->getTile(pTargetX, pTargetY) == 5) // or something else we can move to
	{
		// local Target 
		targetX = pTargetX;
		targetY = pTargetY;

		// Main Target Position saved
		mainTarget.x = pTargetX;
		mainTarget.y = pTargetY;

		// Check Target is in line of sight
		Vector2 targPos = { targetX, targetY };
		Vector2 curPos = { currentX, currentY };

		if (!pMap->checkTargetInLineOfSight(curPos, targPos)) // Target not in sight
		{	
			// get List of  waypoints to Target
			wpsToTarget = pMap->getListOfWaypointsToTarget(curPos, targPos);
			//std::cout << " Path found " << wpsToTarget.size();
		}
	}
}//-----



void Character::moveToTarget(Level* pMap)
{
	if (nextMoveTime < SDL_GetTicks())
	{
		// Update Move coolDown
		nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;

		Vector2 targPos = { mainTarget.x, mainTarget.y };
		Vector2 curPos = { currentX, currentY };
		Vector2 wpPos = {};

		if (!pMap->checkTargetInLineOfSight(curPos, targPos)) // Target not in sight
		{
			if (wpsToTarget.size() > 0) // List of way points exists
			{
				// Get first WP Pos
				wpPos = pMap->getWPPos(wpsToTarget.front());
			//	std::cout << "\n first wp pos" << wpPos.x << wpPos.y;

				// check we are near WP
				if (abs(wpPos.x - currentX) < 1 && abs(wpPos.y - currentY) < 1)
				{
				//	std::cout <<"\n reached WP";
					// remove WP form List
					wpsToTarget.pop_front();
				}
				else // Move to WP
				{
					// Calculate angle to target
					int dX = wpPos.x - currentX;
					int dY = wpPos.y - currentY;
					float angle = atan2(dY, dX); // this is in radians			

					// Calcuate next X & Y				
					nextX = currentX + round(1.4 * cos(angle));
					nextY = currentY + round(1.4 * sin(angle));
				}

				//// Check if next Position is Empty
				if (pMap->getTile(nextX, nextY) == 0)
				{
					// Clear Old position and set new
					pMap->setTile(currentX, currentY, 0);
					pMap->setTile(nextX, nextY, tileType);

					// Update Current Position
					currentX = nextX;
					currentY = nextY;
				}
				else// avoid 
				{					
					if (state != 0)
					{
						lastState = state;
						state = 4;
					}
				}
			}
		}
		else // Target in sight
		{
			//// clear List of Wps
			wpsToTarget.clear();		

			// Check if we are close to target
			if (abs(targetX - currentX) < chaseStopDistance && abs(targetY - currentY) < chaseStopDistance)
			{
				setState(0);  // set to idle / stop chasing
			}
			else // we are not close
			{				
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
			else// avoid 
			{				
				if (state != 0)
				{
					lastState = state;
					state = 4;
				}
			}
		}
	}		
}//-----

//=================================================================================

void Character::chaseTarget(Level* pMap)
{
	if (!isBusy)
	{
		if (nextMoveTime < SDL_GetTicks())
		{

			// Check if we are close to target
			if (abs(targetX - currentX) < chaseStopDistance && abs(targetY - currentY) < chaseStopDistance)
			{
				setState(0);  // set to idle / stop chasing
			}
			else // we are not close
			{
				// Use Trig to calculate the next tile in a straight line to target 
				// Is target in Line of sight  move towards / else find waypoint

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
				// avoid somehow
				/*if (state != 0)
				{
					lastState = state;
					state = 4;
				}*/
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
			// Add a Random number between -1 to 1 to currentX,Y  
			nextX = currentX + (rand() % 3 - 1);
			nextY = currentY + (rand() % 3 - 1);

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
			int yPos = homeY;

			if (xPos > BOARD_WIDTH - 2 || yPos > BOARD_HEIGHT - 2)
			{
				std::cout << " unit off board";
			}
			else // position is on Board
			{
				// Set initial values
				units[i]->setUnitID(tileType * 100 + i + 1);
				units[i]->setType(tileType); // map tile type 
				units[i]->setSpeed(5);
				units[i]->setState(pInitialState);
				units[i]->setHeading(rand() % 8); // random initial Direction
				units[i]->setTarget(pMap, homeX, homeY);

				// is the spawn postion empty and do we have resources 
				if (pMap->getTile(xPos, yPos) == 0)
				{
					if (resources > 0)
					{
						pMap->setTile(xPos, yPos, tileType);
						units[i]->isActive = true;
						units[i]->setPosition(xPos, yPos);
						//std::cout << units[i]->getUnitID() << " unit spawned  ";

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
		std::cout << std::endl;
	}
}//---

//=================================================================================


//  Squad behaviours
void Squad::manageSquad(Level* pMap)
{
	// Loop through all units
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		// Only check these states every delay time
		if (SDL_GetTicks64() > nextUpdateTime)
		{
			nextUpdateTime += updateDelay * 1000;

			if (units[i]->getIsActive()) // only check active units
			{
				// ------------- Resources
				//check Unit Resources and Add to squad resources if any found
				int resourcesFound = units[i]->getResources();
				if (resourcesFound > 0)
				{
					resources += resourcesFound; // add to the sqaud resource
					units[i]->addResource(-resourcesFound);
					//units[i]->setState(2);
				}
			}

			// ---------------------------- Check for idle units
			if (units[i]->getState() == 0)
			{
				//units[i]->setState(2);
			}

			//	send unit home if health is low
			if (units[i]->getHealth() < 20)
			{
				//units[i]->setTarget(homeX, homeY);
				//units[i]->setState(3);
			}
		}

		// SET Group member targets
		int xOffset = 0, yOffset = 1; // for formation position from leader

		if (units[i]->getGroup() == 1)
		{
			if (units[i]->getIsLeader())
			{
				// set the leader pos
				group1LeaderPos = { static_cast<float>(units[i]->getCurrentX()) , static_cast<float>(units[i]->getCurrentY()) };
			}
			else
			{
				units[i]->setTarget(pMap, group1LeaderPos.x + xOffset, group1LeaderPos.y + yOffset);

				xOffset++; // increment 
			}
		}
	}
}//---



//=================================================================================

void Squad::update(Level* pMap)
{
	manageSquad(pMap);

	// Update Active units
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		if (units[i]->getIsActive() && units[i]->getHealth() < 1) // If unit is dead
		{
			units[i]->isActive = false;
			pMap->setTile(units[i]->getCurrentX(), units[i]->getCurrentY(), 0); // Clear Old position on map
			activeUnits--;
		}

		if (units[i]->getIsActive())
		{
			units[i]->update(pMap);
		}
	}
}//-----

//=================================================================================

void Squad::drawUnitHP(Level* pMap)
{
	// Update Active units
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		if (units[i]->getIsActive())
		{
			units[i]->drawHPBar(pMap);
		}
	}

}//---


//=================================================================================
// Squad Getters and Setters 


void Squad::setTarget(Level* pMap, int pUnitID, int pTileX, int pTileY)
{
	// ensure target is valid	
	if (pMap->getTile(pTileX, pTileX) == 0)
	{
		if (units[pUnitID]->getIsActive())
		{
			if (units[pUnitID]->getGroup() != 0) // belongs to a group -- Assuming 1 for example
			{
				if (units[pUnitID]->getGroup() == 1)
				{
					units[pUnitID]->setState(3); // set all units to chase

					if (units[pUnitID]->getIsLeader()) // Leader of a group
					{
						units[pUnitID]->setTarget(pMap, pTileX, pTileY);
					}
				}
			}
			else // unit is not part of a group
			{
				units[pUnitID]->setState(3); // set to Move to Target
				units[pUnitID]->setTarget(pMap, pTileX, pTileY);
			}
		}
	}
}//---


//=================================================================================
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

int Squad::findUnit(int pTileX, int pTileY)
{
	int unitFound = -1;

	// Loop through Active units
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		if (units[i]->getIsActive())
		{
			// does x and y match the unit
			if (units[i]->getCurrentX() == pTileX && units[i]->getCurrentY() == pTileY)
			{
				unitFound = units[i]->getUnitID();
				break;
			}
		}
	}
	return (unitFound);
}//---

//=================================================================================

std::list<int> Squad::getActiveUnitsIDs()
{

	std::list<int> activeUnits = {};

	// Loop through Active units
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		if (units[i]->getIsActive())
		{
			activeUnits.push_back(units[i]->getUnitID());

		}
	}

	return activeUnits;
}//---

//=================================================================================

Vector2 Squad::getUnitPosition(int UnitID)
{
	Vector2 unitPosition;
	unitPosition.x = units[UnitID]->getCurrentX();
	unitPosition.y = units[UnitID]->getCurrentY();
	return	unitPosition;
}//---


void Squad::assignGroup1(std::list<int> pUnits)
{
	if (pUnits.size() > 0)
	{
		group1 = pUnits;

		for (int unit : pUnits)
		{
			std::cout << unit;
		}
	}
}//---

//=================================================================================