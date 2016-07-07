// BypassAnti.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
DWORD g_zwclose_sysid = 0;
DWORD g_syscall64 = 0;
DWORD g_syscallret = 0;
BOOL ZwCloseHooklet(DWORD handle) {
	if (handle == 0x80012345) {
		char buf[128];
		sprintf(buf, "ZwClose handle:%x\n", handle);
		printf(buf);
		return TRUE;
	}
	return FALSE;
}
__declspec(naked) void SysCallHookWrapper() {
	_asm {
		pushad
		pushfd
		cmp g_zwclose_sysid,eax
		jne goon
		push dword ptr [esp+ 0x2c]
		call ZwCloseHooklet
		add esp,4
		test eax,eax
		jz goon
		popfd
		popad
		pop eax
		mov eax, 0xC0000008
		ret 4
goon:
		popfd
		popad
		mov edx,dword ptr fs:[0x30]
		push g_syscallret
		ret
	}
}

void Hook(void* from, void* to, unsigned char style)
{
	DWORD oldpf;
	VirtualProtect(from, 4, PAGE_EXECUTE_READWRITE, &oldpf);
	auto offset = (unsigned int)to - (unsigned int)from - 5;
	*(unsigned char*)from = style;
	*(unsigned int*)((unsigned int)from + 1) = offset;
}

void HookZwClose64() {
	HMODULE hNtDll = GetModuleHandle(_T("ntdll.dll"));
	PVOID pZwClose = GetProcAddress(hNtDll, "ZwClose");
	g_zwclose_sysid = *(unsigned int*)((unsigned int)pZwClose + 1);
	g_syscall64 = *(unsigned int*)((unsigned int)pZwClose + 6);
	g_syscallret = g_syscall64 + 7;
	Hook((PVOID)g_syscall64, SysCallHookWrapper, 0xe9);
}