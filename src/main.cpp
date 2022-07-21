#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


#include "helpers/minhook/detours.h"

#include "helpers/globals.h"
#include "helpers/dumper/getmodule.h"
#include "helpers/dumper/dumper.h"

struct Vec2 {
	float X, Y;

	Vec2(float x, float y) {
		X = x;
		Y = y;
	};

	void operator /= (int amount) {
		X /= amount;
		Y /= amount;
	}
};

std::vector<std::vector<Vec2>> patterns = { {},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{} };  // enough for 20 magazines (after 20 mags you need to dump)
int selected = 0;
int sizes = 0;

void dump() {
	std::ofstream LogFile("dumped.txt", std::ios_base::app); // saved in rust folder
	
	std::string logstring;
	std::vector<Vec2> pattern = { {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f}, {0.f,0.f} };

	for (auto item : patterns) { // go through all saved patterns
		if (item.size() == 0) // if one pattern is empty (accidentally skipped or something) we skip it
			continue;
		sizes += 1; // counting up how many filled patterns we actually have
		for (int i = 0; i < item.size()-1; ++i) { // adding up the final average pattern with all the magazine patterns we saved
			pattern[i].X += item[i].X;
			pattern[i].Y += item[i].Y;
		}
	}

	for (auto item : pattern) {
		item /= sizes; // getting the actual average of the bullet from all patterns combined
		logstring.append("{" + std::to_string(item.X) + "," + std::to_string(item.Y) + "},");
	}
	LogFile << logstring;
	LogFile.close();
}

void hk_AddPunch(UINT_PTR heldEntity, Vector3 amount, float duration) {

	std::string logstring = "AddPunch AmountX: " + std::to_string(amount.X) + " | AddPunch AmountY: " + std::to_string(amount.Y);

	std::cout << logstring << std::endl;

	patterns[selected].push_back({amount.X, amount.Y});

	amount *= 0; // nulling the recoil since its annoying otherwise

	return funcs::Original_AddPunch(heldEntity, amount, duration);
}

bool init() {
	Globals::g_GameAssemblyBase = reinterpret_cast<UINT_PTR>(Internal::HdnGetModuleBase("GameAssembly.dll"));

	if (!Globals::g_GameAssemblyBase) {
		printf("%s - g_GameAssemblyBase is null.", __FUNCTION__);
		return false;
	}

	funcs::Original_AddPunch = (void(__fastcall*)(UINT_PTR, Vector3, float))(Globals::g_GameAssemblyBase + Dumper::GetMethodOffset("Assembly-CSharp", "", "HeldEntity", "AddPunch"));

	return true;
}

bool start_thread(HMODULE hMod) {
	AllocConsole();
	freopen_s((FILE**)stdout,"CONOUT$", "w", stdout);

	if (!init()) {
		FreeConsole();
		FreeLibraryAndExitThread(hMod, 0);
	}

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)funcs::Original_AddPunch, hk_AddPunch);
	DetourTransactionCommit();

	while (!GetAsyncKeyState(VK_INSERT)) { // Stop Dll via insert button
		if (GetAsyncKeyState(VK_MBUTTON) & 1) // Press Mouse 3 to proceed to next magazine
			selected += 1;

		if (GetAsyncKeyState(VK_END) & 1) { // Press End to actually dump
			selected = 0;
			dump();
			for (auto item : patterns)
				item.clear();
		}
		Sleep(1);
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)funcs::Original_AddPunch, hk_AddPunch);
	DetourTransactionCommit();


	FreeConsole();
	FreeLibraryAndExitThread(hMod,0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dReason, LPVOID lpReserved) {
	switch (dReason) {
	case DLL_PROCESS_ATTACH: {
		CloseHandle(CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(start_thread), hModule, NULL, NULL));
		break;
	}
	}
	return TRUE;
}