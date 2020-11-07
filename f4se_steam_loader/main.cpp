#include "f4se_common/f4se_version.h"
#include "f4se_common/Utilities.h"
#include "f4se_common/SafeWrite.h"
#include "f4se_loader_common/IdentifyEXE.h"
#include <shlobj.h>
#include <intrin.h>
#include <string>

IDebugLog	gLog;
HANDLE		g_dllHandle;

static void OnAttach(void);
static void HookMain(void * retAddr);
static void HookIAT();

BOOL WINAPI DllMain(HANDLE procHandle, DWORD reason, LPVOID reserved)
{
	if(reason == DLL_PROCESS_ATTACH)
	{
		g_dllHandle = procHandle;

		OnAttach();
	}

	return TRUE;
}

static void OnAttach(void)
{
	gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\f4se_steam_loader.log");
	gLog.SetPrintLevel(IDebugLog::kLevel_Error);
	gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

	FILETIME	now;
	GetSystemTimeAsFileTime(&now);

	_MESSAGE("f4se loader %08X (steam) %08X%08X %s", PACKED_F4SE_VERSION, now.dwHighDateTime, now.dwLowDateTime, GetOSInfoStr().c_str());
	_MESSAGE("loader base addr = %016I64X", g_dllHandle);
	_MESSAGE("exe base addr = %016I64X", GetModuleHandle(NULL));

	// hook an imported function early so we can inject our code 
	HookIAT();
}

typedef WORD (* ___crtGetShowWindowMode)(void);
___crtGetShowWindowMode __crtGetShowWindowMode_Original = NULL;

static WORD __crtGetShowWindowMode_Hook(void)
{
	HookMain(_ReturnAddress());

	return __crtGetShowWindowMode_Original();
}

static void HookIAT()
{
	___crtGetShowWindowMode * iat = (___crtGetShowWindowMode *)GetIATAddr(GetModuleHandle(NULL), "MSVCR110.dll", "__crtGetShowWindowMode");
	if(iat)
	{
		_MESSAGE("found iat at %016I64X", iat);

		__crtGetShowWindowMode_Original = *iat;
		_MESSAGE("original thunk %016I64X", __crtGetShowWindowMode_Original);

		SafeWrite64(uintptr_t(iat), (UInt64)__crtGetShowWindowMode_Hook);
		_MESSAGE("patched iat");
	}
	else
	{
		_MESSAGE("couldn't find __crtGetShowWindowMode");
	}
}

bool hookInstalled = false;
std::string g_dllPath;

static void HookMain(void * retAddr)
{
	if(hookInstalled)
		return;
	else
		hookInstalled = true;

	_MESSAGE("HookMain: thread = %d retaddr = %016I64X", GetCurrentThreadId(), retAddr);

	std::string runtimePath = GetRuntimePath();
	_MESSAGE("runtimePath = %s", runtimePath.c_str());

	bool isEditor = false;

	// check version etc
	std::string		dllSuffix;
	ProcHookInfo	procHookInfo;

	if(!IdentifyEXE(runtimePath.c_str(), isEditor, &dllSuffix, &procHookInfo))
	{
		_ERROR("unknown exe");
		return;
	}

	const char	* dllPrefix = (isEditor == false) ? "\\f4se_" : "\\f4se_editor_";

	g_dllPath = GetRuntimeDirectory() + dllPrefix + dllSuffix + ".dll";
	_MESSAGE("dll = %s", g_dllPath.c_str());

	LoadLibrary(g_dllPath.c_str());
}
