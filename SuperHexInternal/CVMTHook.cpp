#include "stdafx.h"
#include "CVMTHook.h"

CVMTHook::CVMTHook()
{
}

CVMTHook::CVMTHook(void* pClass)
{
	// Store pointer to class, which is also the pointer to its current VMT
	m_pClassVMT = (PDWORD*) pClass;

	// Store the pointer to the original VMT
	m_vmtOld = *m_pClassVMT;

	// Walk the VMT to count how many functions there are.
	m_nFuncs = 0;
	while (m_vmtOld[m_nFuncs] && !IsBadCodePtr((FARPROC)m_vmtOld[m_nFuncs]))
		m_nFuncs++;

	// Copy old vmt to our new copy
	m_vmtNew = new DWORD[m_nFuncs];
	memcpy(m_vmtNew, m_vmtOld, sizeof(DWORD) * m_nFuncs);
	printf("There are %d funcs\nNew vmt at %p\n", m_nFuncs, m_vmtNew);

	ApplyHook();
}

CVMTHook::~CVMTHook()
{
	Unhook();
	delete m_vmtNew;
}

void CVMTHook::ApplyHook()
{
	printf("write to %p\n", m_pClassVMT);
	*m_pClassVMT = m_vmtNew;
}

void CVMTHook::Unhook()
{
	*m_pClassVMT = m_vmtOld;
}

DWORD CVMTHook::GetVfunc(int index)
{
	if (index >= 0 && index < m_nFuncs)
		return m_vmtOld[index];
	return NULL;
}

DWORD CVMTHook::HookVfunc(int index, DWORD dwTarget)
{
	if (index < 0 || index >= m_nFuncs)
		return NULL;
	m_vmtNew[index] = dwTarget;
	return m_vmtOld[index];
}

int CVMTHook::GetFuncCount()
{
	return m_nFuncs;
}
