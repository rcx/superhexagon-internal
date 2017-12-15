#pragma once

#define RPM_ptr(addr, pBuf) ReadProcessMemory(hProcess, (LPCVOID)(addr), pBuf, sizeof(pBuf), NULL)
#define RPM(addr, buf) ReadProcessMemory(hProcess, (LPCVOID)(addr), &(buf), sizeof(buf), NULL)
#define RPM_size(addr, buf, size) ReadProcessMemory(hProcess, (LPCVOID)(addr), &(buf), size, NULL)
#define WPM_buf(addr, buf) WriteProcessMemory(hProcess, (LPVOID)(addr), buf, sizeof(buf), NULL)
#define WPM_val(addr, val) { auto lmao = val; WriteProcessMemory(hProcess, (LPVOID)(addr), &lmao, sizeof(val), NULL); }

// yea i actually give no fucks, im going to define function bodies in header files because screw it
// whats_performance
DWORD FindPattern(HANDLE hProcess, DWORD dwBase, DWORD dwScanLen, LPCSTR szPattern, LPCSTR szMask)
{
	char buf[1024];
	int patternLen = strlen(szPattern);
	for (DWORD bufOffset = 0; bufOffset < dwScanLen; bufOffset += sizeof(buf) - patternLen)
	{
		RPM_ptr(dwBase + bufOffset, buf);
		int scanEnd = sizeof(buf) - patternLen;
		for (int i = 0; i < scanEnd; i++)
		{
			bool valid = true;
			for (const char* pByte = &buf[i], *pPat = szPattern, *pMask = szMask; *pMask; pByte++, pPat++, pMask++)
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
				return dwBase + bufOffset + i;
		}
	}
	return NULL;
}
