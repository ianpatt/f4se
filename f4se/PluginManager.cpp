#include "PluginManager.h"
#include "common/IDirectoryIterator.h"
#include "GameAPI.h"
#include "f4se_common/Utilities.h"
#include "f4se_common/f4se_version.h"

#include <cassert>
#include <cstdint>
#include <functional>

namespace
{
	class ScopeExit
	{
	public:
		ScopeExit(std::function<void()> fn) noexcept :
			m_fn(std::move(fn))
		{
			assert(m_fn);
		}

		ScopeExit(const ScopeExit&) = delete;
		ScopeExit(ScopeExit&&) = delete;

		~ScopeExit()
		{
			if (m_fn)
			{
				m_fn();
			}
		}

		ScopeExit& operator=(const ScopeExit&) = delete;
		ScopeExit& operator=(ScopeExit&&) = delete;

	private:
		std::function<void()> m_fn;
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
		const auto ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base +  dosHeader->e_lfanew);
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
}

PluginManager	g_pluginManager;

PluginManager::LoadedPlugin *	PluginManager::s_currentLoadingPlugin = NULL;
PluginHandle					PluginManager::s_currentPluginHandle = 0;

static const F4SEInterface g_F4SEInterface =
{
	PACKED_F4SE_VERSION,

#ifdef RUNTIME
	RUNTIME_VERSION,
	0,
	0,
#else
	0,
	0,
	1,
#endif

	PluginManager::QueryInterface,
	PluginManager::GetPluginHandle,
	PluginManager::GetReleaseIndex
};

static const F4SEMessagingInterface g_F4SEMessagingInterface =
{
	F4SEMessagingInterface::kInterfaceVersion,
	PluginManager::RegisterListener,
	PluginManager::Dispatch_Message,
};

#include "Hooks_Scaleform.h"

static const F4SEScaleformInterface g_F4SEScaleformInterface =
{
	F4SEScaleformInterface::kInterfaceVersion,
	RegisterScaleformPlugin
};

#include "Hooks_Papyrus.h"

static const F4SEPapyrusInterface g_F4SEPapyrusInterface =
{
	F4SEPapyrusInterface::kInterfaceVersion,
	RegisterPapyrusPlugin,
	GetExternalEventRegistrations
};

#include "f4se/Serialization.h"

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

#include "Hooks_Threads.h"

static const F4SETaskInterface	g_F4SETaskInterface =
{
	F4SETaskInterface::kInterfaceVersion,

	TaskInterface::AddTask,
	TaskInterface::AddUITask
};

#include "f4se/PapyrusDelayFunctors.h"
#include "f4se/PapyrusObjects.h"

static const F4SEObjectInterface g_F4SEObjectInterface =
{
	F4SEObjectInterface::kInterfaceVersion,
	F4SEDelayFunctorManagerInstance,
	F4SEObjectRegistryInstance,
	F4SEObjectStorageInstance
};

PluginManager::PluginManager()
{
	//
}

PluginManager::~PluginManager()
{
	DeInit();
}

bool PluginManager::Init(void)
{
	bool	result = false;

	if(FindPluginDirectory())
	{
		_MESSAGE("plugin directory = %s", m_pluginDirectory.c_str());

		__try
		{
			InstallPlugins();

			result = true;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			// something very bad happened
			_ERROR("exception occurred while loading plugins");
		}
	}

	return result;
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

void PluginManager::InstallPlugins(void)
{
	// avoid realloc
	m_plugins.reserve(5);

	for(IDirectoryIterator iter(m_pluginDirectory.c_str(), "*.dll"); !iter.Done(); iter.Next())
	{
		std::string	pluginPath = iter.GetFullPath();

		_MESSAGE("checking plugin %s", pluginPath.c_str());

		LoadedPlugin	plugin;
		memset(&plugin, 0, sizeof(plugin));

		s_currentLoadingPlugin = &plugin;
		s_currentPluginHandle = m_plugins.size() + 1;	// +1 because 0 is reserved for internal use

		plugin.handle = LoadLibraryA(pluginPath.c_str());
		if(plugin.handle)
		{
			bool		success = false;

			plugin.query = (_F4SEPlugin_Query)GetProcAddress(plugin.handle, "F4SEPlugin_Query");
			plugin.load = (_F4SEPlugin_Load)GetProcAddress(plugin.handle, "F4SEPlugin_Load");

			if(plugin.query && plugin.load)
			{
				const char	* loadStatus = NULL;

				loadStatus = SafeCallQueryPlugin(&plugin, &g_F4SEInterface);

				if(!loadStatus)
				{
					loadStatus = CheckPluginCompatibility(&plugin);

					if(!loadStatus)
					{
						loadStatus = SafeCallLoadPlugin(&plugin, &g_F4SEInterface);

						if(!loadStatus)
						{
							loadStatus = "loaded correctly";
							success = true;
						}
					}
				}
				else
				{
					loadStatus = "reported as incompatible during query";
				}

				ASSERT(loadStatus);

				_MESSAGE("plugin %s (%08X %s %08X) %s",
					pluginPath.c_str(),
					plugin.info.infoVersion,
					plugin.info.name ? plugin.info.name : "<NULL>",
					plugin.info.version,
					loadStatus);
			}
			else
			{
				_MESSAGE("plugin %s does not appear to be an F4SE plugin", pluginPath.c_str());
			}

			if(success)
			{
				// succeeded, add it to the list
				m_plugins.push_back(plugin);
			}
			else
			{
				// failed, unload the library
				FreeLibrary(plugin.handle);
			}
		}
		else
		{
			std::string post;
			const auto err = GetLastError();
			switch (err) {
			case ERROR_MOD_NOT_FOUND:
				post = CheckModNotFound(pluginPath.c_str());
				break;
			}

			if (!post.empty())
			{
				post = ": " + post;
			}

			_ERROR("couldn't load plugin %s (Error %d%s)",
				   pluginPath.c_str(),
				   err,
				   post.c_str());
		}
	}

	s_currentLoadingPlugin = NULL;
	s_currentPluginHandle = 0;

	// alert any listeners that plugin load has finished
	Dispatch_Message(0, F4SEMessagingInterface::kMessage_PostLoad, NULL, 0, NULL);
	// second post-load dispatch
	Dispatch_Message(0, F4SEMessagingInterface::kMessage_PostPostLoad, NULL, 0, NULL);
}

// SEH-wrapped calls to plugin API functions to avoid bugs from bringing down the core
const char * PluginManager::SafeCallQueryPlugin(LoadedPlugin * plugin, const F4SEInterface * f4se)
{
	__try
	{
		if(!plugin->query(f4se, &plugin->info))
		{
			return "reported as incompatible during query";
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// something very bad happened
		return "disabled, fatal error occurred while querying plugin";
	}

	return NULL;
}

const char * PluginManager::SafeCallLoadPlugin(LoadedPlugin * plugin, const F4SEInterface * f4se)
{
	__try
	{
		if(!plugin->load(f4se))
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

enum
{
	kCompat_BlockFromRuntime =	1 << 0,
	kCompat_BlockFromEditor =	1 << 1,
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

// see if we have a plugin that we know causes problems
const char * PluginManager::CheckPluginCompatibility(LoadedPlugin * plugin)
{
	__try
	{
		// stupid plugin check
		if(!plugin->info.name)
		{
			return "disabled, no name specified";
		}

		// check for 'known bad' versions of plugins
		for(const MinVersionEntry * iter = kMinVersionList; iter->name; ++iter)
		{
			if(!strcmp(iter->name, plugin->info.name))
			{
				if(plugin->info.version < iter->minVersion)
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
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// paranoia
		return "disabled, fatal error occurred while checking plugin compatibility";
	}

	return NULL;
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
