#include "stdafx.h"
#include <consoleapi.h>
#include "Console.h"

static bool m_bCreated;
static PHANDLER_ROUTINE ConCtrlHandler;

void CreateConsole(PHANDLER_ROUTINE CtrlHandler)
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w+", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

	if (ConCtrlHandler = CtrlHandler)
		SetConsoleCtrlHandler(ConCtrlHandler, TRUE);

	SetConsoleTitleA("Debug console");

	printf("Loaded\n");

	m_bCreated = true;
}

void CloseConsole()
{
	if (!m_bCreated)
		return;

	if (ConCtrlHandler)
		SetConsoleCtrlHandler(ConCtrlHandler, FALSE);

	fclose(stdout);
	fclose(stdin);

	FreeConsole();
	m_bCreated = false;
}

void ClearConsole()
{
	const HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	const COORD topLeft = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO screen;
	GetConsoleScreenBufferInfo(hStdout, &screen);
	const DWORD dwLength = screen.dwSize.X * screen.dwSize.Y;

	DWORD _; // NOTE: as of Win10, this is no longer optional!
	FillConsoleOutputCharacterA(hStdout, ' ', dwLength, topLeft, &_);
	FillConsoleOutputAttribute(hStdout, screen.wAttributes, dwLength, topLeft, &_);
	SetConsoleCursorPosition(hStdout, topLeft);
}
