#include "f4se/GameData.h"

// 
RelocPtr <DataHandler*> g_dataHandler(0x030CAD80);

// 
RelocPtr <bool> g_isGameDataReady(0x03251024);

// 
RelocPtr <DefaultObjectMap*> g_defaultObjectMap(0x030CFB50);

// 
RelocPtr <BSReadWriteLock> g_defaultObjectMapLock(0x030CFBA0);

// 
RelocPtr <FavoritesManager*> g_favoritesManager(0x032577B8);

class LoadedModFinder
{
	const char * m_stringToFind;

public:
	LoadedModFinder(const char * str) : m_stringToFind(str) { }

	bool Accept(ModInfo* modInfo)
	{
		return _stricmp(modInfo->name, m_stringToFind) == 0;
	}
};

const ModInfo * DataHandler::LookupModByName(const char * modName)
{
	return modList.modInfoList.Find(LoadedModFinder(modName));
}

UInt8 DataHandler::GetModIndex(const char* modName)
{
	return modList.modInfoList.GetIndexOf(LoadedModFinder(modName));
}

const ModInfo* DataHandler::LookupLoadedModByName(const char* modName)
{
	for(UInt32 i = 0; i < modList.loadedMods.count; i++) {
		ModInfo * modInfo = modList.loadedMods[i];
		if(_stricmp(modInfo->name, modName) == 0)
			return modInfo;
	}

	return nullptr;
}

const ModInfo* DataHandler::LookupLoadedLightModByName(const char* modName)
{
	for(UInt32 i = 0; i < modList.lightMods.count; i++) {
		ModInfo * modInfo = modList.lightMods[i];
		if(_stricmp(modInfo->name, modName) == 0)
			return modInfo;
	}

	return nullptr;
}

UInt8 DataHandler::GetLoadedModIndex(const char* modName)
{
	const ModInfo * modInfo = LookupLoadedModByName(modName);
	if(modInfo) {
		return modInfo->modIndex;
	}

	return -1;
}

UInt16 DataHandler::GetLoadedLightModIndex(const char* modName)
{
	for(UInt32 i = 0; i < modList.lightMods.count; i++) {
		ModInfo * modInfo = modList.lightMods[i];
		if(_stricmp(modInfo->name, modName) == 0)
			return i;
	}

	return -1;
}

BGSDefaultObject * DefaultObjectMap::GetDefaultObject(BSFixedString name)
{
	BSReadLocker locker(g_defaultObjectMapLock);
	if(*g_defaultObjectMap) {
		auto entry = (*g_defaultObjectMap)->Find(&name.data);
		if(entry) {
			return entry->defaultObject;
		}
	}
	return nullptr;
}
