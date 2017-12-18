#include "stdafx.h"

// so you think you're too good for GetWindowThreadProcessId huh??
DWORD getPID(LPTSTR szFileName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(hSnapshot, &pe);
	do
	{
		if (!wcscmp(szFileName, pe.szExeFile))
		{
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &pe));
	return 0;
}

int main()
{
	DWORD dwPid = getPID(_T("superhexagon.exe"));
	printf("pid=%d\n", dwPid);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

	LPCSTR szPath = R"(E:\Documents\Visual Studio 2015\Projects\Superhex\Debug\SuperHexInternal.dll)";
	int len = strlen(szPath);
	LPVOID pBuf = VirtualAllocEx(hProcess, NULL, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(hProcess, pBuf, szPath, len, NULL);
	CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA"), pBuf, NULL, NULL);
	CloseHandle(hProcess);

	return 0;
}

