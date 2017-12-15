#pragma once

// yea so psapi wasnt giving the right one so lets just parse it
DWORD GetModuleBase(LPTSTR szPath)
{
	BY_HANDLE_FILE_INFORMATION bhfi;
	HANDLE hMapping;
	char *lpBase;
	HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	if (!GetFileInformationByHandle(hFile, &bhfi))
		return NULL;

	hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, bhfi.nFileSizeHigh, bhfi.nFileSizeLow, NULL);
	if (!hMapping)
		return NULL;
	lpBase = (char *)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, bhfi.nFileSizeLow);
	if (!lpBase)
		return NULL;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)lpBase;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	PIMAGE_NT_HEADERS32 ntHeader = (PIMAGE_NT_HEADERS32)(lpBase + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	DWORD dwEntryPoint = ntHeader->OptionalHeader.ImageBase;

	UnmapViewOfFile((LPCVOID)lpBase);
	CloseHandle(hMapping);
	CloseHandle(hFile);

	return dwEntryPoint;
}

// i was originally just going to do the hack where you get the pid from the window handle and you get the window handle using the title but then i decided maybe i would do things proper
// look where that got me
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
	}
	while (Process32Next(hSnapshot, &pe));
	return 0;
}
