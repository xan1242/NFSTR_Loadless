//
// Need for Speed The Run - Loadless timer variable setup (for speedrunning & LiveSplit)
// by Xan / Tenjoin
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <thread>
#include "includes/injector/injector.hpp"
#include "includes/patterns.hpp"

uintptr_t ptr_GameLoadingScreenSomething;
uintptr_t ptr_DifficultySelectSomething;
uintptr_t ptr_GameStartSomething;
uintptr_t ptr_ContinueTheRunSomething;
uintptr_t ptr_BeginNextStageSomething;


uintptr_t ptr_IsLoading; // 0x250D8C1
uintptr_t ptr_IsSelectedDifficulty; // 0x250D8C2
uintptr_t ptr_IsStarted; // 0x250D8C3
uintptr_t ptr_IsContinued; // 0x250D8C4
uintptr_t ptr_ContinueCounter; // 0x250D8C5
uintptr_t ptr_BeginNextStageCounter; // 0x250D8C6

void __stdcall LoadingScreenSomething(uintptr_t unk)
{
	uintptr_t that;
	_asm mov that, ecx

	reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t)>(ptr_GameLoadingScreenSomething)(that, unk);

	// at obj + 4 is the bool where loading is located
	*(bool*)ptr_IsLoading = *(uint8_t*)(that + 4) != 0;
}

void __stdcall DifficultySelectSomething(uintptr_t unk)
{
	uintptr_t that;
	_asm mov that, ecx

	reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t)>(ptr_DifficultySelectSomething)(that, unk);

	*(bool*)ptr_IsSelectedDifficulty = true;
}

void __stdcall GameStartSomething()
{
	uintptr_t that;
	_asm mov that, ecx

	reinterpret_cast<void(__thiscall*)(uintptr_t)>(ptr_GameStartSomething)(that);

	*(bool*)ptr_IsStarted = true;
}

void __stdcall ContinueTheRunSomething()
{
	uintptr_t that;
	_asm mov that, ecx

	reinterpret_cast<void(__thiscall*)(uintptr_t)>(ptr_ContinueTheRunSomething)(that);

	*(bool*)ptr_IsContinued = true;
	*(uint8_t*)ptr_ContinueCounter += 1;
}

uintptr_t BeginNextStageSomething()
{
	*(uint8_t*)ptr_BeginNextStageCounter += 1;

	return reinterpret_cast<uintptr_t(__thiscall*)()>(ptr_BeginNextStageSomething)(); // fb::IUISystem::getInstance
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

	uintptr_t loc_8B29CF = pattern::get_first("8B 0D ? ? ? ? E8 ? ? ? ? 0F B6 C8 51 E9 ? ? ? ? 3D", 6);
	if (!loc_8B29CF)
		return;

	uintptr_t loc_887633 = pattern::get_first("8B 44 24 0C 8B 0D ? ? ? ? 50 E8 ? ? ? ? 8D 4C 24 04 E8", 0xB);
	if (!loc_887633)
		return;

	uintptr_t loc_8821F9 = pattern::get_first("8B 0D ? ? ? ? 85 C9 74 14 E8 ? ? ? ? 85 C0 74 0B 8B 0D", 0x19);
	if (!loc_8821F9)
		return;

	uintptr_t loc_8825A2 = pattern::get_first("68 AC 75 63 F7 FF D0 5E C2 0C 00", -0x12);
	if (!loc_8825A2)
		return;

	uintptr_t vTable = *(uintptr_t*)(loc_8FE6B0 + 2);
	ptr_GameLoadingScreenSomething = *(uintptr_t*)vTable;
	injector::WriteMemory(vTable, &LoadingScreenSomething, true);

	ptr_GameStartSomething = static_cast<uintptr_t>(injector::GetBranchDestination(loc_8B29CF));
	injector::MakeCALL(loc_8B29CF, GameStartSomething);

	ptr_DifficultySelectSomething = static_cast<uintptr_t>(injector::GetBranchDestination(loc_887633));
	injector::MakeCALL(loc_887633, DifficultySelectSomething);

	ptr_ContinueTheRunSomething = static_cast<uintptr_t>(injector::GetBranchDestination(loc_8821F9));
	injector::MakeCALL(loc_8821F9, ContinueTheRunSomething);

	ptr_BeginNextStageSomething = static_cast<uintptr_t>(injector::GetBranchDestination(loc_8825A2));
	injector::MakeCALL(loc_8825A2, BeginNextStageSomething);

	// dereference 0xCB8A38 to get 0x250D8C8, which is where the loading bools will live
	uintptr_t loc_250D8C8 = *(uintptr_t*)(loc_CB8A38 + 4);
	ptr_IsLoading = loc_250D8C8 - 7;
	ptr_IsSelectedDifficulty = loc_250D8C8 - 6;
	ptr_IsStarted = loc_250D8C8 - 5;
	ptr_IsContinued = loc_250D8C8 - 4;
	ptr_ContinueCounter = loc_250D8C8 - 3;
	ptr_BeginNextStageCounter = loc_250D8C8 - 2;

	injector::UnprotectMemory(ptr_IsLoading, 8, dummy);

	*(bool*)ptr_IsLoading = false;
	*(bool*)ptr_IsSelectedDifficulty = false;
	*(bool*)ptr_IsStarted = false;
	*(bool*)ptr_IsContinued = false;
	*(uint8_t*)ptr_ContinueCounter = 0;
	*(uint8_t*)ptr_BeginNextStageCounter = 0;
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
