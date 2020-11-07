#include <map>
#include <string>

#include "common/IFileStream.h"

#include "f4se/PluginAPI.h"
#include "f4se/GameData.h"
#include "f4se/InternalSerialization.h"
#include "f4se/Serialization.h"
#include "f4se/PapyrusEvents.h"
#include "f4se/PapyrusObjects.h"
#include "f4se/PapyrusDelayFunctors.h"

#include "f4se/CustomMenu.h"

// Internal

static std::unordered_map<UInt32, UInt32> s_savedModIndexMap;

void LoadModList(const F4SESerializationInterface * intfc)
{
	_MESSAGE("Loading mod list:");

	char name[0x104] = { 0 };
	UInt16 nameLen = 0;

	UInt8 numSavedMods = 0;
	intfc->ReadRecordData(&numSavedMods, sizeof(numSavedMods));
	for (UInt32 i = 0; i < numSavedMods; i++)
	{
		intfc->ReadRecordData(&nameLen, sizeof(nameLen));
		intfc->ReadRecordData(&name, nameLen);
		name[nameLen] = 0;

		const ModInfo * modInfo = (*g_dataHandler)->LookupModByName(name);
		if (modInfo) {
			UInt32 newIndex = modInfo->GetPartialIndex();
			s_savedModIndexMap[i] = newIndex;
			_MESSAGE("\t(%d -> %d)\t%s", i, newIndex, &name);
		}
		else {
			s_savedModIndexMap[i] = 0xFF;
		}
	}
}

void LoadLightModList(const F4SESerializationInterface * intfc, bool fixedSize)
{
	_MESSAGE("Loading light mod list:");

	char name[0x104] = { 0 };
	UInt16 nameLen = 0;

	UInt32 modCount = 0;
	if(fixedSize)
	{
		UInt16 numMods = 0;
		intfc->ReadRecordData(&numMods, sizeof(numMods));
		modCount = numMods;
	}
	else
	{
		UInt8 numMods = 0;
		intfc->ReadRecordData(&numMods, sizeof(numMods));
		modCount = numMods;
	}

	for (UInt32 i = 0; i < modCount; i++)
	{
		intfc->ReadRecordData(&nameLen, sizeof(nameLen));
		intfc->ReadRecordData(&name, nameLen);
		name[nameLen] = 0;

		const ModInfo * modInfo = (*g_dataHandler)->LookupModByName(name);
		if (modInfo) {
			UInt32 newIndex = modInfo->GetPartialIndex();
			s_savedModIndexMap[0xFE000 | i] = newIndex;
			_MESSAGE("\t(%d -> %d)\t%s", i & 0xFF, newIndex & 0xFFF, &name);
		}
		else {
			s_savedModIndexMap[0xFE000 | i] = 0xFF;
		}
	}
}

void SavePluginsList(const F4SESerializationInterface * intfc)
{
	DataHandler * dhand = (*g_dataHandler);

	struct IsActiveFunctor
	{
		bool Accept(ModInfo * modInfo)
		{
			return modInfo && modInfo->IsActive();
		}
	};
	struct LoadedModVisitor
	{
		LoadedModVisitor(std::function<bool(ModInfo*)> func) : modInfoVisitor(func) { }
		bool Accept(ModInfo * modInfo)
		{
			return modInfoVisitor(modInfo);
		}
		std::function<bool(ModInfo*)> modInfoVisitor;
	};

	UInt16 modCount = dhand->modList.modInfoList.CountIf(IsActiveFunctor());

	intfc->OpenRecord('PLGN', 0);
	intfc->WriteRecordData(&modCount, sizeof(modCount));

	_MESSAGE("Saving plugin list:");


	dhand->modList.modInfoList.Visit(LoadedModVisitor([&](ModInfo* modInfo)
	{
		if (modInfo && modInfo->IsActive())
		{
			intfc->WriteRecordData(&modInfo->modIndex, sizeof(modInfo->modIndex));
			if (modInfo->modIndex == 0xFE) {
				intfc->WriteRecordData(&modInfo->lightIndex, sizeof(modInfo->lightIndex));
			}

			UInt16 nameLen = strlen(modInfo->name);
			intfc->WriteRecordData(&nameLen, sizeof(nameLen));
			intfc->WriteRecordData(modInfo->name, nameLen);
			if (modInfo->modIndex != 0xFE)
			{
				_MESSAGE("\t[%d]\t%s", modInfo->modIndex, &modInfo->name);
			}
			else
			{
				_MESSAGE("\t[FE:%d]\t%s", modInfo->lightIndex, &modInfo->name);
			}
		}
		return true; // Continue
	}));
}

void LoadPluginList(const F4SESerializationInterface * intfc)
{
	DataHandler * dhand = (*g_dataHandler);

	_MESSAGE("Loading plugin list:");

	char name[0x104] = { 0 };
	UInt16 nameLen = 0;

	UInt16 modCount = 0;
	intfc->ReadRecordData(&modCount, sizeof(modCount));
	for (UInt32 i = 0; i < modCount; i++)
	{
		UInt8 modIndex = 0xFF;
		UInt16 lightModIndex = 0xFFFF;
		intfc->ReadRecordData(&modIndex, sizeof(modIndex));
		if (modIndex == 0xFE) {
			intfc->ReadRecordData(&lightModIndex, sizeof(lightModIndex));
		}

		intfc->ReadRecordData(&nameLen, sizeof(nameLen));
		intfc->ReadRecordData(&name, nameLen);
		name[nameLen] = 0;

		UInt32 newIndex = 0xFF;
		UInt32 oldIndex = modIndex == 0xFE ? (0xFE000 | lightModIndex) : modIndex;

		const ModInfo * modInfo = dhand->LookupModByName(name);
		if (modInfo) {
			newIndex = modInfo->GetPartialIndex();
		}

		s_savedModIndexMap[oldIndex] = newIndex;

		_MESSAGE("\t(%d -> %d)\t%s", oldIndex, newIndex, name);
	}
}

UInt32 ResolveModIndex(UInt32 modIndex)
{
	auto it = s_savedModIndexMap.find(modIndex);
	if (it != s_savedModIndexMap.end())
	{
		return it->second;
	}

	return 0xFF;
}

//// Callbacks

void Core_RevertCallback(const F4SESerializationInterface * intfc)
{
	g_inputKeyEventRegs.Clear();
	g_inputControlEventRegs.Clear();
	g_externalEventRegs.Clear();
	g_cameraEventRegs.Clear();
	g_furnitureEventRegs.Clear();

	F4SEDelayFunctorManagerInstance().OnRevert();
	F4SEObjectStorageInstance().ClearAndRelease();

	// Unregister all custom menus
	g_customMenuLock.LockForWrite();
	for(auto & menuData : g_customMenuData)
	{
		BSFixedString menuName(menuData.first.c_str());
		if(!(*g_ui)->UnregisterMenu(menuName)) {
			_DMESSAGE("Failed to unregister %s, instance still exists", menuName.c_str());
		}
	}
	g_customMenuData.clear();
	g_customMenuLock.UnlockWrite();
}

void Core_SaveCallback(const F4SESerializationInterface * intfc)
{
	using namespace Serialization;

	SavePluginsList(intfc);

	_MESSAGE("Saving key input event registrations...");
	g_inputKeyEventRegs.Save(intfc, 'KEYR', InternalEventVersion::kCurrentVersion);

	_MESSAGE("Saving control input event registrations...");
	g_inputControlEventRegs.Save(intfc, 'CTLR', InternalEventVersion::kCurrentVersion);

	_MESSAGE("Saving external event registrations...");
	g_externalEventRegs.Save(intfc, 'EXEV', InternalEventVersion::kCurrentVersion);

	_MESSAGE("Saving camera event registrations...");
	g_cameraEventRegs.Save(intfc, 'CAMR', InternalEventVersion::kCurrentVersion);

	_MESSAGE("Saving furniture event registrations...");
	g_furnitureEventRegs.Save(intfc, 'FRNR', InternalEventVersion::kCurrentVersion);

	_MESSAGE("Saving SKSEPersistentObjectStorage data...");
	SaveClassHelper(intfc, 'OBMG', F4SEObjectStorageInstance());

	_MESSAGE("Saving SKSEDelayFunctorManager data...");
	SaveClassHelper(intfc, 'DFMG', F4SEDelayFunctorManagerInstance());
}

void Core_LoadCallback(const F4SESerializationInterface * intfc)
{
	UInt32 type, version, length;

	while (intfc->GetNextRecordInfo(&type, &version, &length))
	{
		switch (type)
		{
		// Plugins list
		case 'PLGN':
			LoadPluginList(intfc);
			break;

		// Mod list (DEPRECATED)
		case 'MODS':
			LoadModList(intfc);
			break;

		// Light Mod list (legacy) (DEPRECATED)
		case 'LMOD':
			LoadLightModList(intfc, false);
			break;

		// Light mod list (16-bit size) (DEPRECATED)
		case 'LIMD':
			LoadLightModList(intfc, true);
			break;

			// Key input events
		case 'KEYR':
			_MESSAGE("Loading key input event registrations...");
			g_inputKeyEventRegs.Load(intfc, InternalEventVersion::kCurrentVersion);
			break;

			// Control input events
		case 'CTLR':
			_MESSAGE("Loading control input event registrations...");
			g_inputControlEventRegs.Load(intfc, InternalEventVersion::kCurrentVersion);
			break;

			// External events
		case 'EXEV':
			_MESSAGE("Loading external event registrations...");
			g_externalEventRegs.Load(intfc, InternalEventVersion::kCurrentVersion);
			break;

			// Camera events
		case 'CAMR':
			_MESSAGE("Loading camera event registrations...");
			g_cameraEventRegs.Load(intfc, InternalEventVersion::kCurrentVersion);
			break;

			// Furniture events
		case 'FRNR':
			_MESSAGE("Loading furniture event registrations...");
			g_furnitureEventRegs.Load(intfc, InternalEventVersion::kCurrentVersion);
			break;

			// SKSEPersistentObjectStorage
		case 'OBMG':
			_MESSAGE("Loading F4SEPersistentObjectStorage data...");
			F4SEObjectStorageInstance().Load(intfc, version);
			break;

			// SKSEDelayFunctorManager
		case 'DFMG':
			_MESSAGE("Loading F4SEDelayFunctorManager data...");
			F4SEDelayFunctorManagerInstance().Load(intfc, version);
			break;

		default:
			_MESSAGE("Unhandled chunk type in Core_LoadCallback: %08X (%.4s)", type, &type);
			continue;
		}
	}
}

void Init_CoreSerialization_Callbacks()
{
	Serialization::SetUniqueID(0, 0);
	Serialization::SetRevertCallback(0, Core_RevertCallback);
	Serialization::SetSaveCallback(0, Core_SaveCallback);
	Serialization::SetLoadCallback(0, Core_LoadCallback);
}
