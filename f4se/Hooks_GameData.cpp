#include "Hooks_GameData.h"
#include "f4se/GameThreads.h"
#include "f4se/GameData.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/SafeWrite.h"
#include "f4se_common/BranchTrampoline.h"
#include "xbyak/xbyak.h"
#include "f4se/PluginManager.h"

typedef UInt64 (* _InitGameDataThread_Run_Original)(InitGameDataThread * thread);
RelocAddr <_InitGameDataThread_Run_Original> InitGameDataThread_Run_Original(0x00D53580);

typedef void (* _GameDataReady_Original)(bool isReady);
// writes to g_isGameDataReady

// 936C94CC97C598DFE8A81979F95A75088F8B44C2+3AF
RelocAddr <_GameDataReady_Original> GameDataReady_Original(0x0082FA50);

UInt64 InitGameDataRun_Hook(InitGameDataThread * thread)
{
	UInt64 res = InitGameDataThread_Run_Original(thread);
	PluginManager::Dispatch_Message(0, F4SEMessagingInterface::kMessage_GameLoaded, (void*)NULL, 0, NULL);
	return res;
}

void GameDataReady_Hook(bool isReady)
{
	// This message is before so if the listener reads from the global it will be the previous value
	PluginManager::Dispatch_Message(0, F4SEMessagingInterface::kMessage_GameDataReady, (void*)isReady, 1, NULL);
	(*g_isGameDataReady) = isReady;
}

void Hooks_GameData_Init()
{

}

void Hooks_GameData_Commit()
{
	// ??_7InitGameDataThread@?A0x1eb8b661@@6B@
	RelocAddr <uintptr_t> InitGameDataThread_Run(0x02D3BF18 + 8); 

	SafeWrite64(InitGameDataThread_Run.GetUIntPtr(), (UInt64)InitGameDataRun_Hook);
	g_branchTrampoline.Write6Branch(GameDataReady_Original.GetUIntPtr(), (uintptr_t)GameDataReady_Hook);
}
