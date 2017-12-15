#include "stdafx.h"
#include "memory.h"
#include "process.h"
#include "Superhex.h"

// fuck me sideways
#define fail(...) {fprintf(stderr, __VA_ARGS__); system("pause"); return 1;}

int section(int s, int i)
{
	return (int)(360 / s * (i + 0.5));
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
		WPM_val(pSuperhex + offsetof(superhex_t, gamestate.baseRotation), 0);
		if (GetAsyncKeyState(VK_F7)) 
			break;
		superhex_t superhex;
		RPM_size(pSuperhex, superhex, offsetof(superhex_t, gamestate)+sizeof(gamestate_t));
		if (superhex.gamestate.gameover2 || superhex.gamestate.gameover || superhex.gamestate.stage == -1)
		{
			Sleep(100);
			continue;
		}

		int sides = superhex.gamestate.axisCount;
		float playerSection = (superhex.gamestate.playerRotation + superhex.gamestate.velocity) * sides / 360.f;

		int distances[6];
		for (int i = 0; i < sides; i++)
			distances[i] = 999999;
		for (int i = 0; i < superhex.gamestate.wallCnt; i++)
		{
			wall_t wall = superhex.gamestate.walls[i];
			if (!wall.checkCollisions)
				continue;
			int farDist = wall.distance + wall.width; // wall range from wall.distance to wall.distance + width
			// 150 is critical distance
			if (wall.distance <= 150 && farDist >= 150) // second conditional redundant tbh; width always >0
			{
				distances[wall.section] = 150; // its impeded do not go here.
			}
			else if (wall.distance < distances[wall.section])
			{
				distances[wall.section] = wall.distance;
			}
		}

		float best = -1;
		int dist = -1;
		printf("%d ", (int)playerSection);
		for (int i = 0; i < sides; i++)
		{
			//if (distances[i] == 999999) // no walls found in this section, let's go here!
			//{
			//	best = i + 0.5f;
			//	break;
			//}
			float ccwDist = fmodf(i + .5f - playerSection, sides); // sections numbered counterclockwise
			float cwDist = fmodf(playerSection - i - .5f, sides);
			if (ccwDist < 0.f) ccwDist += 6.f; // are you fucking kidding me
			if (cwDist < 0.f) cwDist += 6.f;
			int weightedDist = distances[i] - fminf(ccwDist, cwDist);
			printf("%d ", (int)fminf(ccwDist, cwDist));
			if (best == -0.5f || weightedDist > dist)// || (delta == 0 && fmodf(i - playerSection, 6.f) < fmodf(best - playerSection, 6.f)))
			{
				dist = weightedDist;
				best = i + 0.5f;
			}
		}	
		printf("\n");
		//WPM_val(pSuperhex + offsetof(struct superhex, gamestate.baseRotation), 0);
		if (best != -0.5f && fabsf(best - playerSection) > 0.1f)
		{
			float ccwDist = fmodf(best - playerSection, 6.f); // sections numbered counterclockwise
			float cwDist = fmodf(playerSection - best, 6.f);
			if (ccwDist < 0.f) ccwDist += 6.f; // are you fucking kidding me
			if (cwDist < 0.f) cwDist += 6.f;
			if (cwDist <= ccwDist) // r = clockwise, l = CCW
			{
				rDown = true;
				WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + RightArrow, (char)1);
				if (lDown)
					WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + LeftArrow, (char)0);
			}
			else
			{
				lDown = true;
				WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + LeftArrow, (char)1);
				if (rDown)
					WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + RightArrow, (char)0);
			}
			
		}
		else
		{
			if (lDown)
				WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + LeftArrow, (char)0);
			if (rDown)
				WPM_val(pSuperhex + offsetof(superhex_t, buttonStates) + RightArrow, (char)0);
		}
		Sleep(5);
	}

	CloseHandle(hProcess);
	system("pause");
    return 0;
}

