#include "PluginManager.h"
#include "common/IDirectoryIterator.h"
#include "common/IFileStream.h"
#include "GameAPI.h"
#include "f4se_common/Utilities.h"
#include "f4se_common/f4se_version.h"
#include "f4se_common/BranchTrampoline.h"
#include "Hooks_Scaleform.h"
#include "Hooks_Papyrus.h"
#include "f4se/Serialization.h"
#include "Hooks_Threads.h"
#include "f4se/PapyrusDelayFunctors.h"
#include "f4se/PapyrusObjects.h"

#include <cassert>
#include <cstdint>
#include <functional>
#include <mutex>

namespace
{
	class ScopeExit
	{
	public:
		ScopeExit(std::function<void()> fn) :
			m_fn(std::move(fn))
		{
			assert(m_fn);
		}

		~ScopeExit()
		{
			if (m_fn)
			{
				m_fn();
			}
		}

	private:
		std::function<void()> m_fn;

		ScopeExit(const ScopeExit&);
		ScopeExit(ScopeExit&&);

		ScopeExit& operator=(const ScopeExit&);
		ScopeExit& operator=(ScopeExit&&);
	};

	std::string CheckModNotFound(const char* a_plugin)
	{
		assert(a_plugin != nullptr);

		const auto file = LoadLibraryExA(
			a_plugin,
			NULL,
			LOAD_LIBRARY_AS_DATAFILE);
		if (file == NULL)
		{
			return "failed to load resource with error code " + std::to_string(GetLastError());
		}
		ScopeExit _file([&]() { FreeLibrary(file); });

		const auto base = reinterpret_cast<const char*>(
			reinterpret_cast<std::uintptr_t>(file) & 
			~(static_cast<std::uintptr_t>(0xFFFF)));	// https://devblogs.microsoft.com/oldnewthing/20051006-09/?p=33883
		const auto dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
		const auto ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dosHeader->e_lfanew);
		const auto sections = IMAGE_FIRST_SECTION(ntHeader);
		const auto adjustRVA = [&](std::size_t rva) -> DWORD
		{
			for (WORD i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
			{
				const auto section = sections[i];
				if (section.VirtualAddress <= rva && rva < section.VirtualAddress + section.Misc.VirtualSize)
				{
					return rva - section.VirtualAddress + section.PointerToRawData;
				}
			}
			return 0;
		};

		const auto importDirectory = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (importDirectory.Size == 0)
		{
			return "no import entry to enumerate";
		}

		const auto imports = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + adjustRVA(importDirectory.VirtualAddress));
		for (auto iter = imports; iter->Characteristics != 0; ++iter)
		{
			const auto name = reinterpret_cast<const char*>(base + adjustRVA(iter->Name));
			if (name && name[0] != '\0')
			{
				const auto handle = LoadLibraryA(name);
				if (handle == NULL)
				{
					return std::string("failed to load ") + name;
				}
				else
				{
					FreeLibrary(handle);
				}
			}
		}

		return "";
	}

	void* AllocateFromPool(PluginAllocator& alloc, const char* name, PluginHandle plugin, size_t size)
	{
		assert(name != nullptr);

		const auto mem = alloc.Allocate(size);
		if (mem) {
			static std::mutex lock;
			const std::lock_guard<decltype(lock)> l(lock);	// global log isn't thread-safe
			_DMESSAGE("plugin %u allocated %u bytes from %s pool", plugin, size, name);
		}

		return mem;
	}

	void* AllocateFromBranchPool(PluginHandle plugin, size_t size)
	{
		return AllocateFromPool(g_branchPluginAllocator, "branch", plugin, size);
	}

	void* AllocateFromLocalPool(PluginHandle plugin, size_t size)
	{
		return AllocateFromPool(g_localPluginAllocator, "local", plugin, size);
	}
}

static const char *	GetSaveFolderName(void)
{
	return SAVE_FOLDER_NAME;
}

PluginManager	g_pluginManager;
PluginAllocator	g_branchPluginAllocator;
PluginAllocator	g_localPluginAllocator;

PluginManager::LoadedPlugin *	PluginManager::s_currentLoadingPlugin = NULL;
PluginHandle					PluginManager::s_currentPluginHandle = 0;

static const F4SEInterface g_F4SEInterface =
{
	PACKED_F4SE_VERSION,

	RUNTIME_VERSION,
	0,
	0,

	PluginManager::QueryInterface,
	PluginManager::GetPluginHandle,
	PluginManager::GetReleaseIndex,
	PluginManager::GetPluginInfo,
	GetSaveFolderName
};

static const F4SEMessagingInterface g_F4SEMessagingInterface =
{
	F4SEMessagingInterface::kInterfaceVersion,
	PluginManager::RegisterListener,
	PluginManager::Dispatch_Message,
};

static const F4SEScaleformInterface g_F4SEScaleformInterface =
{
	F4SEScaleformInterface::kInterfaceVersion,
	RegisterScaleformPlugin
};

static const F4SEPapyrusInterface g_F4SEPapyrusInterface =
{
	F4SEPapyrusInterface::kInterfaceVersion,
	RegisterPapyrusPlugin,
	GetExternalEventRegistrations
};

static const F4SESerializationInterface	g_F4SESerializationInterface =
{
	F4SESerializationInterface::kInterfaceVersion,

	Serialization::SetUniqueID,

	Serialization::SetRevertCallback,
	Serialization::SetSaveCallback,
	Serialization::SetLoadCallback,
	Serialization::SetFormDeleteCallback,

	Serialization::WriteRecord,
	Serialization::OpenRecord,
	Serialization::WriteRecordData,

	Serialization::GetNextRecordInfo,
	Serialization::ReadRecordData,
	Serialization::ResolveHandle,
	Serialization::ResolveFormId
};

static const F4SETaskInterface	g_F4SETaskInterface =
{
	F4SETaskInterface::kInterfaceVersion,

	TaskInterface::AddTask,
	TaskInterface::AddUITask,
	TaskInterface::AddTaskPermanent
};

static const F4SEObjectInterface g_F4SEObjectInterface =
{
	F4SEObjectInterface::kInterfaceVersion,
	F4SEDelayFunctorManagerInstance,
	F4SEObjectRegistryInstance,
	F4SEObjectStorageInstance
};

static const F4SETrampolineInterface g_F4SETrampolineInterface =
{
	F4SETrampolineInterface::kInterfaceVersion,
	AllocateFromBranchPool,
	AllocateFromLocalPool
};

PluginManager::PluginManager()
{
	//
}

PluginManager::~PluginManager()
{
	DeInit();
}

PluginManager::LoadedPlugin::LoadedPlugin()
{
	memset(&info, 0, sizeof(info));
	memset(&version, 0, sizeof(version));
}

void PluginManager::Init(void)
{
	bool	result = false;

	if(FindPluginDirectory())
	{
		_MESSAGE("plugin directory = %s", m_pluginDirectory.c_str());

		// avoid realloc
		m_plugins.reserve(5);

		__try
		{
			ScanPlugins();

			result = true;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			// something very bad happened
			_ERROR("exception occurred while loading plugins");
		}
	}
}

void PluginManager::InstallPlugins(UInt32 phase)
{
	for(size_t i = 0; i < m_plugins.size(); i++)
	{
		auto & plugin = m_plugins[i];

		// skip plugins that don't care about this phase
		if(phase == kPhase_Preload)
		{
			if(!plugin.hasPreload)
				continue;
		}
		else
		{
			if(!plugin.hasLoad)
				continue;
		}

		_MESSAGE("%sloading plugin \"%s\"", (phase == kPhase_Preload) ? "pre" : "", plugin.version.name);

		s_currentLoadingPlugin = &plugin;
		s_currentPluginHandle = plugin.internalHandle;

		std::string pluginPath = m_pluginDirectory + plugin.dllName;

		if(!plugin.handle)
		{
			plugin.handle = (HMODULE)LoadLibrary(pluginPath.c_str());
			if(!plugin.handle)
				LogPluginLoadError(plugin, "couldn't load plugin", GetLastError());
		}

		bool	success = false;

		if(plugin.handle)
		{
			plugin.load[phase] = (_F4SEPlugin_Load)GetProcAddress(plugin.handle, (phase == kPhase_Preload) ? "F4SEPlugin_Preload" : "F4SEPlugin_Load");
			if(plugin.load)
			{
				const char * loadStatus = nullptr;

				loadStatus = SafeCallLoadPlugin(&plugin, &g_F4SEInterface, phase);

				if(!loadStatus)
				{
					success = true;
				}
				else
				{
					LogPluginLoadError(plugin, loadStatus);
				}
			}
			else
			{
				LogPluginLoadError(plugin, "does not appear to be an F4SE plugin");
			}
		}

		if(!success)
		{
			// failed, unload the library
			if(plugin.handle) FreeLibrary(plugin.handle);

			// and remove from plugins list
			m_plugins.erase(m_plugins.begin() + i);

			// fix iterator
			i--;
		}

	}

	s_currentLoadingPlugin = nullptr;
	s_currentPluginHandle = 0;
}

void PluginManager::LoadComplete()
{
	for(size_t i = 0; i < m_plugins.size(); i++)
	{
		auto & plugin = m_plugins[i];

		_MESSAGE("plugin %s (%08X %s %08X) %s (handle %d)",
			plugin.dllName.c_str(),
			plugin.version.dataVersion,
			plugin.version.name,
			plugin.version.pluginVersion,
			"loaded correctly",
			plugin.internalHandle);
	}

	ReportPluginErrors();

	// make fake PluginInfo structs after m_plugins is locked
	for(auto & plugin : m_plugins)
	{
		plugin.info.infoVersion = PluginInfo::kInfoVersion;
		plugin.info.name = plugin.version.name;
		plugin.info.version = plugin.version.pluginVersion;
	}

	// alert any listeners that plugin load has finished
	Dispatch_Message(0, F4SEMessagingInterface::kMessage_PostLoad, NULL, 0, NULL);
	// second post-load dispatch
	Dispatch_Message(0, F4SEMessagingInterface::kMessage_PostPostLoad, NULL, 0, NULL);
}

void PluginManager::DeInit(void)
{
	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);

		if(plugin->handle)
		{
			FreeLibrary(plugin->handle);
		}
	}

	m_plugins.clear();
}

UInt32 PluginManager::GetNumPlugins(void)
{
	UInt32	numPlugins = m_plugins.size();

	// is one currently loading?
	if(s_currentLoadingPlugin) numPlugins++;

	return numPlugins;
}

PluginInfo * PluginManager::GetInfoByName(const char * name)
{
	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);

		if(plugin->info.name && !_stricmp(name, plugin->info.name))
			return &plugin->info;
	}

	return NULL;
}

void * PluginManager::QueryInterface(UInt32 id)
{
	void	* result = NULL;

#ifdef RUNTIME
	switch(id)
	{
	case kInterface_Messaging:
		result = (void *)&g_F4SEMessagingInterface;
		break;
	case kInterface_Scaleform:
		result = (void *)&g_F4SEScaleformInterface;
		break;
	case kInterface_Papyrus:
		result = (void *)&g_F4SEPapyrusInterface;
		break;
	case kInterface_Serialization:
		result = (void *)&g_F4SESerializationInterface;
		break;
	case kInterface_Task:
		result = (void *)&g_F4SETaskInterface;
		break;
	case kInterface_Object:
		result = (void *)&g_F4SEObjectInterface;
		break;
	case kInterface_Trampoline:
		result = (void *)&g_F4SETrampolineInterface;
		break;
	default:
		_WARNING("unknown QueryInterface %08X", id);
		break;
	}
#else
	_WARNING("unknown QueryInterface %08X", id);
#endif

	return result;
}

PluginHandle PluginManager::GetPluginHandle(void)
{
	ASSERT_STR(s_currentPluginHandle, "A plugin has called F4SEInterface::GetPluginHandle outside of its Query/Load handlers");

	return s_currentPluginHandle;
}

UInt32 PluginManager::GetReleaseIndex( void )
{
	return F4SE_VERSION_RELEASEIDX;
}

const PluginInfo* PluginManager::GetPluginInfo(const char* name)
{
	return g_pluginManager.GetInfoByName(name);
}

bool PluginManager::FindPluginDirectory(void)
{
	bool	result = false;

	// find the path <runtime directory>/data/f4se/
	std::string	runtimeDirectory = GetRuntimeDirectory();

	if(!runtimeDirectory.empty())
	{
		m_pluginDirectory = runtimeDirectory + "Data\\F4SE\\Plugins\\";
		result = true;
	}

	return result;
}

void PluginManager::ScanPlugins(void)
{
	_MESSAGE("scanning plugin directory %s", m_pluginDirectory.c_str());

	UInt32 handleIdx = 1;	// start at 1, 0 is reserved for internal use

	for(IDirectoryIterator iter(m_pluginDirectory.c_str(), "*.dll"); !iter.Done(); iter.Next())
	{
		std::string	pluginPath = iter.GetFullPath();

		LoadedPlugin	plugin;
		plugin.dllName = iter.Get()->cFileName;

		_MESSAGE("checking plugin %s", plugin.dllName.c_str());

		HMODULE resourceHandle = (HMODULE)LoadLibraryEx(pluginPath.c_str(), nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
		if(resourceHandle)
		{
			if(Is64BitDLL(resourceHandle))
			{
				auto * version = (const F4SEPluginVersionData *)GetResourceLibraryProcAddress(resourceHandle, "F4SEPlugin_Version");
				if(version)
				{
					plugin.version = *version;
					Sanitize(&plugin.version);

					auto * loadStatus = CheckPluginCompatibility(plugin.version);
					if(!loadStatus)
					{
						// compatible, add to list

						plugin.internalHandle = handleIdx;
						handleIdx++;

						plugin.hasLoad = GetResourceLibraryProcAddress(resourceHandle, "F4SEPlugin_Load") != nullptr;
						plugin.hasPreload = GetResourceLibraryProcAddress(resourceHandle, "F4SEPlugin_Preload") != nullptr;

						m_plugins.push_back(plugin);
					}
					else
					{
						LogPluginLoadError(plugin, loadStatus);
					}
				}
				else
				{
					LogPluginLoadError(plugin, "no version data", 0, false);
				}
			}
			else
			{
				LogPluginLoadError(plugin, "32-bit plugins can never work");
			}

			FreeLibrary(resourceHandle);
		}
		else
		{
			LogPluginLoadError(plugin, "couldn't load plugin", GetLastError());
		}
	}
}

const char * PluginManager::CheckAddressLibrary(void)
{
	static bool s_checked = false;
	static const char * s_status = nullptr;

	if(s_checked)
	{
		return s_status;
	}

	char fileName[256];
	_snprintf_s(fileName, sizeof(fileName), "Data\\F4SE\\Plugins\\version-%d-%d-%d-%d.bin",
		GET_EXE_VERSION_MAJOR(RUNTIME_VERSION),
		GET_EXE_VERSION_MINOR(RUNTIME_VERSION),
		GET_EXE_VERSION_BUILD(RUNTIME_VERSION),
		0);

	IFileStream versionLib;
	if(!versionLib.Open(fileName))
	{
		m_oldAddressLibrary = true;
		s_status = "disabled, address library needs to be updated";
	}

	s_checked = true;

	return s_status;
}

const char * PluginManager::SafeCallLoadPlugin(LoadedPlugin * plugin, const F4SEInterface * f4se, UInt32 phase)
{
	__try
	{
		if(!plugin->load[phase](f4se))
		{
			return "reported as incompatible during load";
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// something very bad happened
		return "disabled, fatal error occurred while loading plugin";
	}

	return NULL;
}

void PluginManager::Sanitize(F4SEPluginVersionData * version)
{
	version->name[sizeof(version->name) - 1] = 0;
	version->author[sizeof(version->author) - 1] = 0;
}

enum
{
	kCompat_BlockFromRuntime =			1 << 0,
	kCompat_BlockFromEditor =			1 << 1,
};

struct MinVersionEntry
{
	const char	* name;
	UInt32		minVersion;
	const char	* reason;
	UInt32		compatFlags;
};

static const MinVersionEntry	kMinVersionList[] =
{
	{	NULL, 0, NULL }
};

const char * PluginManager::CheckPluginCompatibility(const F4SEPluginVersionData & version)
{
	__try
	{
		// basic validity
		if(!version.dataVersion)
		{
			return "disabled, bad version data";
		}

		if(!version.name[0])
		{
			return "disabled, no name specified";
		}

		// check for 'known bad' versions of plugins
		for(const MinVersionEntry * iter = kMinVersionList; iter->name; ++iter)
		{
			if(!strcmp(iter->name, version.name))
			{
				if(version.pluginVersion < iter->minVersion)
				{
#ifdef RUNTIME
					if(iter->compatFlags & kCompat_BlockFromRuntime)
					{
						return iter->reason;
					}
#endif

#ifdef EDITOR
					if(iter->compatFlags & kCompat_BlockFromEditor)
					{
						return iter->reason;
					}
#endif
				}

				break;
			}
		}

		// version compatibility
		const UInt32 kCurrentAddressLibrary = F4SEPluginVersionData::kAddressIndependence_AddressLibrary_1_11_137;

		bool hasAddressIndependence = version.addressIndependence &
			(F4SEPluginVersionData::kAddressIndependence_Signatures |
			kCurrentAddressLibrary);
		bool hasStructureIndependence = version.structureIndependence &
			(F4SEPluginVersionData::kStructureIndependence_NoStructs |
			F4SEPluginVersionData::kStructureIndependence_1_10_980Layout);
		
		bool versionIndependent = hasAddressIndependence && hasStructureIndependence;

		// currently anything in the "breaking change" field means that compatibility has been broken by an update
		if(version.reservedBreaking)
			versionIndependent = false;

		// verify the address library is there to centralize error message
		if(version.addressIndependence & kCurrentAddressLibrary)
		{
			const char * result = CheckAddressLibrary();
			if(result) return result;
		}
		
		// simple version list
		if(!versionIndependent)
		{
			bool found = false;

			for(UInt32 i = 0; i < _countof(version.compatibleVersions); i++)
			{
				UInt32 compatibleVersion = version.compatibleVersions[i];

				if(compatibleVersion == RUNTIME_VERSION)
				{
					found = true;
					break;
				}
				else if(!compatibleVersion)
				{
					break;
				}
			}

			if(!found)
			{
				return "disabled, incompatible with current version of the game";
			}
		}

		// SE version compatibility
		if(version.seVersionRequired > PACKED_F4SE_VERSION)
		{
			return "disabled, requires newer script extender";
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// paranoia
		return "disabled, fatal error occurred while checking plugin compatibility";
	}

	return nullptr;
}

void PluginManager::LogPluginLoadError(const LoadedPlugin & pluginSrc, const char * errStr, UInt32 errCode, bool isError)
{
	LoadedPlugin plugin = pluginSrc;

	plugin.errorState = errStr;
	plugin.errorCode = errCode;

	if(isError)
		m_erroredPlugins.push_back(plugin);

	_MESSAGE("plugin %s (%08X %s %08X) %s %d (handle %d)",
		plugin.dllName.c_str(),
		plugin.version.dataVersion,
		plugin.version.name,
		plugin.version.pluginVersion,
		plugin.errorState,
		plugin.errorCode,
		s_currentPluginHandle);
}

struct BetterPluginName
{
	const char * dllName;
	const char * userReportedName;
};

// some plugins have non-descriptive names resulting in bad bug reports
static const BetterPluginName kBetterPluginNames[] =
{
	{ nullptr, nullptr }
};

void PluginManager::ReportPluginErrors()
{
	if(m_erroredPlugins.empty())
		return;

	if(m_oldAddressLibrary)
		UpdateAddressLibraryPrompt();

	// Fallout doesn't have a line that comes to mind here.

	std::string message = "A mod you have installed contains a DLL plugin that has failed to load correctly. If a new version of Fallout was just released, the plugin needs to be updated. Please check the mod's webpage for updates. This is not a problem with F4SE.\n";

	for(auto & plugin : m_erroredPlugins)
	{
		message += "\n";

		bool foundReplacementName = false;
		for(auto * iter = kBetterPluginNames; iter->dllName; ++iter)
		{
			if(!_stricmp(iter->dllName, plugin.dllName.c_str()))
			{
				foundReplacementName = true;

				message += iter->userReportedName;
				message += " (" + plugin.dllName + ")";
			}
		}
		if(!foundReplacementName)
			message += plugin.dllName;

		message += ": ";
		message += plugin.errorState;

		if(plugin.errorCode)
		{
			char codeStr[128];
			sprintf_s(codeStr, "%08X", plugin.errorCode);

			message += " (";
			message += codeStr;
			message += ")";
		}
	}

	message += "\n\nContinuing to load may result in lost save data or other undesired behavior.";
	message += "\nExit game? (yes highly suggested)";

	int result = MessageBox(0, message.c_str(), "F4SE Plugin Loader (" __PREPRO_TOKEN_STR__(F4SE_VERSION_INTEGER) "."
		__PREPRO_TOKEN_STR__(F4SE_VERSION_INTEGER_MINOR) "."
		__PREPRO_TOKEN_STR__(F4SE_VERSION_INTEGER_BETA) ")",
		MB_YESNO);

	if(result == IDYES)
	{
		TerminateProcess(GetCurrentProcess(), 0);
	}
}

void PluginManager::UpdateAddressLibraryPrompt()
{
	int result = MessageBox(0,
		"DLL plugins you have installed require a new version of the Address Library. Either this is a new install, or Fallout was just updated. Visit the Address Library webpage for updates?",
		"F4SE Plugin Loader", MB_YESNO);

	if(result == IDYES)
	{
		ShellExecute(0, nullptr, "https://www.nexusmods.com/fallout4/mods/47327", nullptr, nullptr, 0);
		TerminateProcess(GetCurrentProcess(), 0);
	}
}

// Plugin communication interface
struct PluginListener {
	PluginHandle	listener;
	F4SEMessagingInterface::EventCallback	handleMessage;
};

typedef std::vector<std::vector<PluginListener> > PluginListeners;
static PluginListeners s_pluginListeners;

bool PluginManager::RegisterListener(PluginHandle listener, const char* sender, F4SEMessagingInterface::EventCallback handler)
{
	// because this can be called while plugins are loading, gotta make sure number of plugins hasn't increased
	UInt32 numPlugins = g_pluginManager.GetNumPlugins() + 1;
	if (s_pluginListeners.size() < numPlugins)
	{
		s_pluginListeners.resize(numPlugins + 5);	// add some extra room to avoid unnecessary re-alloc
	}

	_MESSAGE("registering plugin listener for %s at %u of %u", sender, listener, numPlugins);

	// handle > num plugins = invalid
	if (listener > g_pluginManager.GetNumPlugins() || !handler) 
	{
		return false;
	}

	if (sender)
	{
		// is target loaded?
		PluginHandle target = g_pluginManager.LookupHandleFromName(sender);
		if (target == kPluginHandle_Invalid)
		{
			return false;
		}
		// is listener already registered?
		for (std::vector<PluginListener>::iterator iter = s_pluginListeners[target].begin(); iter != s_pluginListeners[target].end(); ++iter)
		{
			if (iter->listener == listener)
			{
				return true;
			}
		}

		// register new listener
		PluginListener newListener;
		newListener.handleMessage = handler;
		newListener.listener = listener;

		s_pluginListeners[target].push_back(newListener);
	}
	else
	{
		// register listener to every loaded plugin
		UInt32 idx = 0;
		for(PluginListeners::iterator iter = s_pluginListeners.begin(); iter != s_pluginListeners.end(); ++iter)
		{
			// don't add the listener to its own list
			if (idx && idx != listener)
			{
				bool skipCurrentList = false;
				for (std::vector<PluginListener>::iterator iterEx = iter->begin(); iterEx != iter->end(); ++iterEx)
				{
					// already registered with this plugin, skip it
					if (iterEx->listener == listener)
					{
						skipCurrentList = true;
						break;
					}
				}
				if (skipCurrentList)
				{
					continue;
				}
				PluginListener newListener;
				newListener.handleMessage = handler;
				newListener.listener = listener;

				iter->push_back(newListener);
			}
			idx++;
		}
	}

	return true;
}

bool PluginManager::Dispatch_Message(PluginHandle sender, UInt32 messageType, void * data, UInt32 dataLen, const char* receiver)
{
#ifdef _DEBUG
	_MESSAGE("dispatch message (%d) to plugin listeners", messageType);
#endif
	UInt32 numRespondents = 0;
	PluginHandle target = kPluginHandle_Invalid;

	if (!s_pluginListeners.size())	// no listeners yet registered
	{
#ifdef _DEBUG
		_MESSAGE("no listeners registered");
#endif
		return false;
	}
	else if (sender >= s_pluginListeners.size())
	{
#ifdef _DEBUG
		_MESSAGE("sender is not in the list");
#endif
		return false;
	}

	if (receiver)
	{
		target = g_pluginManager.LookupHandleFromName(receiver);
		if (target == kPluginHandle_Invalid)
			return false;
	}

	const char* senderName = g_pluginManager.GetPluginNameFromHandle(sender);
	if (!senderName)
		return false;
	for (std::vector<PluginListener>::iterator iter = s_pluginListeners[sender].begin(); iter != s_pluginListeners[sender].end(); ++iter)
	{
		F4SEMessagingInterface::Message msg;
		msg.data = data;
		msg.type = messageType;
		msg.sender = senderName;
		msg.dataLen = dataLen;

		if (target != kPluginHandle_Invalid)	// sending message to specific plugin
		{
			if (iter->listener == target)
			{
				iter->handleMessage(&msg);
				return true;
			}
		}
		else
		{
#ifdef _DEBUG
			_DMESSAGE("sending message type %u to plugin %u", messageType, iter->listener);
#endif
			iter->handleMessage(&msg);
			numRespondents++;
		}
	}
#ifdef _DEBUG
	_DMESSAGE("dispatched message.");
#endif
	return numRespondents ? true : false;
}

const char * PluginManager::GetPluginNameFromHandle(PluginHandle handle)
{
	if (handle > 0 && handle <= m_plugins.size())
		return (m_plugins[handle - 1].info.name);
	else if (handle == 0)
		return "F4SE";

	return NULL;
}

PluginHandle PluginManager::LookupHandleFromName(const char* pluginName)
{
	if (!_stricmp("F4SE", pluginName))
		return 0;

	UInt32	idx = 1;
	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);
		if(!_stricmp(plugin->info.name, pluginName))
		{
			return idx;
		}
		idx++;
	}
	return kPluginHandle_Invalid;
}
