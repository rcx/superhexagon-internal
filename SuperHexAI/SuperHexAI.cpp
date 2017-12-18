#include "stdafx.h"
#include "memory.h"
#include "process.h"
#include "Superhex.h"

#define fail(...) {fprintf(stderr, __VA_ARGS__); system("pause"); return 1;}

int section(int s, int i)
{
	return (int)(360 / s * (i + 0.5));
}

int hedge2 = 0;

int ceilings[6];
int floors[6];
int prevBestPos;

float evaluateCandPos(int candPos, int curPos, int dir, int sides, int hedge) {
	int startCeiling = ceilings[curPos];
	int penalty = 0;
	for (; curPos != candPos; curPos = (curPos + dir + sides) % sides) {
		if (floors[(curPos + dir + sides) % sides] + hedge2 > ceilings[curPos]) {// not possible to move there coz of obstruction
			//printf("obstruction\t%d > %d @ %d via %d %d\n", floors[(curPos + dir + sides) % sides], ceilings[curPos], curPos, dir);
			penalty += 6 * (floors[(curPos + dir + sides) % sides] - ceilings[curPos]);
		}
		if (floors[curPos] + hedge > ceilings[(curPos + dir + sides) % sides]) {// not possible to move coz its too tight 
			//printf("too tight\t%d > %d @ %d via %d\n", floors[curPos], ceilings[(curPos + dir + sides) % sides], curPos, dir);
			penalty += 4 * (floors[curPos] + hedge - ceilings[(curPos + dir + sides) % sides]);
		}
		penalty += 30;
	}
	return ceilings[candPos] - startCeiling - penalty;
}

int main()
{
	DWORD dwPid = getPID(L"superhexagon.exe");
	if (!dwPid)
		fail("bad pid\n");
	printf("found superhexagon; pid=%d\n", dwPid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (!hProcess)
		fail("couldn't open process\n");

	TCHAR szExePath[MAX_PATH];
	GetModuleFileNameEx(hProcess, NULL, szExePath, MAX_PATH);
	printf("%S\n", szExePath);
	DWORD dwBaseAddr = GetModuleBase(szExePath);
	printf("base addr = 0x%x\n", GetModuleBase(szExePath));

	DWORD dwSigLoc = FindPattern(hProcess, dwBaseAddr + 0x20000, 0x30000, "\xEB\x02\x33\xC0\x50\xC7\x45", "xxxxxxx");
	printf("%x\n", dwSigLoc);
	if (!dwSigLoc)
		fail("couldn't locate superhex pointer");	
	DWORD ppSuperhex, pSuperhex;
	RPM(dwSigLoc + 0xD, ppSuperhex); // sick double dereference xd
	printf("%x\n", ppSuperhex);
	RPM(ppSuperhex, pSuperhex);
	printf("pSuperhex = %x\n", pSuperhex);
	printf("offset = %x\n", pSuperhex - dwBaseAddr);

	// spin until ingame
	int stage = 1;
	for (; stage == 1; RPM((DWORD)pSuperhex + (DWORD)offsetof(superhex_t, gamestate.stage), stage))
	{
		Sleep(100);
	}

	bool lDown, rDown;
	while (true)
	{
		//WPM_val(pSuperhex + offsetof(superhex_t, gamestate.baseRotation), 0);
		if (GetAsyncKeyState(VK_F7)) 
			break;
		superhex_t superhex;
		RPM_size(pSuperhex, superhex, offsetof(superhex_t, gamestate)+sizeof(gamestate_t));
		if (superhex.gamestate.gameover2 || superhex.gamestate.gameover || superhex.gamestate.stage == -1)
		{
			Sleep(100);
			continue;
		}
		//printf("\n\n\n\n\n\n");
		int sides = superhex.gamestate.axisCount;
		float playerSection = (superhex.gamestate.playerRotation) * sides / 360.f;

		int maxDist = 145 - superhex.gamestate.wallSpeed; // distance below which walls no longer hurt us

		for (int i = 0; i < sides; i++)
			floors[i] = maxDist;
		for (int i = 0; i < sides; i++)
			ceilings[i] = 999999;

		for (int i = 0; i < superhex.gamestate.wallCnt; i++)
		{
			//if (superhex.gamestate.walls[i].section == 0)
			//	WPM_val(pSuperhex + offsetof(superhex_t, gamestate.walls[i].width), 10000);
			//if (superhex.gamestate.walls[i].section == 1)
			//	WPM_val(pSuperhex + offsetof(superhex_t, gamestate.walls[i].enabled), 0);
		}

		for (int i = 0; i < superhex.gamestate.wallCnt; i++)
		{
			wall_t wall = superhex.gamestate.walls[i];
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

		//system("clear");
		int hedge = maxDist * (360 / sides / 10) + 50; // min space required to fit thru section
		printf("<%d> <%d> ", maxDist, hedge);
		for (int i = 0; i < sides; i++)
			printf("%d.%d ", floors[i], ceilings[i]);
		printf("\n");

		int bestPos = playerSection;
		float bestScore = 0;
		int bestDir = 0;
		for (int i = 0; i < sides; i++) {
			for (int dir = -1; dir < 2; dir++) {
				if (dir != 0) {
					float score = evaluateCandPos(i, playerSection, dir, sides, hedge);
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
		prevBestPos = bestDir;

		if (bestDir == 0) {
			float thang = fmodf(playerSection, 1.f)-.5f;
			//printf("%f\n", thang);
			if (thang > .1f) {
				bestDir = -1;
			}
			else if (thang < -.1f) {
				bestDir = 1;
			}
		}

		if (bestDir == 1)
		{
			lDown = true;
			WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + LeftArrow, (char)1);
			if (rDown)
				WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + RightArrow, (char)0);
			rDown = false;
		}
		else if (bestDir == -1)
		{
			rDown = true;
			WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + RightArrow, (char)1);
			if (lDown)
				WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + LeftArrow, (char)0);
			lDown = false;
		}
		else
		{
			if (lDown)
				WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + LeftArrow, (char)0);
			if (rDown)
				WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + RightArrow, (char)0);
			lDown = false;
			rDown = false;
		}
		Sleep(3);
	}

	CloseHandle(hProcess);
	system("pause");
    return 0;
}

