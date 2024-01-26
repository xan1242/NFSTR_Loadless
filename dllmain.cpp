//
// Need for Speed The Run - Loadless timer variable setup (for speedrunning & LiveSplit)
// by Xan / Tenjoin
//
// Stores the "IsLoading" bool at 0x250D8C8
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <thread>
#include "includes/injector/injector.hpp"
#include "includes/patterns.hpp"

uintptr_t ptr_GameLoadingScreenSomething;
uintptr_t ptr_IsLoading; // 0x250D8C8

void __stdcall LoadingScreenSomething(uintptr_t unk)
{
	uintptr_t that;
	_asm mov that, ecx

	reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t)>(ptr_GameLoadingScreenSomething)(that, unk);

	// at obj + 4 is the bool where loading is located
	*(int*)ptr_IsLoading = *(uint8_t*)(that + 4) != 0;
}


void Init()
{
	DWORD dummy;

	pattern::Win32::Init();

	uintptr_t loc_8FE6B0 = pattern::get_first("68 D4 AC 7A 2F E8 ? ? ? ? 8B 0D ? ? ? ? 57 68 AB BB BB DA", -0x16);
	if (!loc_8FE6B0)
		return;

	uintptr_t loc_CB8A38 = pattern::get_first("C7 44 24 20 ? ? ? ? C7 44 24 24 82 01 00 00");
	if (!loc_CB8A38)
		return;

	uintptr_t vTable = *(uintptr_t*)(loc_8FE6B0 + 2);
	ptr_GameLoadingScreenSomething = *(uintptr_t*)vTable;
	injector::WriteMemory(vTable, &LoadingScreenSomething, true);

	// dereference 0xCB8A38 to get 0x250D8C8, which is where the loading bool will live
	ptr_IsLoading = *(uintptr_t*)(loc_CB8A38 + 4);
	injector::UnprotectMemory(ptr_IsLoading, sizeof(int), dummy);

	*(int*)ptr_IsLoading = 0;
}

void DetectGame()
{
	pattern::Win32::Init();

	uintptr_t loc_CB8A38 = pattern::get_first("C7 44 24 20 ? ? ? ? C7 44 24 24 82 01 00 00");
	while (loc_CB8A38 == 0)
	{
		loc_CB8A38 = pattern::get_first("C7 44 24 20 ? ? ? ? C7 44 24 24 82 01 00 00");
		Sleep(1);
	}

	Init();
}


BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		std::thread(DetectGame).detach();
	}
	return TRUE;
}
