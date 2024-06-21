#include "Game.h"
#include "TextureManager.h"
#include "GameObjects.h"
#include "Levels.h"
#include "AI.h"
#include<string>


//=================================================================================
// SDL
SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::playerInputEvent;

// Gobal Text Variables
TTF_Font* font = nullptr;
SDL_Color textColour = { 255, 255, 255 };
SDL_Surface* textSurface = nullptr;
SDL_Texture* textTexture = nullptr;
SDL_Rect textRect;

// Audio
Mix_Music* music = NULL;
Mix_Chunk* shootSound = NULL;

//==========================================
//Game Objects
PlayerInput playerInput;
Level* map;
Level* bg_map;
AI* gameAI;

// Create Squads
Squad* squadEnemies;
Squad* squadHeroes;

// Game objects
Item* targetMarker;
Item* overlay;
Item* highLights[50];

Attack* fireBalls[10]; // Mage attacks
Attack* meleeAttacks[10];
Attack* arrowAttacks[10];
Item* hits[10];
Item* keys[3];


//=================================================================================
//Constructor
Game::Game()
{
	printf("Game Started \n");
}//---

//=================================================================================

void Game::createGameObjects()
{
	printf("************************* \nCreating GameObjects \n");
	// Create Game Level Map
	map = new Level();
	bg_map = new Level();
	bg_map->removeKeys();


	// Create Teams (Max units = 50)
	// Enemy Squad NPCs
	squadEnemies = new Squad("Enemies", 5); // name, tileType
	// Goblins
	for (int row = 0; row < BOARD_HEIGHT; row++)
	{
		for (int col = 0; col < BOARD_WIDTH; col++)
		{
			if (map->getTileContent(col, row) == 8) // Goblin
			{
				// Set tile as floor
				map->setTile(col, row, 0);
				bg_map->setTile(col, row, 0);
				squadEnemies->createUnits(map, 1, 0, 8, col, row);
			}

			if (map->getTileContent(col, row) == 9) // Skeleton
			{
				// Set tile as floor
				map->setTile(col, row, 0);
				bg_map->setTile(col, row, 0);
				squadEnemies->createUnits(map, 1, 0, 9, col, row);
			}
			
			
		}
	}
	
	squadEnemies->createUnits(map, 1, 0, 10, 90, 63);


	// Hero Squad - Player PCs

	// x 10 y 10,11,12
	squadHeroes = new Squad("Heroes", 6); 
	squadHeroes->createUnits(map, 1, 0, 5, 10, 10); // Warrior
	squadHeroes->createUnits(map, 1, 0, 6, 10, 11); // RAnger
	squadHeroes->createUnits(map, 1, 0, 7, 10, 12); // Mage


	// Create other Objects ------------------------------------------------

	// Marker2
	targetMarker = new Item("assets/images/Square_Purple.png", 16, 16, 8);


	// Selection highlights
	for (int i = 0; i < sizeof(highLights) / sizeof(highLights[0]); i++)
	{
		highLights[i] = new Item("assets/images/tiles/yellowbox.png", -1, -1, 32);
	}

	// Fire Balls
	for (int i = 0; i < sizeof(fireBalls) / sizeof(fireBalls[0]); i++)
	{
		fireBalls[i] = new Attack("assets/images/tiles/shot.png", 300, 300, 16);
		fireBalls[i]->setRange(20);
		fireBalls[i]->setSpeed(5);
	}
	// Melee Attacks
	for (int i = 0; i < sizeof(meleeAttacks) / sizeof(meleeAttacks[0]); i++)
	{
		meleeAttacks[i] = new Attack("assets/images/MeleeAttack.png", 300, 300, 16);
		meleeAttacks[i]->setRange(2);
		meleeAttacks[i]->setSpeed(10);
	}
	// Arrow Attacks
	for (int i = 0; i < sizeof(arrowAttacks) / sizeof(arrowAttacks[0]); i++)
	{
		arrowAttacks[i] = new Attack("assets/images/tiles/as_arrow.png", 300, 300, 16);
		arrowAttacks[i]->setRange(30);
		arrowAttacks[i]->setSpeed(15);
	}

	// Sprite for Interface Overlay
	overlay = new Item("assets/images/Overlay.png", 16, 16, 1280);
	overlay->srcRect.w = 1280; overlay->srcRect.h = 720;
	overlay->destRect.w = 1280; overlay->destRect.h = 720;


}//---

//=================================================================================

void Game::welcomeScreen()
{
	bool isReady = false;
	SDL_Surface* welcomeImage = IMG_Load("assets/images/welcomeImage.png");
	SDL_Surface* mainGameSurface = SDL_GetWindowSurface(gameWindow);

	SDL_BlitSurface(welcomeImage, NULL, mainGameSurface, NULL);


	while (!isReady)
	{
		handleEvents();
		if (playerInput.keyPressed == 13) // Enter
		{
			isReady = true;
		}
		SDL_UpdateWindowSurface(gameWindow);

	}
	SDL_UpdateWindowSurface(gameWindow);
	SDL_FreeSurface(welcomeImage);

}//---

void Game::winScreen()
{
	bool isReady = false;
	SDL_Surface* welcomeImage = IMG_Load("assets/images/winImage.png");
	SDL_Surface* mainGameSurface = SDL_GetWindowSurface(gameWindow);

	SDL_BlitSurface(welcomeImage, NULL, mainGameSurface, NULL);


	while (!isReady)
	{
		handleEvents();
		if (playerInput.keyPressed == 13) // Enter
		{
			isReady = true;
			isRunning = false;
		}
		SDL_UpdateWindowSurface(gameWindow);

	}
	SDL_UpdateWindowSurface(gameWindow);
	SDL_FreeSurface(welcomeImage);
}//---

void Game::gameOverScreen()
{
	bool isReady = false;
	SDL_Surface* welcomeImage = IMG_Load("assets/images/gameOverImage.png");
	SDL_Surface* mainGameSurface = SDL_GetWindowSurface(gameWindow);

	SDL_BlitSurface(welcomeImage, NULL, mainGameSurface, NULL);


	while (!isReady)
	{
		handleEvents();
		if (playerInput.keyPressed == 13) // Enter
		{
			isReady = true;
			isRunning = false;
		}
		SDL_UpdateWindowSurface(gameWindow);

	}
	SDL_UpdateWindowSurface(gameWindow);
	SDL_FreeSurface(welcomeImage);
}//



//=================================================================================
void Game::findNearestTargetHeroes() // 
{
	// Get a list of the active units
	std::list<int> enemyActiveUnits = squadEnemies->getActiveUnitsIDs();
	std::list<int> heroUnitsActive = squadHeroes->getActiveUnitsIDs();

	// Loop through all active Hero units to find nearest enemy unit
	
		for (int heroUnit : heroUnitsActive)
		{
			if (squadHeroes->getUnit(heroUnit)->getState() == 0)
			{
				// Reset distance variables
				int nearestHeroTarget = 0;
				int nearestEnemyTarget = 0;
				float distanceToNearestUnit = 1000; //  a large number off the board initially

				// get the current active Hero unit's position
				Vector2 heroUnitPos = squadHeroes->getUnitPosition(heroUnit - 601);
				Vector2 targetPos;

				// loop through active enemy units 
				for (int enemyUnit : enemyActiveUnits)
				{
					// get loop unit Position
					Vector2 enemyUnitPos = squadEnemies->getUnitPosition(enemyUnit - 501);


					// Calc distance using pythagoras
					float distance = sqrt((heroUnitPos.x - enemyUnitPos.x) * (heroUnitPos.x - enemyUnitPos.x) + (heroUnitPos.y - enemyUnitPos.y) * (heroUnitPos.y - enemyUnitPos.y));

					if (distance < distanceToNearestUnit && map->checkTargetInLineOfSight(heroUnitPos, enemyUnitPos)) // check if this unit is closer and can be seen
					{
						distanceToNearestUnit = distance;
						nearestHeroTarget = enemyUnit;
						targetPos = enemyUnitPos;

					}			
				}
				// Set Target of current Hero unit		
				squadHeroes->setUnitTarget(map, heroUnit - 601, nearestHeroTarget, targetPos);
				squadHeroes->setUnitTargetDistance(heroUnit - 601, distanceToNearestUnit);
		
			}
			
		}

}//---

void Game::findNearestTargetEnemies()
{
	// Get a list of the active units
	std::list<int> enemyActiveUnits = squadEnemies->getActiveUnitsIDs();
	std::list<int> heroUnitsActive = squadHeroes->getActiveUnitsIDs();
	// Loop through all active Enemy units to find nearest hero unit
	for (int enemyUnit : enemyActiveUnits)
	{
		// Reset distance variables
		int nearestTarget = 0;
		float distanceToNearestUnit = 1000; //  a large number off the board initially

		// get the current active Enemy unit's position
		Vector2 unitPos = squadEnemies->getUnitPosition(enemyUnit - 501);
		Vector2 targetPos;

		// loop through active hero units 
		for (int heroUnit : heroUnitsActive)
		{
			// get loop unit Position
			Vector2 heroUnitPos = squadHeroes->getUnitPosition(heroUnit - 601);

			// Calc distance using pythagoras
			float distance = sqrt((unitPos.x - heroUnitPos.x) * (unitPos.x - heroUnitPos.x) + (unitPos.y - heroUnitPos.y) * (unitPos.y - heroUnitPos.y));

			if (distance < distanceToNearestUnit && map->checkTargetInLineOfSight(unitPos, heroUnitPos)) // check if this unit is closer and can be seen
			{
				distanceToNearestUnit = distance;
				nearestTarget = heroUnit;
				targetPos = heroUnitPos;
					
			}
		}
		// Set Target of current Hero unit		
		squadEnemies->setUnitTarget(map, enemyUnit - 501, nearestTarget,targetPos);
		squadEnemies->setUnitTargetDistance(enemyUnit - 501, distanceToNearestUnit);


	}
}

//=================================================================================


void heroAttackTarget() // Do Hero units have an attack Target in Range
{
	// Loop through all active Hero units
	std::list<int> heroUnitsActive = squadHeroes->getActiveUnitsIDs();

	for (int heroUnit : heroUnitsActive)
	{
		if (squadHeroes->getUnitTarget(heroUnit - 601) > 0)
		{
			// get the target enemy ID 
			int enemyTargetID = squadHeroes->getUnitTarget(heroUnit - 601);

			if (enemyTargetID > 500 && enemyTargetID < 600) //-- Enemy
			{
				

				Vector2 enemyUnitPos = squadEnemies->getUnitPosition(enemyTargetID - 501);
				Vector2 unitPos = squadHeroes->getUnitPosition(heroUnit - 601);

				// Calc distance using pythagoras
				float distance = sqrt((unitPos.x - enemyUnitPos.x) * (unitPos.x - enemyUnitPos.x) + (unitPos.y - enemyUnitPos.y) * (unitPos.y - enemyUnitPos.y));

				if (distance < squadHeroes->getUnitAttackRange(heroUnit - 601))
				{
					float angle = std::atan2(unitPos.x - enemyUnitPos.x, unitPos.y - enemyUnitPos.y) * -180 / M_PI;

					switch (heroUnit)
					{
						case 601: // Warrior
							for (int i = 0; i < sizeof(meleeAttacks) / sizeof(meleeAttacks[0]); i++)
							{
								if (!meleeAttacks[i]->getIsActive())
								{
									int spawnX = unitPos.x * map->getMapTileSize() + map->getStartX(); // convert to screen
									int spawnY = unitPos.y * map->getMapTileSize() + map->getStartY();

									meleeAttacks[i]->spawn(spawnX, spawnY, angle, map, squadHeroes->getUnit(heroUnit)->getAttack(), 6);
									break;
								}
							}
							break;
						case 602: // Ranger
							for (int i = 0; i < sizeof(arrowAttacks) / sizeof(arrowAttacks[0]); i++)
							{
								if (!arrowAttacks[i]->getIsActive())
								{
									int spawnX = unitPos.x * map->getMapTileSize() + map->getStartX(); // convert to screen
									int spawnY = unitPos.y * map->getMapTileSize() + map->getStartY();

									arrowAttacks[i]->spawn(spawnX, spawnY, angle, map, squadHeroes->getUnit(heroUnit)->getAttack(), 6);
									break;
								}
							}
							break;
						case 603: // MAge
							for (int i = 0; i < sizeof(fireBalls) / sizeof(fireBalls[0]); i++)
							{
								if (!fireBalls[i]->getIsActive())
								{
									int spawnX = unitPos.x * map->getMapTileSize() + map->getStartX(); // convert to screen
									int spawnY = unitPos.y * map->getMapTileSize() + map->getStartY();

									fireBalls[i]->spawn(spawnX, spawnY, angle, map, squadHeroes->getUnit(heroUnit)->getAttack(), 6);
									break;
								}
							}
							break;
					}
					
				}
			}
		}
	}
}//-----

void enemyAttackTarget(Character* enemyUnit)
{

	// If enemy unit has a target and morale is high enough
	if (enemyUnit->getTargetID() > 0)
	{
		// get target ID
		int heroTargetID = enemyUnit->getTargetID();

		if (heroTargetID > 600) // -- Hero
		{
			Vector2 unitPos = {enemyUnit->getCurrentX(), enemyUnit->getCurrentY()};
			Vector2 heroUnitPos = squadHeroes->getUnitPosition(heroTargetID - 601);

			float distance = sqrt((unitPos.x - heroUnitPos.x) * (unitPos.x - heroUnitPos.x) + 
				(unitPos.y - heroUnitPos.y) * (unitPos.y - heroUnitPos.y));

			float angle = std::atan2(unitPos.x - heroUnitPos.x, unitPos.y - heroUnitPos.y) * -180 / M_PI;

			if (distance < enemyUnit->getAttackRange())
			{
				
				for (int i = 0; i < sizeof(meleeAttacks) / sizeof(meleeAttacks[0]); i++)
				{
					if (!meleeAttacks[i]->getIsActive())
					{
						int spawnX = unitPos.x * map->getMapTileSize() + map->getStartX(); // convert to screen
						int spawnY = unitPos.y * map->getMapTileSize() + map->getStartY();

						meleeAttacks[i]->spawn(spawnX, spawnY, angle, map, enemyUnit->getAttack(), 5);
						break;
					}
				}
			}
			
			// If demon sees target outside of his attack range then it shoots projectiles
			if (enemyUnit->getType() == 10 && enemyUnit->getAttackRange() < distance)
			{
				switch ((rand() % 2))
				{
					case 0:
					for (int i = 0; i < sizeof(fireBalls) / sizeof(fireBalls[0]); i++)
					{
						if (!fireBalls[i]->getIsActive())
						{
							int spawnX = unitPos.x * map->getMapTileSize() + map->getStartX(); // convert to screen
							int spawnY = unitPos.y * map->getMapTileSize() + map->getStartY();

							fireBalls[i]->spawn(spawnX, spawnY, angle, map, enemyUnit->getAttack(), 5);
							
						}
					}
					break;
					case 1:
					for (int i = 0; i < sizeof(arrowAttacks) / sizeof(arrowAttacks[0]); i++)
					{
						if (!arrowAttacks[i]->getIsActive())
						{
							int spawnX = unitPos.x * map->getMapTileSize() + map->getStartX(); // convert to screen
							int spawnY = unitPos.y * map->getMapTileSize() + map->getStartY();

							arrowAttacks[i]->spawn(spawnX, spawnY, angle, map, enemyUnit->getAttack(), 5);

						}
					}
					break;
				}
			}
		}
	}
	

}

void checkMorale(Character* pEnemyUnit)
{
	// Reset morale
	pEnemyUnit->setMorale(0);

	std::list<int> allActiveHeroUnits = squadHeroes->getActiveUnitsIDs();
	std::list<int> allActiveEnemyUnits = squadEnemies->getActiveUnitsIDs();
	Vector2 currentPos = { pEnemyUnit->getCurrentX(),pEnemyUnit->getCurrentY() };

	// Check if any enemy units can be seen
	for (int enemyUnit : allActiveEnemyUnits)
	{
		Vector2 enemyUnitPos = { squadEnemies->getUnit(enemyUnit)->getCurrentX(),squadEnemies->getUnit(enemyUnit)->getCurrentY() };

		if (map->checkTargetInLineOfSight(currentPos, enemyUnitPos))
		{
			// Depending on type of enemy that the unut can see add to the morale value
			switch (squadEnemies->getUnit(enemyUnit)->getType())
			{
			case 8: // Goblin
				pEnemyUnit->addMorale(2);
				break;
			case 9: // Skeleton
				pEnemyUnit->addMorale(3);
				break;
			case 10: // Demon
				pEnemyUnit->addMorale(10);
				break;
			}
		}
	}
	// Check if any hero units can be seen
	for (int heroUnit : allActiveHeroUnits)
	{
		Vector2 heroUnitPos = { squadHeroes->getUnit(heroUnit)->getCurrentX(),squadHeroes->getUnit(heroUnit)->getCurrentY() };

		if (map->checkTargetInLineOfSight(currentPos, heroUnitPos))
		{
			// Depending on type of Hero that the unut can see add to the morale value
			switch (squadHeroes->getUnit(heroUnit)->getType())
			{
			case 5: // Warrior
				pEnemyUnit->addMorale(-5);
				break;
			case 6: // Ranger
				pEnemyUnit->addMorale(-3);
				break;
			case 7: // Mage
				pEnemyUnit->addMorale(-2);
				break;
			}
		}

	}
}

//=================================================================================

Vector2 startSelectPos;
Vector2 endSelectPos;
std::list<int> unitsSelected = {};

void Game::selectUnits() 
{
	if (playerInput.mouseL) //mouse down
	{
		// reset end Pos and get start pos - box top left
		endSelectPos.x = startSelectPos.x = playerInput.mouseX;
		endSelectPos.y = startSelectPos.y = playerInput.mouseY;
	}

	if (playerInput.mouseUpL) // mouse released
	{
		endSelectPos.x = playerInput.mouseX;
		endSelectPos.y = playerInput.mouseY; // box end

		// find list of tiles in box from AI Class		
		std::list<Vector2> tilesFound = gameAI->findTilesinSelection(map, startSelectPos, endSelectPos);

		unitsSelected.clear(); // clear list of units

		for (Vector2 tile : tilesFound) // search for units in tiles
		{
			int heroUnitFound = -1;
			int enemyUnitFound = -1;
			heroUnitFound = squadHeroes->findUnit(tile.x, tile.y);
			enemyUnitFound = squadEnemies->findUnit(tile.x, tile.y);

			if (heroUnitFound > 0) unitsSelected.push_back(heroUnitFound); // add unit to list
			if (enemyUnitFound > 0) unitsSelected.push_back(enemyUnitFound);
		}
	}

	//  ************ ASIGN TARGET **************** Right click
	// Reset Target
	int targetTileX = -1, targetTileY = -1;

	// When R mouse is clicked  
	if (playerInput.mouseR)
	{
		// Move the target Marker
		targetMarker->x = playerInput.mouseX;
		targetMarker->y = playerInput.mouseY;

		// Get Tile Clicked on
		targetTileX = gameAI->findTileXFromMouseX(map, playerInput.mouseX);
		targetTileY = gameAI->findTileYFromMouseY(map, playerInput.mouseY);
		std::cout << "\n======\nMouse X:" << targetTileX << " Y:" << targetTileY << "\n=========" << std::endl;

		if (unitsSelected.size() > 0)
		{
			for (int unit : unitsSelected)
			{
				if (unit > 600 && unit < 700) // only for Hero units
				{
					int unitSelected = unit - 601; // subtract 601 to get real pos in array	
					squadHeroes->setTarget(map, unitSelected, targetTileX, targetTileY);
				}
			}
		}
	}
}//---


//=================================================================================

void Game::highlightSelection()
{
	// Reset all highlights
	for (int i = 0; i < sizeof(highLights) / sizeof(highLights[0]); i++)
	{
		highLights[i]->isActive = false;
	}

	if (unitsSelected.size() > 0) // only highlight if selction active
	{
		int tileSize = map->getMapTileSize();
		for (int unit : unitsSelected)
		{
			if (unit > 600 && unit < 700) // only for Hero units
			{
				int unitSelected = unit - 601;
				Vector2 unitPos = squadHeroes->getUnitPosition(unitSelected); // current tile pos				

				// find first unused highlight
				for (int i = 0; i < sizeof(highLights) / sizeof(highLights[0]); i++)
				{
					if (!highLights[i]->getIsActive())
					{
						highLights[i]->isActive = true;
						highLights[i]->destRect.h = highLights[i]->destRect.w = map->getMapTileSize();
						highLights[i]->x = unitPos.x * tileSize + map->getStartX();
						highLights[i]->y = unitPos.y * tileSize + map->getStartY();
						break;
					}
				}
			}
		}
	}
}//---

//=================================================================================

void Game::manageAttacks()
{

	// Fire Balls
	
	// Loop through Attacks Check Collison and Update 
	for (int i = 0; i < sizeof(fireBalls) / sizeof(fireBalls[0]); i++)
	{
		if (fireBalls[i]->getIsActive())
		{
			int tileCollidingX = gameAI->findTileXFromMouseX(map, fireBalls[i]->x);
			int tileCollidingY = gameAI->findTileYFromMouseY(map, fireBalls[i]->y);

			// check if an object is in the Tile the attack is currently over
			int objectColldingWith = map->getTileContent(tileCollidingX, tileCollidingY);

			if (objectColldingWith > 0) // attack hit something
			{
				if (objectColldingWith == 1) // Attack hit Wall
				{
					fireBalls[i]->isActive = false; // Destroy attack
				}

				if (objectColldingWith >= 8 && fireBalls[i]->getSquadType() == 6) // Enemy Unit Hit
				{
					// Destroy attack
					fireBalls[i]->isActive = false;

					// Get a list of the active Enemy units
					std::list<int> EnemyActiveUnits = squadEnemies->getActiveUnitsIDs();

					// loop through active Enemy units 
					for (int unit : EnemyActiveUnits)
					{
						// get loop unit Position
						Vector2 EnemyUnitPos = squadEnemies->getUnitPosition(unit - 501);

						// check if is colliding with attack
						if (EnemyUnitPos.x == tileCollidingX && EnemyUnitPos.y == tileCollidingY)
						{
							// find Enemy Tile and apply damage							
							squadEnemies->modifyHealth(unit - 501, -fireBalls[i]->getDamage());
						}
					}
				}

				if (objectColldingWith < 8 && objectColldingWith > 4 && fireBalls[i]->getSquadType() == 5) // Hero Unit Hit
				{
					// Destroy attack
					fireBalls[i]->isActive = false;

					// Get a list of the active Enemy units
					std::list<int> HeroActiveUnits = squadHeroes->getActiveUnitsIDs();

					// loop through active Enemy units 
					for (int unit : HeroActiveUnits)
					{
						// get loop unit Position
						Vector2 HeroUnitPos = squadHeroes->getUnitPosition(unit - 601);

						// check if is colliding with attack
						if (HeroUnitPos.x == tileCollidingX && HeroUnitPos.y == tileCollidingY)
						{
							// find Enemy Tile and apply damage							
							squadHeroes->modifyHealth(unit - 601, -fireBalls[i]->getDamage());
						}
					}
				}
			}
			// update Attack Movement and life
			fireBalls[i]->update(map);
		}
	}


	// Melee Attacks
		// Loop through Attacks Check Collison and Update 
	for (int i = 0; i < sizeof(meleeAttacks) / sizeof(meleeAttacks[0]); i++)
	{
		if (meleeAttacks[i]->getIsActive())
		{
			int tileCollidingX = gameAI->findTileXFromMouseX(map, meleeAttacks[i]->x);
			int tileCollidingY = gameAI->findTileYFromMouseY(map, meleeAttacks[i]->y);

			// check if an object is in the Tile the attack is currently over
			int objectColldingWith = map->getTileContent(tileCollidingX, tileCollidingY);

			if (objectColldingWith > 0) // attack hit something
			{
				if (objectColldingWith == 1) // Attack hit Wall
				{
					meleeAttacks[i]->isActive = false; // Destroy attack
				}

				if (objectColldingWith >= 8 && meleeAttacks[i]->getSquadType() == 6) // Enemy Unit Hit
				{
					// Destroy attack
					meleeAttacks[i]->isActive = false;

					// Get a list of the active Enemy units
					std::list<int> EnemyActiveUnits = squadEnemies->getActiveUnitsIDs();

					// loop through active Enemy units 
					for (int unit : EnemyActiveUnits)
					{
						// get loop unit Position
						Vector2 EnemyUnitPos = squadEnemies->getUnitPosition(unit - 501);

						// check if is colliding with attack
						if (EnemyUnitPos.x == tileCollidingX && EnemyUnitPos.y == tileCollidingY)
						{
							// find Enemy Tile and apply damage							
							squadEnemies->modifyHealth(unit - 501, -meleeAttacks[i]->getDamage());
						}
					}
				}

				if (objectColldingWith < 8 && objectColldingWith > 4 && meleeAttacks[i]->getSquadType() == 5) // Hero Unit Hit
				{
					// Destroy attack
					meleeAttacks[i]->isActive = false;

					// Get a list of the active Enemy units
					std::list<int> HeroActiveUnits = squadHeroes->getActiveUnitsIDs();

					// loop through active Enemy units 
					for (int unit : HeroActiveUnits)
					{
						// get loop unit Position
						Vector2 HeroUnitPos = squadHeroes->getUnitPosition(unit - 601);

						// check if is colliding with attack
						if (HeroUnitPos.x == tileCollidingX && HeroUnitPos.y == tileCollidingY)
						{
							// find Enemy Tile and apply damage							
							squadHeroes->modifyHealth(unit - 601, -meleeAttacks[i]->getDamage());
						}
					}
				}
			}
			// update Attack Movement and life
			meleeAttacks[i]->update(map);
		}
	}


	// Arrow Attack
		// Loop through Attacks Check Collison and Update 
	for (int i = 0; i < sizeof(arrowAttacks) / sizeof(arrowAttacks[0]); i++)
	{
		if (arrowAttacks[i]->getIsActive())
		{
			int tileCollidingX = gameAI->findTileXFromMouseX(map, arrowAttacks[i]->x);
			int tileCollidingY = gameAI->findTileYFromMouseY(map, arrowAttacks[i]->y);

			// check if an object is in the Tile the attack is currently over
			int objectColldingWith = map->getTileContent(tileCollidingX, tileCollidingY);

			if (objectColldingWith > 0) // attack hit something
			{
				if (objectColldingWith == 1) // Attack hit Wall
				{
					arrowAttacks[i]->isActive = false; // Destroy attack
				}

				if (objectColldingWith >= 8 && meleeAttacks[i]->getSquadType() == 6) // Enemy Unit Hit
				{
					// Destroy attack
					arrowAttacks[i]->isActive = false;

					// Get a list of the active Enemy units
					std::list<int> EnemyActiveUnits = squadEnemies->getActiveUnitsIDs();

					// loop through active Enemy units 
					for (int unit : EnemyActiveUnits)
					{
						// get loop unit Position
						Vector2 EnemyUnitPos = squadEnemies->getUnitPosition(unit - 501);

						// check if is colliding with attack
						if (EnemyUnitPos.x == tileCollidingX && EnemyUnitPos.y == tileCollidingY)
						{
							// find Enemy Tile and apply damage							
							squadEnemies->modifyHealth(unit - 501, -arrowAttacks[i]->getDamage());
						}
					}
				}

				if (objectColldingWith < 8 && objectColldingWith > 4 && arrowAttacks[i]->getSquadType() == 5) // Hero Unit Hit
				{
					// Destroy attack
					arrowAttacks[i]->isActive = false;

					// Get a list of the active Enemy units
					std::list<int> HeroActiveUnits = squadHeroes->getActiveUnitsIDs();

					// loop through active Enemy units 
					for (int unit : HeroActiveUnits)
					{
						// get loop unit Position
						Vector2 HeroUnitPos = squadHeroes->getUnitPosition(unit - 601);

						// check if is colliding with attack
						if (HeroUnitPos.x == tileCollidingX && HeroUnitPos.y == tileCollidingY)
						{
							// find Enemy Tile and apply damage							
							squadHeroes->modifyHealth(unit - 601, -arrowAttacks[i]->getDamage());
						}
					}
				}
			}
			// update Attack Movement and life
			arrowAttacks[i]->update(map);
		}
	}
}//---


//=================================================================================

void Game::moveGameObjects(float  pMoveAmountX, float pMoveAmountY)
{ // Move Objects as the map moves

	// Fire Balls
	for (int i = 0; i < sizeof(fireBalls) / sizeof(fireBalls[0]); i++)
	{
		if (fireBalls[i]->getIsActive())
		{
			fireBalls[i]->x += pMoveAmountX;
			fireBalls[i]->y += pMoveAmountY;
		}
	}
	// Melee Attacks
	for (int i = 0; i < sizeof(meleeAttacks) / sizeof(meleeAttacks[0]); i++)
	{
		if (meleeAttacks[i]->getIsActive())
		{
			meleeAttacks[i]->x += pMoveAmountX;
			meleeAttacks[i]->y += pMoveAmountY;
		}
	}

	// Arrow Attacks
	for (int i = 0; i < sizeof(arrowAttacks) / sizeof(arrowAttacks[0]); i++)
	{
		if (arrowAttacks[i]->getIsActive())
		{
			arrowAttacks[i]->x += pMoveAmountX;
			arrowAttacks[i]->y += pMoveAmountY;
		}
	}
	

	// Move Target marker
	targetMarker->x += pMoveAmountX;
	targetMarker->y += pMoveAmountY;





}//---

//=================================================================================

void Game::updateGameObjects()
{

	std::list<int> updatedActiveEnemyList = squadEnemies->getActiveUnitsIDs();
	std::list<int> updatedActiveHeroList = squadHeroes->getActiveUnitsIDs();
	
	// Hero Units Behaviours
	selectUnits();
	highlightSelection();

	manageAttacks();

	//--------------------------------------------------------------------------
	// add timer to only call every time period *******
	if (nextSlowUpdateTime < SDL_GetTicks64())
	{
		findNearestTargetHeroes();
		findNearestTargetEnemies();
		heroAttackTarget();
		for (int enemyUnit : updatedActiveEnemyList)
		{
			if (squadEnemies->getUnit(enemyUnit)->getMorale() > 0)
			{
				enemyAttackTarget(squadEnemies->getUnit(enemyUnit));
			}
		}
		nextSlowUpdateTime = SDL_GetTicks64() + 1000;
	}

	//--------------------------------------------------------------------------
	//  Update Sqauds 
	squadEnemies->update(map);
	squadHeroes->update(map);

	//---------------------------------------------------------------------------

	for (int enemyUnit : updatedActiveEnemyList)
	{
		checkMorale(squadEnemies->getUnit(enemyUnit));
	}

	// Modify Display
	adjustMapDisplay();


	// Check win/lose condition
	// ----------------------------------------------------------------------------

	// Win check
	for (int enemyUnit : updatedActiveEnemyList)
	{
		Character* unit = squadEnemies->getUnit(enemyUnit);
		if (unit->getType() == 10 && unit->getIsActive() == false)
		{
			winScreen();
		}
	}

	// Game Over check
	if (updatedActiveHeroList.size() < 1)
	{
		gameOverScreen();
	}


}//---


//=================================================================================

void Game::clearAttacks()
{
	// Fire Balls
	for (int i = 0; i < sizeof(fireBalls) / sizeof(fireBalls[0]); i++)
	{
		fireBalls[i]->isActive = false;
	}
	// Melee Attacks
	for (int i = 0; i < sizeof(meleeAttacks) / sizeof(meleeAttacks[0]); i++)
	{
		meleeAttacks[i]->isActive = false;
	}
	// Arrow Attacks
	for (int i = 0; i < sizeof(arrowAttacks) / sizeof(arrowAttacks[0]); i++)
	{
		arrowAttacks[i]->isActive = false;
	}
}//----


void Game::adjustMapDisplay()
{	
	Vector2 moveDirection = { 0,0 }; // Mapr move vector

	if (playerInput.keyPressed == SDLK_EQUALS || playerInput.mouseWheelUp == true)
	{
		map->setMapTileSize(+1);
		bg_map->setMapTileSize(+1);
		moveDirection.x--;
		moveDirection.y--;
		clearAttacks(); // remove all attack sprites
	}

	if (playerInput.keyPressed == SDLK_MINUS || playerInput.mouseWheelDown == true)
	{
		map->setMapTileSize(-1);
		bg_map->setMapTileSize(-1);
		moveDirection.x++;
		moveDirection.y++;
		clearAttacks();
	}

	// Check Keys for moving map
	if (playerInput.keyPressed == SDLK_LEFT || playerInput.keyPressed == SDLK_a) moveDirection.x++;
	if (playerInput.keyPressed == SDLK_RIGHT || playerInput.keyPressed == SDLK_d) moveDirection.x--;
	if (playerInput.keyPressed == SDLK_UP || playerInput.keyPressed == SDLK_w) moveDirection.y++;
	if (playerInput.keyPressed == SDLK_DOWN || playerInput.keyPressed == SDLK_s) moveDirection.y--;

	// Check mouse Pos 
	if (SDL_GetTicks64() > nextSlowMouseUpdateTime)
	{
		if (playerInput.mouseX > 1260 && playerInput.mouseX < 1179) moveDirection.x--;
		if (playerInput.mouseX > 0 && playerInput.mouseX < 20) moveDirection.x++;
		if (playerInput.mouseY > 700 && playerInput.mouseY < 679) moveDirection.y--;
		if (playerInput.mouseY > 0 && playerInput.mouseY < 20) moveDirection.y++;
		nextSlowMouseUpdateTime = SDL_GetTicks64() + 100; // limit speed of mouse moving map
	}

	// Move the Map
	if (moveDirection.x != 0 || moveDirection.y != 0)
	{
		map->moveMapX(moveDirection.x);
		map->moveMapY(moveDirection.y);
		bg_map->moveMapX(moveDirection.x);
		bg_map->moveMapY(moveDirection.y);
		// Move game objects as we move the map
		moveGameObjects(moveDirection.x * map->getMapTileSize(), moveDirection.y * map->getMapTileSize());
	}
}//---

//=================================================================================

void Game::render()
{
	SDL_RenderClear(renderer);

	// Map Tiles
	bg_map->drawMap();
	map->drawMap();
	

	// Highlights
	for (int i = 0; i < sizeof(highLights) / sizeof(highLights[0]); i++)
	{
		if (highLights[i]->getIsActive()) highLights[i]->render();
	}

	// Fire Balls
	for (int i = 0; i < sizeof(fireBalls) / sizeof(fireBalls[0]); i++)
	{
		if (fireBalls[i]->getIsActive())
		{
			fireBalls[i]->renderEX();
		}
	}
	// Melee Attacks
	for (int i = 0; i < sizeof(meleeAttacks) / sizeof(meleeAttacks[0]); i++)
	{
		if (meleeAttacks[i]->getIsActive())
		{
			meleeAttacks[i]->renderEX();
		}
	}
	// Arrow Attacks
	for (int i = 0; i < sizeof(arrowAttacks) / sizeof(arrowAttacks[0]); i++)
	{
		if (arrowAttacks[i]->getIsActive())
		{
			arrowAttacks[i]->renderEX();
		}
	}

	// HP Bars
	squadHeroes->drawUnitHP(map);
	squadEnemies->drawUnitHP(map);



	// objects
	targetMarker->render();
	overlay->render();

	// Screen Text
	updateGUI();

	SDL_SetRenderDrawColor(Game::renderer, 10, 10, 10, 255);
	SDL_RenderPresent(renderer);
}//---

 //=================================================================================

void Game::updateGUI()
{
	std::string  screenText;
	std::string keyText;
	int textW = 0, textH = 0;

	// Squad Data to Text
	screenText = "";
	keyText = "";

	// Display Unit Stats
	int unitID = 0;
	if (unitsSelected.size() > 0) unitID = unitsSelected.front();

	//screenText += "Unit:" + std::to_string(unitID);
	if (unitID > 600 && unitID < 700) // Hero squad
	{
		unitID -= 601;
		screenText +=  squadHeroes->getUnitName(unitID);
		screenText += "\n--------\nHP:\n" + std::to_string((int)squadHeroes->getUnitHealth(unitID)) + "/" + std::to_string((int)squadHeroes->getUnitMaxHealth(unitID));
		screenText += "\n--------\nDamage:" + std::to_string((int)squadHeroes->getUnitAttack(unitID));
		screenText += "\nRange:" + std::to_string((int)squadHeroes->getUnitAttackRange(unitID));
		screenText += "\n--------\nState:\n" + squadHeroes->getUnitStateString(unitID);
	}

	if (unitID < 600 && unitID > 500) // Enemy squad
	{
		unitID -= 501;
		screenText += squadEnemies->getUnitName(unitID);
		screenText += "\n--------\nHP:\n" + std::to_string((int)squadEnemies->getUnitHealth(unitID)) + "/" + std::to_string((int)squadEnemies->getUnitMaxHealth(unitID));
		screenText += "\n--------\nDamage:" + std::to_string((int)squadEnemies->getUnitAttack(unitID));
		screenText += "\nRange:" + std::to_string((int)squadEnemies->getUnitAttackRange(unitID));
		screenText += "\n--------\nState:\n" + squadEnemies->getUnitStateString(unitID);
	}


	keyText = std::to_string(squadHeroes->getKeys()) + "/3";


	// Create Text Texture	
	textSurface = TTF_RenderText_Blended_Wrapped(font, screenText.c_str(), textColour, 0);

	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 16, 422, textW, textH };

	// Copy Text Texture to Renderer
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	// Clear the Memory
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

	// Create Text Texture	
	textSurface = TTF_RenderText_Blended_Wrapped(font, keyText.c_str(), textColour, 0);

	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
	textRect = { 1120, 32, textW, textH };

	// Copy Text Texture to Renderer
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	// Clear the Memory
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);

}//---

//=================================================================================


void Game::handleEvents() // Updates Player Input
{
	// Reset Inputs
	playerInput.keyPressed = NULL;
	playerInput.mouseL = false;
	playerInput.mouseR = false;
	playerInput.mouseUpL = false;
	playerInput.mouseWheelUp = false;
	playerInput.mouseWheelDown = false;

	SDL_PollEvent(&playerInputEvent);

	switch (playerInputEvent.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;

	case SDL_MOUSEMOTION:
		int mouseXpos, mouseYpos;
		SDL_GetMouseState(&mouseXpos, &mouseYpos);
		playerInput.mouseX = mouseXpos;
		playerInput.mouseY = mouseYpos;
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (playerInputEvent.button.button == SDL_BUTTON_LEFT) playerInput.mouseL = true;
		if (playerInputEvent.button.button == SDL_BUTTON_RIGHT) playerInput.mouseR = true;
		break;

	case SDL_MOUSEBUTTONUP:

		if (playerInputEvent.button.button == SDL_BUTTON_LEFT)
			playerInput.mouseUpL = true;
		break;

	case SDL_MOUSEWHEEL:
		if (playerInputEvent.wheel.y > 0) playerInput.mouseWheelUp = true;
		if (playerInputEvent.wheel.y < 0) playerInput.mouseWheelDown = true;
		break;

	case SDL_KEYDOWN:

		playerInput.keyPressed = playerInputEvent.key.keysym.sym;
		break;

	default:
		break;
	}
}//---


//=================================================================================

void Game::init(const char* pTitle, int pXpos, int pYpos, int pWidth, int pHeight, bool pFullscreen)
{
	int flags = 0;
	if (pFullscreen) flags = SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "SDL Initialised  \n";
		// Create Game Window
		gameWindow = SDL_CreateWindow(pTitle, pXpos, pYpos, pWidth, pHeight, flags);
		if (gameWindow)	std::cout << "Game Window Created \n";
		// Create Renderer
		renderer = SDL_CreateRenderer(gameWindow, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 128, 128, 128, 0);
			std::cout << "Renderer Created \n";
		}
		// Initialise Fonts
		TTF_Init();
		font = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", 24);
		if (font) std::cout << "TTF initialise \n";

		// Init AUDIO
		Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
		std::printf("Audio Mixer Initialised \n");

		// Set initial states
		isRunning = true;
	}
	else
	{
		isRunning = false;
	}
}//---

//=================================================================================

void Game::clean()
{
	std::cout << "************************* \nClearing Memory \n";
	SDL_Delay(1000);

	//AUDIO
	Mix_FreeMusic(music);
	Mix_FreeChunk(shootSound);
	Mix_CloseAudio();

	// Fonts
	TTF_CloseFont(font);
	TTF_Quit();

	SDL_DestroyWindow(gameWindow);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "SDL Closed \n";	

}//---

//=================================================================================
