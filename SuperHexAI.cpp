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

	_getwch();
	bool lDown, rDown;
	while (true)
	{
		if (GetAsyncKeyState(VK_F7)) 
			break;
		superhex_t superhex;
		RPM_size(pSuperhex, superhex, offsetof(superhex_t, gamestate)+sizeof(gamestate_t));
		if (superhex.gamestate.gameover2)
			break;

		int sides = superhex.gamestate.axisCount;
		float playerSection = (superhex.gamestate.playerRotation + superhex.gamestate.velocity) * sides / 360.f;

		int distances[6];
		for (int i = 0; i < sides; i++)
			distances[i] = 999999;
		for (int i = 0; i < superhex.gamestate.wallCnt; i++)
		{
			wall_t wall = superhex.gamestate.walls[i];
			if (!wall.checkCollisions || wall.distance == 0)
				continue;
			if (wall.distance < distances[wall.section])
				distances[wall.section] = wall.distance;
		}
		for (int i = 0; i < 6;i++)
			printf("%d ", distances[i]);
		printf("\n");

		float best = -1;
		int dist = -1;
		for (int i = 0; i < sides; i++)
		{
			if (distances[i] == 999999)
			{
				best = i + 0.5f;
				break;
			}
			int delta = distances[i] - dist;
			if (best == -0.5f || delta > 0)// || (delta == 0 && fmodf(i - playerSection, 6.f) < fmodf(best - playerSection, 6.f)))
			{
				dist = distances[i];
				best = i + 0.5f;
			}
		}
		//WPM_val(pSuperhex + offsetof(struct superhex, gamestate.baseRotation), 0);
		if (best != -0.5f && fabsf(best - playerSection) > 0.1f)
		{
			float ccwDist = fmodf(best - playerSection, 6.f);
			float cwDist = fmodf(playerSection - best, 6.f);
			if (ccwDist < 0.f) ccwDist += 6.f; // are you fucking kidding me
			if (cwDist < 0.f) cwDist += 6.f;
			//printf("%f %f %f %f\n", best, playerSection, cwDist, ccwDist);
			if (cwDist <= ccwDist)
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
				WPM_val(pSuperhex + 0x429C0, (char)0);
			if (rDown)
				WPM_val(pSuperhex + 0x429C2, (char)0);
		}
	}

	CloseHandle(hProcess);
	system("pause");
    return 0;
}

