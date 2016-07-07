// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
FILE* g_console = NULL;
void HookZwClose64();

void InitConsoleWindow(void)
{
	int hCrt;
	FILE *hf;
	AllocConsole();
	g_console = freopen("CONOUT$", "w+t", stdout);

	// test code ...  
	printf("InitConsoleWindow OK!\n\n");
}

void CloseConsoleWindow(void) {
	FreeConsole();
	fclose(g_console);
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: {
			HookZwClose64();
			InitConsoleWindow();
			break;
		}
		case DLL_THREAD_ATTACH: {
			break;
		}
		case DLL_THREAD_DETACH: {
			break;
		}
		case DLL_PROCESS_DETACH: {
			CloseConsoleWindow();
			break;
		}
		
	}
	return TRUE;
}

