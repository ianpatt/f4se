#pragma once

#include <string>
#include <vector>

#include "f4se/PluginAPI.h"

class PluginManager
{
public:
	PluginManager();
	~PluginManager();

	bool	Init(void);
	void	DeInit(void);

	PluginInfo *	GetInfoByName(const char * name);
	const char *	GetPluginNameFromHandle(PluginHandle handle);

	PluginHandle	LookupHandleFromName(const char* pluginName);


	UInt32			GetNumPlugins(void);

	static void *		QueryInterface(UInt32 id);
	static PluginHandle	GetPluginHandle(void);
	static UInt32		GetReleaseIndex(void);

	static bool Dispatch_Message(PluginHandle sender, UInt32 messageType, void * data, UInt32 dataLen, const char* receiver);
	static bool	RegisterListener(PluginHandle listener, const char* sender, F4SEMessagingInterface::EventCallback handler);

private:
	struct LoadedPlugin
	{
		// internals
		HMODULE		handle;
		PluginInfo	info;

		_F4SEPlugin_Query	query;
		_F4SEPlugin_Load	load;
	};

	bool	FindPluginDirectory(void);
	void	InstallPlugins(void);

	const char *	SafeCallQueryPlugin(LoadedPlugin * plugin, const F4SEInterface * f4se);
	const char *	SafeCallLoadPlugin(LoadedPlugin * plugin, const F4SEInterface * f4se);

	const char *	CheckPluginCompatibility(LoadedPlugin * plugin);

	typedef std::vector <LoadedPlugin>	LoadedPluginList;

	std::string			m_pluginDirectory;
	LoadedPluginList	m_plugins;

	static LoadedPlugin		* s_currentLoadingPlugin;
	static PluginHandle		s_currentPluginHandle;
};

extern PluginManager	g_pluginManager;

extern const F4SESerializationInterface	g_F4SESerializationInterface;
extern const F4SEPapyrusInterface		g_F4SEPapyrusInterface;
extern const F4SEScaleformInterface		g_F4SEScaleformInterface;
extern const F4SEMessagingInterface		g_F4SEMessagingInterface;
extern const F4SETaskInterface			g_F4SETaskInterface;
