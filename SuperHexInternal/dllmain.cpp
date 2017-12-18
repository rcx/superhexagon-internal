#include "stdafx.h"

#include "console.h"
#include "memory.h"
#include "CVMTHook.h"
#include "Superhex.h"
#include "ai.h"

DWORD CALLBACK cbEjectDLL(LPVOID hModule)
{
	Sleep(100);
	FreeLibraryAndExitThread((HMODULE)hModule, 0);
	return 0;
}

typedef int(__thiscall *MainLoop)(CSuperhex *pThis);

CSuperhex *pSuperhex;
CVMTHook* hkCreateMove;
MainLoop pMainLoop;

int __fastcall cbMainLoop(CSuperhex *pThis) // we can't use __thiscall or msvc gets ANGRY. so we settle for __fastcall.
{
	AiOnTick(pThis);
	return pMainLoop(pThis);
}

void Init()
{
	HMODULE hSuperhex = GetModuleHandleA(NULL);
	printf("hSuperhex = %p", hSuperhex);

	/*
	.text:00439E30     ; int __cdecl main(int argc, const char **argv, const char **envp)
	.text:00439E30     _main           proc near               ; CODE XREF: ___tmainCRTStartup+11D
	...
	.text:00439EB1 018                 jmp     short loc_439EB5
	.text:00439EB3     ; ---------------------------------------------------------------------------
	.text:00439EB3
	.text:00439EB3     loc_439EB3:                             ; CODE XREF: _main+78
	.text:00439EB3 018                 xor     eax, eax
	.text:00439EB5
	.text:00439EB5     loc_439EB5:                             ; CODE XREF: _main+81
	.text:00439EB5 018                 push    eax             ; a1
	.text:00439EB5     ;   } // starts at 439E9F
	.text:00439EB6 01C                 mov     [ebp+var_4], 0FFFFFFFFh
	.text:00439EBD 01C                 mov     pSuperHex3, eax
	*/
	DWORD dwSigLoc = FindPattern((DWORD) hSuperhex + 0x20000, 0x30000, "\xEB\x02\x33\xC0\x50\xC7\x45", "xxxxxxx");
	printf("JMP = %x\n", dwSigLoc);
	if (!dwSigLoc)
	{
		printf("Couldn't locate superhex pointer\n");
		return;
	}

	CSuperhex **ppSuperhex;
	ppSuperhex = *(CSuperhex***)(dwSigLoc + 0xD);
	printf("ppSuperhex = %p\n", ppSuperhex);
	pSuperhex = *ppSuperhex;
	printf("pSuperhex = %p (superhex+%p)\n", pSuperhex, pSuperhex - (DWORD)hSuperhex);

	hkCreateMove = new CVMTHook(pSuperhex);
	pMainLoop = (MainLoop)hkCreateMove->HookVfunc(5, (DWORD)cbMainLoop);
	printf("MainLoop originally = %p\n", pMainLoop);

	printf("Hook in place\n");
}

DWORD CALLBACK cbThreadStart(LPVOID hModule)
{
	Beep(440, 250);
	CreateConsole();
	ClearConsole();

	Init();

	while (!GetAsyncKeyState(VK_F6))
		Sleep(10);
	Beep(440, 250);

	delete hkCreateMove;

	CreateThread(NULL, 0, cbEjectDLL, hModule, NULL, NULL);
	return EXIT_SUCCESS;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, cbThreadStart, hModule, NULL, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

