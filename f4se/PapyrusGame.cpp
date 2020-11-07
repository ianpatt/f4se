#include "f4se/PapyrusGame.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

#include "f4se/GameAPI.h"
#include "f4se/GameReferences.h"
#include "f4se/GameData.h"
#include "f4se/GameSettings.h"
#include "f4se/GameCamera.h"
#include "f4se/GameRTTI.h"


namespace papyrusGame
{
	DECLARE_STRUCT(PluginInfo, "Game")

	TESObjectREFR * GetCurrentConsoleRef(StaticFunctionTag * base)
	{
		UInt32 handle = (*g_consoleHandle);
		NiPointer<TESObjectREFR> refr;
		if(handle != 0 && handle != (*g_invalidRefHandle)) {
			LookupREFRByHandle(handle, refr);
			return refr;
		}

		return nullptr;
	}

	VMArray<PluginInfo> GetInstalledPlugins(StaticFunctionTag * base)
	{
		VMArray<PluginInfo> result;
		UInt8 modCount = (*g_dataHandler)->modList.loadedMods.count;
		for (UInt32 i = 0; i < modCount; i++)
		{
			ModInfo * modInfo = (*g_dataHandler)->modList.loadedMods[i];
			
			PluginInfo info;
			info.Set<UInt32>("index", modInfo->modIndex);
			info.Set<BSFixedString>("name", modInfo->name);
			info.Set<BSFixedString>("author", modInfo->author.Get());
			info.Set<BSFixedString>("description", modInfo->description.Get());
			result.Push(&info);
		}

		return result;
	}

	VMArray<PluginInfo> GetInstalledLightPlugins(StaticFunctionTag * base)
	{
		VMArray<PluginInfo> result;
		UInt32 modCount = (*g_dataHandler)->modList.lightMods.count;
		for (UInt32 i = 0; i < modCount; i++)
		{
			ModInfo * modInfo = (*g_dataHandler)->modList.lightMods[i];

			PluginInfo info;
			info.Set<UInt32>("index", modInfo->modIndex);
			info.Set<BSFixedString>("name", modInfo->name);
			info.Set<BSFixedString>("author", modInfo->author.Get());
			info.Set<BSFixedString>("description", modInfo->description.Get());
			result.Push(&info);
		}

		return result;
	}

	VMArray<BSFixedString> GetPluginDependencies(StaticFunctionTag * base, BSFixedString plugin)
	{
		VMArray<BSFixedString> result;
		const ModInfo* pluginInfo = (*g_dataHandler)->LookupLoadedModByName(plugin);
		if(!pluginInfo)
			pluginInfo = (*g_dataHandler)->LookupLoadedLightModByName(plugin);

		if(pluginInfo)
		{
			for(UInt32 i = 0; i < pluginInfo->numRefMods; i++)
			{
				ModInfo * modInfo = pluginInfo->refModInfo[i];
				if(modInfo) {
					BSFixedString modName(modInfo->name);
					result.Push(&modName);
				}
			}
		}

		return result;
	}

	void SetGameSettingFloat(StaticFunctionTag * base, BSFixedString name, float value)
	{
		Setting	* setting = GetGameSetting(name.c_str());
		if(setting)
		{
			if(!setting->SetDouble(value))
			{
				_WARNING("SetGameSettingFloat: %s is not a float", name.data);
			}
		}
		else
		{
			_WARNING("SetGameSettingFloat: %s not found", name.data);
		}
	}

	void SetGameSettingInt(StaticFunctionTag * base, BSFixedString name, UInt32 value)
	{
		Setting	* setting = GetGameSetting(name.c_str());
		if(setting)
		{
			if(setting->GetType() == Setting::kType_Integer)
			{
				setting->data.u32 = value;
			}
			else
			{
				_WARNING("SetGameSettingInt: %s is not an int", name.data);
			}	
		}
		else
		{
			_WARNING("SetGameSettingInt: %s not found", name.data);
		}
	}

	void SetGameSettingBool(StaticFunctionTag * base, BSFixedString name, bool value)
	{
		Setting	* setting = GetGameSetting(name.c_str());
		if(setting)
		{
			if(setting->GetType() == Setting::kType_Bool)
			{
				setting->data.u8 = value;
			}
			else
			{
				_WARNING("SetGameSettingBool: %s is not a bool", name.data);
			}
		}
		else
		{
			_WARNING("SetGameSettingBool: %s not found", name.data);
		}
	}

	void SetGameSettingString(StaticFunctionTag * base, BSFixedString name, BSFixedString value)
	{
		Setting	* setting = GetGameSetting(name.c_str());
		if(setting)
		{
			if(!setting->SetString(value.c_str()))
			{
				_WARNING("SetGameSettingString: %s is not a string", name.data);
			}
		}
		else
		{
			_WARNING("SetGameSettingString: %s not found", name.data);
		}
	}

	void UpdateThirdPerson(StaticFunctionTag * base)
	{
		PlayerCharacter * player = *g_player;
		PlayerCamera * playerCamera = *g_playerCamera;
		if(playerCamera && player)
		{
			ThirdPersonState * thirdPersonCamera = DYNAMIC_CAST(
				playerCamera->cameraStates[PlayerCamera::kCameraState_ThirdPerson2],
				TESCameraState, ThirdPersonState);
			if(thirdPersonCamera)
			{
				thirdPersonCamera->UpdateMode(player->actorState.IsWeaponDrawn());
			}
		}
	}

	SInt32 GetCameraState(StaticFunctionTag * base)
	{
		PlayerCamera * playerCamera = *g_playerCamera;
		if(playerCamera)
			return playerCamera->GetCameraStateId(playerCamera->cameraState);

		return -1;
	}
}

void papyrusGame::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, TESObjectREFR*>("GetCurrentConsoleRef", "Game", papyrusGame::GetCurrentConsoleRef, vm));

	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, VMArray<PluginInfo>>("GetInstalledPlugins", "Game", papyrusGame::GetInstalledPlugins, vm));

	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, VMArray<PluginInfo>>("GetInstalledLightPlugins", "Game", papyrusGame::GetInstalledLightPlugins, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, VMArray<BSFixedString>, BSFixedString>("GetPluginDependencies", "Game", papyrusGame::GetPluginDependencies, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, BSFixedString, float>("SetGameSettingFloat", "Game", papyrusGame::SetGameSettingFloat, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, BSFixedString, UInt32>("SetGameSettingInt", "Game", papyrusGame::SetGameSettingInt, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, BSFixedString, bool>("SetGameSettingBool", "Game", papyrusGame::SetGameSettingBool, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, BSFixedString, BSFixedString>("SetGameSettingString", "Game", papyrusGame::SetGameSettingString, vm));

	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, void>("UpdateThirdPerson", "Game", papyrusGame::UpdateThirdPerson, vm));

	vm->RegisterFunction(
		new NativeFunction0 <StaticFunctionTag, SInt32>("GetCameraState", "Game", papyrusGame::GetCameraState, vm));

	vm->SetFunctionFlags("Game", "GetCameraState", IFunction::kFunctionFlag_NoWait);
}
