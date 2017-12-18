#include "stdafx.h"
#include "ai.h"
#include "Superhex.h"
#include <math.h>

int ceilings[6];
int floors[6];

int evaluateCandPos(int candPos, int curPos, int dir, int sides, int hedge)
{
	int startCeiling = ceilings[curPos];
	int penalty = 0;
	for (; curPos != candPos; curPos = (curPos + dir + sides) % sides)
	{
		if (floors[(curPos + dir + sides) % sides] > ceilings[curPos]) // not possible to move there coz of obstruction
		{
			//printf("obstruction\t%d > %d @ %d via %d %d\n", floors[(curPos + dir + sides) % sides], ceilings[curPos], curPos, dir);
			penalty += 6 * (floors[(curPos + dir + sides) % sides] - ceilings[curPos]);
		}
		if (floors[curPos] + hedge > ceilings[(curPos + dir + sides) % sides]) // not possible to move coz its too tight 
		{
			//printf("too tight\t%d > %d @ %d via %d\n", floors[curPos], ceilings[(curPos + dir + sides) % sides], curPos, dir);
			penalty += 4 * (floors[curPos] + hedge - ceilings[(curPos + dir + sides) % sides]);
		}
		penalty += 30;
	}
	return ceilings[candPos] - startCeiling - penalty;
}

void AiOnTick(CSuperhex* pSuperhex)
{
	// return if not ingame
	if ((pSuperhex->gamestate.stage == 1 && pSuperhex->gamestate.gameover == 2) || pSuperhex->gamestate.stage == -1)
		return;

	pSuperhex->gamestate.baseRotation = 0;
	pSuperhex->gamestate.oscillator = 0;
	pSuperhex->gamestate.oscillator2 = 0;
	pSuperhex->gamestate.oscillatorState = 0;

	// grab some game metadata
	int sides = pSuperhex->gamestate.axisCount;
	float playerSection = (pSuperhex->gamestate.playerRotation) * sides / 360.f;
	int maxDist = 145 - pSuperhex->gamestate.wallSpeed; // distance below which walls no longer hurt us
	int hedge = maxDist * (360 / sides / 10) + 50; // min space required to fit thru section

	// compute sector information
	for (int i = 0; i < sides; i++)
		floors[i] = maxDist;
	for (int i = 0; i < sides; i++)
		ceilings[i] = 999999;
	for (int i = 0; i < pSuperhex->gamestate.wallCnt; i++)
	{
		wall_t wall = pSuperhex->gamestate.walls[i];
		if (!wall.enabled)
			continue;
		int section = wall.section;

		int startDist = wall.distance;
		int endDist = wall.distance + wall.width;
		if (startDist <= floors[section] && endDist > floors[section]) // obstruction
		{
			floors[section] = endDist;
		}
		if (startDist < ceilings[section] && wall.distance > 0 && startDist > maxDist)
		{
			ceilings[section] = startDist;
		}
	}

	printf("<%d> <%d> ", maxDist, hedge);
	for (int i = 0; i < sides; i++)
		printf("%d.%d ", floors[i], ceilings[i]);
	printf("\n");

	// choose best sector
	int bestPos = (int) playerSection;
	int bestScore = 0;
	int bestDir = 0;
	for (int i = 0; i < sides; i++)
	{
		for (int dir = -1; dir < 2; dir++)
		{
			if (dir != 0)
			{
				int score = evaluateCandPos(i, playerSection, dir, sides, hedge);
				if (score > bestScore)
				{
					bestScore = score;
					bestDir = dir;
					bestPos = i;
				}
				//printf("%d\t%d\t%d\t%f%\n", (int)playerSection, i, dir, score);
			}
		}
	}
	//printf("*** %d\t%d\t%d\t%f%\n", (int)playerSection, bestPos, bestDir, bestScore);

	// align to the center of the sector
	if (bestDir == 0)
	{
		float thang = fmodf(playerSection, 1.f) - .5f;
		//printf("%f\n", thang);
		if (thang > .1f)
		{
			bestDir = -1;
		}
		else if (thang < -.1f)
		{
			bestDir = 1;
		}
	}

	// move
	if (bestDir == 1)
	{
		pSuperhex->buttonStates[LeftArrow] = 1;
		pSuperhex->buttonStates[RightArrow] = 0;
	}
	else if (bestDir == -1)
	{
		pSuperhex->buttonStates[RightArrow] = 1;
		pSuperhex->buttonStates[LeftArrow] = 0;
	}
	else
	{
		pSuperhex->buttonStates[LeftArrow] = 0;
		pSuperhex->buttonStates[RightArrow] = 0;
	}
}
