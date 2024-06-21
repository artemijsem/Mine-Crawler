#include"GameObjects.h"
#include"TextureManager.h"



//=================================================================================

void GameObject::render()
{
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopy(Game::renderer, spriteTexture, &srcRect, &destRect);
}//----

void Attack::renderEX()
{
	destRect.x = x;
	destRect.y = y;


	SDL_RenderCopyEx(Game::renderer, spriteTexture, &srcRect, &destRect, angle - 90, NULL, SDL_FLIP_NONE);
}

void Character::drawHPBar(Level* pMap)
{
	if (isActive)
	{
		float currentUnitHealth = getHealth();
		float maxUnitHealth = getMaxHealth();
		int currentMapSize = pMap->getMapTileSize();
		int xOffSet = pMap->getStartX();
		int yOffset = pMap->getStartY() - currentMapSize / 4;

		// Placement and size of HP Bar above Unit
		destRect.h = 3;
		destRect.w = currentMapSize * currentUnitHealth / maxUnitHealth;
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


void Attack::spawn(int pX, int pY, float pAngle, Level* pMap, float pDamage, int pSquadType)
{
	isActive = true;
	x = pX;
	y = pY;
	angle = pAngle;
	damage = pDamage;
	squadType = pSquadType;

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


void Attack::setRange(float pRange)
{
	range = pRange;
}

void Attack::setSpeed(float pSpeed)
{
	speed = pSpeed;
}

void Attack::setDamage(float pDamage)
{
	damage = pDamage;
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
		followPath(pMap);
		break;
	case 4: // Avoid  / RandomMove
		avoid(pMap);
		break;
	case 5: // Flee
		moveToTarget(pMap);
		break;
	case 6: // Guard
		setTarget(pMap, homeX, homeY);
		followPath(pMap);
		break;
	case 7: // Chase
		chaseTarget(pMap);
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

void Character::setAttackRange(float pAttackRange)
{
	attackRange = pAttackRange;
}

void Character::setAttack(float pAttack)
{
	attack = pAttack;
}

void Character::setMorale(float pMorale)
{
	morale = pMorale;
}

void Character::addMorale(float pMorale)
{
	morale += pMorale;
}



//=================================================================================


int  Character::getKeys()
{
	return keys;
}

void Character::addKey(int pAmount)
{
	keys += pAmount;
}



//=================================================================================

void Character::avoid(Level* pMap)
{

	// Create Random Direction
	if (nextMoveTime < SDL_GetTicks())
	{
		// Add a Random number between -1 to 1 to currentX,Y  
		nextX = currentX + (rand() % 3 - 1);
		nextY = currentY + (rand() % 3 - 1);
		// Check if next Position is Empty
		if (pMap->getTileContent(nextX, nextY) == 0 )
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
		if (lastState != NULL)
		{
			state = lastState;
		}
			
	}
	
}//---



//=================================================================================

void Character::setTarget(Level* pMap, int pTargetX, int pTargetY)
{
	// ensure Target is empty or valid
	if (pMap->getTileContent(pTargetX, pTargetY) == 0 || pMap->getTileContent(pTargetX, pTargetY) == 3 || pMap->getTileContent(pTargetX, pTargetY) >= 7)
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
			// Find path only for static objects
			if (pMap->getTileContent(pTargetX, pTargetY) == 0 || pMap->getTileContent(pTargetX, pTargetY) == 3)
			{
				pathToTarget = pMap->findPath(curPos, targPos);
				wpsToTarget = pMap->getListOfWaypointsToTarget(curPos, targPos);
			}
			
		}
	}
}//-----



void Character::followPath(Level* pMap)
{

	if (nextMoveTime < SDL_GetTicks())
	{
		nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;// Update Move coolDown

		Vector2 targPos = { mainTarget.x, mainTarget.y };
		Vector2 curPos = { currentX, currentY };
		float angle = 0;

		if (!pMap->checkTargetInLineOfSight(curPos, targPos)) // Target not in sight - Use Path
		{
			if (pathToTarget.size() > 0)
			{
				nextX = pathToTarget.back().x;
				nextY = pathToTarget.back().y;

				// Check if next Position is Empty
				if (pMap->getTileContent(nextX, nextY) == 0 || pMap->getTileContent(nextX, nextY) == 3)
				{
					// Key found by Hero squad memeber
					if (pMap->getTileContent(nextX, nextY) == 3 && tileType < 8) 
					{
						std::cout << "\n key found";
						keys++;
					}

					// Clear Old position and set new
					pMap->setTile(currentX, currentY, 0);
					pMap->setTile(nextX, nextY, tileType);

					// Update Current Position
					currentX = nextX;
					currentY = nextY;

					pathToTarget.pop_back(); // remove last path tile
				}
			}
		}
		else // Target in sight
		{
			// clear paths
			pathToTarget.clear();

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
				angle = atan2(dY, dX); // this is in radians		
			}

			// Calcuate next X & Y				
			nextX = currentX + round(1.4 * cos(angle));
			nextY = currentY + round(1.4 * sin(angle));

			// Check if next Position is Empty
			if (pMap->getTileContent(nextX, nextY) == 0 || pMap->getTileContent(nextX, nextY) == 3)
			{
				// Key found by Hero squad memeber
				if (pMap->getTileContent(nextX, nextY) == 3 && tileType < 8) 
				{
					std::cout << "\n key found";
					keys++;
				}

				// Clear Old position and set new
				pMap->setTile(currentX, currentY, 0);
				pMap->setTile(nextX, nextY, tileType);

				// Update Current Position
				currentX = nextX;
				currentY = nextY;
			}
			//else // try to avoid occupied tile 
			//{
			//	if (state != 0) // unit is not idle
			//	{
			//		lastState = state;
			//		state = 4;
			//	}
			//}
		}
	}
}//-----


void Character::moveToTarget(Level* pMap)
{
	if (nextMoveTime < SDL_GetTicks())
	{
		nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;// Update Move coolDown

		Vector2 targPos = { mainTarget.x, mainTarget.y };
		Vector2 curPos = { currentX, currentY };
		Vector2 wpPos = {};
		float angle = 0;

		if (!pMap->checkTargetInLineOfSight(curPos, targPos)) // Target not in sight - Find Waypoints
		{
			if (wpsToTarget.size() > 0) // List of way points exists
			{
				wpPos = pMap->getWPPos(wpsToTarget.front()); // Get first WP Pos

				if (abs(wpPos.x - currentX) < 2 && abs(wpPos.y - currentY) < 2) // check if we have reached current WP
				{
					wpsToTarget.pop_front(); // remove WP form List
				}
				else // Move to WP
				{
					// Calculate angle to target
					int dX = wpPos.x - currentX;
					int dY = wpPos.y - currentY;
					angle = atan2(dY, dX); // this is in radians	
				}
			}
		}
		else // Target in sight
		{
			wpsToTarget.clear(); // clear List of Wps

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
				angle = atan2(dY, dX); // this is in radians		
			}
		}

		// Calcuate next X & Y				
		nextX = currentX + round(1.4 * cos(angle));
		nextY = currentY + round(1.4 * sin(angle));

		// Check if next Position is Empty
		if (pMap->getTileContent(nextX, nextY) == 0 || pMap->getTileContent(nextX, nextY) == 3)
		{
			// Key found by Hero squad memeber
			if (pMap->getTileContent(nextX, nextY) == 3 && tileType < 8) 
			{
				std::cout << "\n key found";
				keys++;
			}

			// Clear Old position and set new
			pMap->setTile(currentX, currentY, 0);
			pMap->setTile(nextX, nextY, tileType);
			// Update Current Position
			currentX = nextX;
			currentY = nextY;
		}
		else // try to avoid occupied tile 
		{
			if (state != 0) // unit is not idle
			{
				lastState = state;
				state = 4;
			}
		}
	}
}//-----

//=================================================================================

void Character::chaseTarget(Level* pMap)
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
		if (pMap->getTileContent(nextX, nextY) == 0)
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
			if (state != 0)
			{
				lastState = state;
				state = 4;
			}
		}
		// Update Move coolDown
		nextMoveTime = SDL_GetTicks() + 1000 * 1 / speed;

	}
}//---


//=================================================================================

void Character::roam(Level* pMap)
{// Move to Random Adjacent Tile

	// Create Random Direction
	if (nextMoveTime < SDL_GetTicks())
	{
		// Add a Random number between -1 to 1 to currentX,Y  
		nextX = currentX + (rand() % 3 - 1);
		nextY = currentY + (rand() % 3 - 1);

		// Check if next Position is Empty
		if (pMap->getTileContent(nextX, nextY) == 0)
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
	
}//---
//=================================================================================

void Character::wander(Level* pMap)
{
	// Move to Next Tile based on  heading

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

		// Check if next Position is Empty or Key
		if (pMap->getTileContent(nextX, nextY) == 0 || pMap->getTileContent(nextX, nextY) == 3)
		{
			// Key found by Hero squad memeber
			if (pMap->getTileContent(nextX, nextY) == 3 && tileType < 8)
			{
				std::cout << "\n Key found";
				keys++;
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
	
}//---

void Character::manageHealth()
{
	if (health > hpMax) health = hpMax;
}

//=================================================================================
// Squad Methods

Squad::Squad(const char* pName, int pType)// Constructor
{
	// Set display Name
	name = pName;
	tileType = pType;

	// Create unit Object but do not activate them
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		units[i] = new Character(-10, -10); // Set Position off the board
	}

	std::cout << "\n Squad Created\n";
	std::cout << name << " Tile type =" << tileType << std::endl;
}//----


void Squad::createUnits(Level* pMap, int pSpawnAmount, int pInitialState, int pTileType, int pSpawnX, int pSpawnY)
{

	if (pSpawnAmount < maxUnits)
	{
		// Create the initial starting units staring at Home X Y
		for (int i = IDoffset; i < pSpawnAmount + IDoffset; i++)
		{
			// Create Instance & starting positions and spacing
			int xPos = pSpawnX + i - IDoffset;
			int yPos = pSpawnY;

			if (xPos > BOARD_WIDTH - 2 || yPos > BOARD_HEIGHT - 2)
			{
				std::cout << " unit off board";
			}
			else // position is on Board
			{
				if (units[i]->getIsActive() == false)
				{
					// Set initial values
					units[i]->setUnitID(tileType * 100 + i + 1);
					units[i]->setType(pTileType); // map tile type 
					units[i]->setSpeed(5);
					units[i]->setState(pInitialState);
					units[i]->setHeading(rand() % 8); // random initial Direction

					// Assign stats to each unit
					switch (units[i]->getType())
					{
					case 5: // Warrior
						units[i]->setName("Warrior");
						units[i]->setAttackRange(5);
						units[i]->setAttack(30);
						break;
					case 6: // Ranger
						units[i]->setName("Ranger");
						units[i]->setAttackRange(15);
						units[i]->setAttack(15);
						break;
					case 7: // Mage
						units[i]->setName("Mage");
						units[i]->setAttackRange(10);
						units[i]->setAttack(20);
						break;
					case 8: // Goblin
						units[i]->setName("Goblin");
						units[i]->setAttackRange(3);
						units[i]->setAttack(5);
						break;
					case 9: // Skeleton
						units[i]->setName("Skeleton");
						units[i]->setAttackRange(4);
						units[i]->setAttack(10);
						units[i]->setHomeX(xPos);
						units[i]->setHomeY(yPos);
						break;
					case 10: // Demon
						units[i]->setName("Demon");
						units[i]->setAttack(30);
						units[i]->setAttackRange(5);
						units[i]->setMaxHealth(300);
						units[i]->setHealth(200);
						break;
					}

					if (pMap->getTileContent(xPos, yPos) == 0)
					{
						pMap->setTile(xPos, yPos, pTileType);
						units[i]->isActive = true;
						units[i]->setPosition(xPos, yPos);
						std::cout << units[i]->getUnitID() << " unit spawned  ";

						// Update Squad Stats
						activeUnits++;
					}
					else
					{
						std::cout << "Tile occupied, unit: " << i << " not created" << std::endl;
						units[i]->isActive = false;
					}
				}
			}
		}
		IDoffset += pSpawnAmount; // Make sure that created enemies will take empty indexes in array
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

			units[i]->manageHealth();

			if (units[i]->getIsActive()) // only check active units
			{
				int keysFound = units[i]->getKeys();
				if (keysFound > 0)
				{
					keys += keysFound; // add keys to the squad
					units[i]->addKey(-keysFound);
				}
			}
			// Check for idle Warrior unit
			if (units[i]->getState() == 0 && units[i]->getType() < 8)
			{
				units[i]->setHealth(10);
			}
		}

		// If 3 keys gathered, remove door
		if (keys == 3)
		{
			pMap->removeDoor();
		}

		// ======================================================================================
		// Enemy squad managment
		if (units[i]->getType() == 8) // goblins
		{
			if (units[i]->getTargetID() > 0 && units[i]->getMorale() > 0)
			{
				// If morale is high - get closer, if low - flee from target
				units[i]->setState(7); // Chase
			}
			else if(units[i]->getMorale() < 0) 
			{
				// If morale is low - get waypoint next to the closest waypoint currently
				Vector2 currentPos = { units[i]->getCurrentX(), units[i]->getCurrentY() };
				Vector2 fleeingWP = pMap->getNextRoomWPPos(currentPos);
				units[i]->setTarget(pMap, fleeingWP.x, fleeingWP.y);
				units[i]->setState(5); // Flee
			}
			else
			{
				units[i]->setState(0); // Idle
			}
		}

		if (units[i]->getType() == 9) // Skeletons
		{
			float dX = units[i]->getCurrentX() - units[i]->getHomeX();
			float dY = units[i]->getCurrentY() - units[i]->getHomeY();
			float distanceFromHome = sqrt(dX * dX + dY * dY);

			// If there is a target chase it, if moved away from the home too far - return
			if (units[i]->getTargetID() > 0 && distanceFromHome < 30)
			{
				units[i]->setState(7); // Chase
			}
			else if (distanceFromHome > 30)
			{ units[i]->setState(6);}  // Guard

			else{units[i]->setState(0);} // Idle
		}
		if (units[i]->getType() == 10) // Demon
		{
			// If there is a target - move randomly, otherwise idle
			if (units[i]->getTargetID() > 0)
			{ units[i]->setState(4);} // Random move

			else 
			{ units[i]->setState(0);} // Idle
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

void Squad::setTarget(Level* pMap, int pUnitID, int pTileX, int pTileY)
{
	// Move to target if empty, enemy or key
	if (pMap->getTileContent(pTileX, pTileY) == 0 || pMap->getTileContent(pTileX, pTileY) == 3 || pMap->getTileContent(pTileX, pTileY) == 5)
	{
		if (units[pUnitID]->getIsActive())
		{
			units[pUnitID]->setState(3); // set to Move to Target
			units[pUnitID]->setTarget(pMap, pTileX, pTileY);
			
		}
	}
}

//=================================================================================
// Squad Getters and Setters 


int Squad::getKeys()
{
	return keys;
}

void Squad::addKey(int pResAmount)
{
	keys += pResAmount;
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


//=================================================================================

Character* Squad::getUnit(int pUnitID)
{
	for (int i = 0; i < sizeof(units) / sizeof(units[0]); i++)
	{
		if (units[i]->getUnitID() == pUnitID)
		{
			return units[i];
		}
	}
}


std::string Squad::getUnitStateString(int pUnitID)
{
	std::string state;
	switch (units[pUnitID]->getState())
	{
	case 0: // Idle
		state = "Idling";
		break;
	case 1: // Roam
		state = "Roaming";
		break;
	case 2: // Wander
		state = "Wandering";
		break;
	case 3: // Chase Target	
		state = "Moving";
		break;
	case 4: // Avoid  / RandomMove
		state = "Avoiding";
		break;
	case 5: // Flee
		state = "Fleeing";
		break;
	case 6: // Guard
		state = "Guarding";
		break;
	case 7: // Chase
		state = "Chasing";
		break;
	}

	return state;
}