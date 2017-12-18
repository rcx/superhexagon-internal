#pragma once

class CVMTHook
{
	// Original VMT
	PDWORD m_vmtOld;

	// My copy of the VMT
	PDWORD m_vmtNew;

	// Pointer to class, whose first value is a pointer to the VMT
	PDWORD* m_pClassVMT;

	int m_nFuncs;

public:
	CVMTHook();
	CVMTHook(void* pClass, int vmtLength);
	~CVMTHook();

	void ApplyHook();
	void Unhook();

	// Gets the original address of the vfunc at the given index. Returns NULL if the index is invalid.
	DWORD GetVfunc(int index);

	// Hooks the vfunc at the given index to point at dwTarget instead. Returns false if the index is invalid.
	DWORD HookVfunc(int index, DWORD dwTarget);

	// Returns the number of vfuncs in the original VMT.
	int GetFuncCount();
};
