#include "f4se_common/f4se_version.h"
#include "f4se_common/Utilities.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"
#include "f4se_common/SafeWrite.h"
#include <shlobj.h>
#include "common/IFileStream.h"
#include "Hooks_ObScript.h"
#include "Hooks_Papyrus.h"
#include "Hooks_Scaleform.h"
#include "Hooks_Gameplay.h"
#include "Hooks_Memory.h"
#include "Hooks_GameData.h"
#include "Hooks_SaveLoad.h"
#include "Hooks_Input.h"
#include "Hooks_Debug.h"
#include "Hooks_Threads.h"
#include "Hooks_Camera.h"
#include "PluginManager.h"
#include "InternalSerialization.h"

IDebugLog gLog;
void * g_moduleHandle = nullptr;

void WaitForDebugger(void)
{
	while(!IsDebuggerPresent())
	{
		Sleep(10);
	}

	Sleep(1000 * 2);
}

static bool isInit = false;

void F4SE_Initialize(void)
{
	if(isInit) return;
	isInit = true;

	gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\f4se.log");

#ifndef _DEBUG
	__try {
#endif

		FILETIME	now;
		GetSystemTimeAsFileTime(&now);

		_MESSAGE("F4SE runtime: initialize (version = %d.%d.%d %08X %08X%08X, os = %s)",
			F4SE_VERSION_INTEGER, F4SE_VERSION_INTEGER_MINOR, F4SE_VERSION_INTEGER_BETA, RUNTIME_VERSION,
			now.dwHighDateTime, now.dwLowDateTime, GetOSInfoStr().c_str());

		_MESSAGE("imagebase = %016I64X", GetModuleHandle(NULL));
		_MESSAGE("reloc mgr imagebase = %016I64X", RelocationManager::s_baseAddr);

#ifdef _DEBUG
		SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

		WaitForDebugger();
#endif

		if(!g_branchTrampoline.Create(1024 * 64))
		{
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return;
		}

		if(!g_localTrampoline.Create(1024 * 64, g_moduleHandle))
		{
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return;
		}

		Hooks_Debug_Init();
		Hooks_ObScript_Init();
		Hooks_Papyrus_Init();
		Hooks_Scaleform_Init();
		Hooks_Gameplay_Init();
		Hooks_GameData_Init();
		Hooks_SaveLoad_Init();
		Hooks_Input_Init();
		Hooks_Threads_Init();
		Hooks_Camera_Init();

		g_pluginManager.Init();

		Hooks_Debug_Commit();
		Hooks_ObScript_Commit();
		Hooks_Papyrus_Commit();
		Hooks_Scaleform_Commit();
		Hooks_Gameplay_Commit();
		Hooks_GameData_Commit();
		Hooks_SaveLoad_Commit();
		Hooks_Input_Commit();
		Hooks_Threads_Commit();
		Hooks_Camera_Commit();

		Init_CoreSerialization_Callbacks();

		FlushInstructionCache(GetCurrentProcess(), NULL, 0);

#ifndef _DEBUG
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		_ERROR("exception thrown during startup");
	}
#endif

	_MESSAGE("init complete");
}

extern "C" {

	void StartF4SE(void)
	{
		F4SE_Initialize();
	}

	BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
	{
		switch(dwReason)
		{
		case DLL_PROCESS_ATTACH:
			g_moduleHandle = (void *)hDllHandle;
			F4SE_Initialize();
			break;

		case DLL_PROCESS_DETACH:
			break;
		};

		return TRUE;
	}

};
