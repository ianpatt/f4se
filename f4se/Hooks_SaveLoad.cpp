#include "Hooks_SaveLoad.h"
#include "GameForms.h"

#include "f4se_common/BranchTrampoline.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/SafeWrite.h"

#include "f4se/PluginManager.h"
#include "f4se/Serialization.h"

#include "xbyak/xbyak.h"

class BGSSaveLoadGame;

typedef void (* _SaveGame)(BGSSaveLoadGame * saveLoadMgr, const char * name, UInt8 unk1);
RelocAddr <_SaveGame> SaveGame(0x00CDE4F0);
_SaveGame SaveGame_Original = nullptr;

typedef bool (* _LoadGame)(BGSSaveLoadGame * saveLoadMgr, const char * name, UInt8 unk1, void * unk2);
RelocAddr <_LoadGame> LoadGame(0x00CDE9F0);
_LoadGame LoadGame_Original = nullptr;

RelocAddr <uintptr_t> NewGame_Enter(0x012A23E0 + 0x5A);

typedef void (* _DeleteSaveGame)(BGSSaveLoadGame * saveLoadMgr, const char * name, UInt32 unk1, UInt8 unk2);
RelocAddr <_DeleteSaveGame> DeleteSaveGame(0x00CECF50);
_DeleteSaveGame DeleteSaveGame_Original = nullptr;

void SaveGame_Hook(BGSSaveLoadGame * saveLoadMgr, const char * saveName, UInt8 unk1)
{
#ifdef _DEBUG
	_MESSAGE("Executing BGSSaveLoadGame::SaveGame_Hook. saveName: %s", saveName);
#endif

	Serialization::SetSaveName(saveName);
	PluginManager::Dispatch_Message(0, F4SEMessagingInterface::kMessage_PreSaveGame, (void*)saveName, strlen(saveName), NULL);
	SaveGame_Original(saveLoadMgr, saveName, unk1);
	PluginManager::Dispatch_Message(0, F4SEMessagingInterface::kMessage_PostSaveGame, (void*)saveName, strlen(saveName), NULL);
	Serialization::SetSaveName(NULL);
}

bool LoadGame_Hook(BGSSaveLoadGame * saveLoadMgr, const char * saveName, UInt8 unk1, void * unk2)
{
#ifdef _DEBUG
	_MESSAGE("Executing BGSSaveLoadGame::LoadGame_Hook. saveName: %s", saveName);
#endif

	Serialization::SetSaveName(saveName);
	PluginManager::Dispatch_Message(0, F4SEMessagingInterface::kMessage_PreLoadGame, (void*)saveName, strlen(saveName), NULL);
	bool result = LoadGame_Original(saveLoadMgr, saveName, unk1, unk2);
	PluginManager::Dispatch_Message(0, F4SEMessagingInterface::kMessage_PostLoadGame, (void*)result, 1, NULL);
	Serialization::SetSaveName(NULL);
	return result;
}

void DeleteSaveGame_Hook(BGSSaveLoadGame * saveLoadMgr, const char * saveName, UInt32 unk1, UInt8 unk2)
{
#ifdef _DEBUG
	_MESSAGE("Executing BGSSaveLoadGame::DeleteSaveGame_Hook. saveName: %s", saveName);
#endif

	PluginManager::Dispatch_Message(0, F4SEMessagingInterface::kMessage_DeleteGame, (void*)saveName, strlen(saveName), NULL);
	DeleteSaveGame_Original(saveLoadMgr, saveName, unk1, unk2);
	Serialization::HandleDeleteSave(saveName);
}

class TESQuest;

UInt8 TESQuest::NewGame_Hook(UInt8 * unk1, UInt8 unk2)
{
#ifdef _DEBUG
	_MESSAGE("Executing TESQuest::NewGame_Hook.");
#endif
	UInt8 ret = CALL_MEMBER_FN(this, NewGame_Internal)(unk1, unk2);
	PluginManager::Dispatch_Message(0, F4SEMessagingInterface::kMessage_NewGame, (void*)this, sizeof(void*), NULL);
	return ret;
}

void Hooks_SaveLoad_Init()
{

}

void Hooks_SaveLoad_Commit()
{
	// hook save game
	{
		struct SaveGame_Code : Xbyak::CodeGenerator {
			SaveGame_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(rax, rsp);
				mov(ptr[rax+0x18], r8b);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(SaveGame.GetUIntPtr() + 7);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		SaveGame_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		SaveGame_Original = (_SaveGame)codeBuf;

		g_branchTrampoline.Write6Branch(SaveGame.GetUIntPtr(), (uintptr_t)SaveGame_Hook);
	}

	// hook load game
	{
		struct LoadGame_Code : Xbyak::CodeGenerator {
			LoadGame_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp+0x18], rbx);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(LoadGame.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		LoadGame_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		LoadGame_Original = (_LoadGame)codeBuf;

		g_branchTrampoline.Write5Branch(LoadGame.GetUIntPtr(), (uintptr_t)LoadGame_Hook);
	}

	// new game
	g_branchTrampoline.Write5Call(NewGame_Enter, GetFnAddr(&TESQuest::NewGame_Hook));

	// delete save game
	{
		struct DeleteSaveGame_Code : Xbyak::CodeGenerator {
			DeleteSaveGame_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp+0x08], rbx);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(DeleteSaveGame.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		DeleteSaveGame_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		DeleteSaveGame_Original = (_DeleteSaveGame)codeBuf;

		g_branchTrampoline.Write5Branch(DeleteSaveGame.GetUIntPtr(), (uintptr_t)DeleteSaveGame_Hook);
	}
}
