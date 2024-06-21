#pragma once
#include"game.h"
#include "GameObjects.h"
#include "Levels.h"

//=================================================================================


class AI
{
public:
	AI();

	int findTileXFromMouseX(Level* pMap, int pMouseX);
	int findTileYFromMouseY(Level* pMap, int pMouseY);

	std::list<Vector2>findTilesinSelection(Level* pMap, Vector2 startPos, Vector2 endPos);
	std::list<Vector2> findAdjacentRandomTile(Level* pMap, int x, int y);

	// PathFinding
	bool checkTargetInLineOfSight(Level* pMap, Vector2 pCurrentPos, Vector2 pTargtetPos);
	std::list<Vector2>findWaypointTarget(Level* pMap, Vector2 startPos, Vector2 endPos);


private:

};

//=================================================================================
