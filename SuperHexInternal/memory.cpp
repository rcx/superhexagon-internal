#include "stdafx.h"
#include "memory.h"

DWORD FindPattern(DWORD dwBase, DWORD dwScanLen, LPCSTR szPattern, LPCSTR szMask)
{
	const char* pEnd = (const char*)dwBase + dwScanLen;
	for (const char* ptr = (const char*)dwBase; ptr < pEnd; ptr++)
	{
		bool valid = true;
		int i = 0;
		for (const char *pByte = ptr, *pPat = szPattern, *pMask = szMask; *pMask; pByte++, pPat++, pMask++)
		{
			if (*pMask == '?')
				continue;
			if (*pByte != *pPat)
			{
				valid = false;
				break;
			}
		}
		if (valid)
			return (DWORD)ptr;
	}
	return NULL;
}
