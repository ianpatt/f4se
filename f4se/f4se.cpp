#include "f4se_common/f4se_version.h"
#include "f4se_common/Utilities.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"
#include "f4se_common/SafeWrite.h"
#include <cassert>
#include <cstring>
#include <shlobj.h>
#include <vector>
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

void F4SE_Preinit();
void F4SE_Initialize();

const size_t kPoolSize = 1024 * 64;
const size_t kPoolReserveSize = 512;

// api-ms-win-crt-runtime-l1-1-0.dll
typedef int (*__initterm_e)(_PIFV *, _PIFV *);
__initterm_e _initterm_e_Original = nullptr;

typedef char * (*__get_narrow_winmain_command_line)();
__get_narrow_winmain_command_line _get_narrow_winmain_command_line_Original = NULL;

// runs before global initializers
int __initterm_e_Hook(_PIFV * a, _PIFV * b)
{
	// could be used for plugin optional preload

	F4SE_Preinit();

	return _initterm_e_Original(a, b);
}

// runs after global initializers
char * __get_narrow_winmain_command_line_Hook()
{
	// the usual load time

	F4SE_Initialize();

	return _get_narrow_winmain_command_line_Original();
}

void InstallBaseHooks()
{
	gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\f4se.log");

	HANDLE exe = GetModuleHandle(nullptr);

	// fetch functions to hook
	auto * initterm = (__initterm_e *)GetIATAddr(exe, "api-ms-win-crt-runtime-l1-1-0.dll", "_initterm_e");
	auto * cmdline = (__get_narrow_winmain_command_line *)GetIATAddr(exe, "api-ms-win-crt-runtime-l1-1-0.dll", "_get_narrow_winmain_command_line");

	// hook them
	if(initterm)
	{
		_initterm_e_Original = *initterm;
		SafeWrite64(uintptr_t(initterm), UInt64(__initterm_e_Hook));
	}
	else
	{
		_ERROR("couldn't find _initterm_e");
	}

	if(cmdline)
	{
		_get_narrow_winmain_command_line_Original = *cmdline;
		SafeWrite64(uintptr_t(cmdline), UInt64(__get_narrow_winmain_command_line_Hook));
	}
	else
	{
		_ERROR("couldn't find _get_narrow_winmain_command_line");
	}
}

void WaitForDebugger(void)
{
	while(!IsDebuggerPresent())
	{
		Sleep(10);
	}

	Sleep(1000 * 2);
}

bool ShouldWaitForDebugger()
{
	const char* env = "F4SE_WAITFORDEBUGGER";
	const auto printErr = [=]()
	{
		const DWORD err = GetLastError();
		if (err != ERROR_ENVVAR_NOT_FOUND)
			_ERROR("failed to get %s with error code %u", env, err);
	};

	std::vector<char> buf;
	const DWORD len = GetEnvironmentVariableA(env, buf.data(), 0);
	if (len == 0) 	{
		printErr();
		return false;
	}

	buf.resize(len, '\0');
	if (GetEnvironmentVariableA(env, buf.data(), buf.size()) == 0) 	{
		printErr();
		return false;
	}

	return std::strcmp(buf.data(), "1") == 0;
}

void F4SE_Preinit()
{
	static bool runOnce = false;
	if(runOnce) return;
	runOnce = true;

	FILETIME	now;
	GetSystemTimeAsFileTime(&now);

	_MESSAGE("F4SE runtime: initialize (version = %d.%d.%d %08X %08X%08X, os = %s)",
		F4SE_VERSION_INTEGER, F4SE_VERSION_INTEGER_MINOR, F4SE_VERSION_INTEGER_BETA, RUNTIME_VERSION,
		now.dwHighDateTime, now.dwLowDateTime, GetOSInfoStr().c_str());

	_MESSAGE("imagebase = %016I64X", GetModuleHandle(NULL));
	_MESSAGE("reloc mgr imagebase = %016I64X", RelocationManager::s_baseAddr);

	if (ShouldWaitForDebugger())
	{
		SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
		WaitForDebugger();
	}

	if(!g_branchTrampoline.Create(kPoolSize))
	{
		_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
		return;
	}

	if(!g_localTrampoline.Create(kPoolSize, g_moduleHandle))
	{
		_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
		return;
	}

	const auto initAlloc = [=](PluginAllocator& alloc, BranchTrampoline& trampoline)
	{
		const auto size = kPoolSize - kPoolReserveSize;
		alloc.Initialize(trampoline.Allocate(size), size);
	};
	initAlloc(g_branchPluginAllocator, g_branchTrampoline);
	initAlloc(g_localPluginAllocator, g_localTrampoline);

	// scan plugin folder
	g_pluginManager.Init();

	// preload plugins
	g_pluginManager.InstallPlugins(PluginManager::kPhase_Preload);

	_MESSAGE("preinit complete");
}

void F4SE_Initialize(void)
{
	static bool isInit = false;
	if(isInit) return;
	isInit = true;

	F4SE_Preinit();	// move after updating load timing

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

	g_pluginManager.InstallPlugins(PluginManager::kPhase_Load);
	g_pluginManager.LoadComplete();

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

	const auto printAlloc = [=](BranchTrampoline& pool, const char* name)
	{
		const auto allocated = kPoolReserveSize - pool.Remain();
		assert(allocated <= kPoolReserveSize);
		_DMESSAGE("F4SE allocated %u bytes from %s pool", allocated, name);
	};
	printAlloc(g_branchTrampoline, "branch");
	printAlloc(g_localTrampoline, "local");

	Init_CoreSerialization_Callbacks();

	FlushInstructionCache(GetCurrentProcess(), NULL, 0);

	_MESSAGE("init complete");
}

extern "C" {
	void StartF4SE(void)
	{
		InstallBaseHooks();
	}

	BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
	{
		switch(dwReason)
		{
		case DLL_PROCESS_ATTACH:
			g_moduleHandle = (void *)hDllHandle;
			break;

		case DLL_PROCESS_DETACH:
			break;
		};

		return TRUE;
	}
};
