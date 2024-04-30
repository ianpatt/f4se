#pragma once

typedef UInt32	PluginHandle;	// treat this as an opaque type

class GFxMovieView;
class GFxValue;
class ITaskDelegate;
class F4SEDelayFunctorManager;
class F4SEObjectRegistry;
class F4SEPersistentObjectStorage;
struct PluginInfo;

enum
{
	kPluginHandle_Invalid = 0xFFFFFFFF
};

enum
{
	kInterface_Invalid = 0,
	kInterface_Messaging,
	kInterface_Scaleform,
	kInterface_Papyrus,
	kInterface_Serialization,
	kInterface_Task,
	kInterface_Object,
	kInterface_Trampoline,
	kInterface_Max,
};

struct F4SEInterface
{
	UInt32	f4seVersion;
	UInt32	runtimeVersion;
	UInt32	editorVersion;
	UInt32	isEditor;
	void *	(* QueryInterface)(UInt32 id);

	// call during your Query or Load functions to get a PluginHandle uniquely identifying your plugin
	// invalid if called at any other time, so call it once and save the result
	PluginHandle	(* GetPluginHandle)(void);
	
	// returns the F4SE build's release index
	UInt32			(* GetReleaseIndex)(void);

	// Minimum F4SE version 0.6.22
	// returns the plugin info structure for a plugin by name, only valid to be called after PostLoad message
	const PluginInfo*	(*GetPluginInfo)(const char* name);
};


/**** Messaging API docs ********************************************************************
 *
 *	Messaging API allows inter-plugin communication at run-time. A plugin may register
 *	one callback for each plugin from which it wishes to receive messages, specifying
 *	the sender by name in the call to RegisterListener(). RegisterListener returns false
 *	if the specified plugin is not loaded, true otherwise. Any messages dispatched by
 *	the specified plugin will then be forwarded to the listener as they occur. Passing NULL as 
 *	the sender registers the calling plugin as a listener to every loaded plugin.
 *
 *	Messages may be dispatched via Dispatch() to either a specific listener (specified
 *	by name) or to all listeners (with NULL passed as the receiver). The contents and format of
 *	messageData are left up to the sender, and the receiver is responsible for casting the message
 *	to the expected type. If no plugins are registered as listeners for the sender, 
 *	Dispatch() returns false, otherwise it returns true.
 *
 *	Calling RegisterListener() or Dispatch() during plugin load is not advised as the requested plugin
 *	may not yet be loaded at that point. Instead, if you wish to register as a listener or dispatch a
 *	message immediately after plugin load, use RegisterListener() during load to register to receive
 *	messages from F4SE (sender name: "F4SE"). You will then receive a message from F4SE once 
 *	all plugins have been loaded, at which point it is safe to establish communications between
 *	plugins.
 *
 *	Some plugin authors may wish to use strings instead of integers to denote message type. In
 *	that case the receiver can pass the address of the string as an integer and require the receiver
 *	to cast it back to a char* on receipt.
 *
 *********************************************************************************************/

struct F4SEMessagingInterface
{
	struct Message {
		const char	* sender;
		UInt32		type;
		UInt32		dataLen;
		void		* data;
	};

	typedef void (* EventCallback)(Message* msg);

	enum {
		kInterfaceVersion = 1
	};

	// F4SE messages
	enum {
		kMessage_PostLoad,		// sent to registered plugins once all plugins have been loaded (no data)
		kMessage_PostPostLoad,	// sent right after kMessage_PostLoad to facilitate the correct dispatching/registering of messages/listeners
		kMessage_PreLoadGame,	// dispatched immediately before savegame is read by Fallout
								// dataLen: length of file path, data: char* file path of .ess savegame file
		kMessage_PostLoadGame,	//dispatched after an attempt to load a saved game has finished (the game's LoadGame() routine
								//has returned). You will probably want to handle this event if your plugin uses a Preload callback
								//as there is a chance that after that callback is invoked the game will encounter an error
								//while loading the saved game (eg. corrupted save) which may require you to reset some of your
								//plugin state.
								//data: bool, true if game successfully loaded, false otherwise
								// plugins may register as listeners during the first callback while deferring dispatches until the next
		kMessage_PreSaveGame,	// right before the game is saved
		kMessage_PostSaveGame,	// right after the game is saved
		kMessage_DeleteGame,	// sent right before deleting the .f4se cosave and the .ess save.
								// dataLen: length of file path, data: char* file path of .ess savegame file
		kMessage_InputLoaded,	// sent right after game input is loaded, right before the main menu initializes
		kMessage_NewGame,		// sent after a new game is created, before the game has loaded (Sends CharGen TESQuest pointer)
		kMessage_GameLoaded,	// sent after the game has finished loading (only sent once)
		kMessage_GameDataReady	// sent when the data handler is ready (data is false before loading, true when finished loading)
	};

	UInt32	interfaceVersion;
	bool	(* RegisterListener)(PluginHandle listener, const char* sender, EventCallback handler);
	bool	(* Dispatch)(PluginHandle sender, UInt32 messageType, void * data, UInt32 dataLen, const char* receiver);

	// Use this to acquire F4SE's internal EventDispatchers so that you can sink to them
	// Currently none implemented yet
	void	* (* GetEventDispatcher)(UInt32 dispatcherId);
};

struct F4SEScaleformInterface
{
	enum
	{
		kInterfaceVersion = 1
	};

	UInt32	interfaceVersion;

	// This callback will be called once for every new menu that is created.
	// Create your objects relative to the 'root' GFxValue parameter.
	typedef bool (* RegisterCallback)(GFxMovieView * view, GFxValue * value);

	// Register your plugin's scaleform API creation callback here.
	// The "name" parameter will be used to create an object with the path:
	// "f4se.plugins.name" that will be passed to the callback.
	// Make sure that the memory it points to is valid from the point the callback
	// is registered until the game exits.
	bool	(* Register)(const char * name, RegisterCallback callback);
};


struct F4SESerializationInterface
{
	enum
	{
		kInterfaceVersion = 1,
	};
	
	typedef void (* EventCallback)(const F4SESerializationInterface * intfc);
	typedef void (* FormDeleteCallback)(UInt64 handle);

	UInt32	version;

	void	(* SetUniqueID)(PluginHandle plugin, UInt32 uid);

	void	(* SetRevertCallback)(PluginHandle plugin, EventCallback callback);
	void	(* SetSaveCallback)(PluginHandle plugin, EventCallback callback);
	void	(* SetLoadCallback)(PluginHandle plugin, EventCallback callback);
	void	(* SetFormDeleteCallback)(PluginHandle plugin, FormDeleteCallback callback);

	bool	(* WriteRecord)(UInt32 type, UInt32 version, const void * buf, UInt32 length);
	bool	(* OpenRecord)(UInt32 type, UInt32 version);
	bool	(* WriteRecordData)(const void * buf, UInt32 length);

	bool	(* GetNextRecordInfo)(UInt32 * type, UInt32 * version, UInt32 * length);
	UInt32	(* ReadRecordData)(void * buf, UInt32 length);
	bool	(* ResolveHandle)(UInt64 handle, UInt64 * handleOut);
	bool	(* ResolveFormId)(UInt32 formId, UInt32 * formIdOut);
};

class VirtualMachine;

struct F4SEPapyrusInterface
{
	enum
	{
		kInterfaceVersion = 2
	};
	UInt32	interfaceVersion;

	typedef bool (* RegisterFunctions)(VirtualMachine * vm);
	bool	(* Register)(RegisterFunctions callback);

	typedef void (* RegistrantFunctor)(UInt64 handle, const char * scriptName, const char * callbackName, void * data);
	void	(* GetExternalEventRegistrations)(const char * eventName, void * data, RegistrantFunctor functor);
};

struct F4SETaskInterface
{
	enum
	{
		kInterfaceVersion = 2
	};
	UInt32	interfaceVersion;

	void	(* AddTask)(ITaskDelegate * task);
	void	(* AddUITask)(ITaskDelegate * task);
	void	(*AddTaskPermanent)(ITaskDelegate* task);
};

struct F4SEObjectInterface
{
	enum
	{
		kInterfaceVersion = 1
	};

	UInt32	interfaceVersion;

	F4SEDelayFunctorManager & (* GetDelayFunctorManager)();
	F4SEObjectRegistry & (* GetObjectRegistry)();
	F4SEPersistentObjectStorage & (* GetPersistentObjectStorage)();
};

struct F4SETrampolineInterface
{
	enum
	{
		kInterfaceVersion = 1
	};

	UInt32	interfaceVersion;

	void* (*AllocateFromBranchPool)(PluginHandle plugin, size_t size);
	void* (*AllocateFromLocalPool)(PluginHandle plugin, size_t size);
};

struct PluginInfo
{
	enum
	{
		kInfoVersion = 1
	};

	UInt32			infoVersion;
	const char *	name;
	UInt32			version;
};

typedef bool (* _F4SEPlugin_Load)(const F4SEInterface * f4se);

/**** plugin versioning ********************************************************
 *
 *	The NG version of Fallout 4 broke plugin versioning as many were written
 *	with the assumption that the Address Library would always be valid.
 *	These always report that they are compatible, then exit on startup because
 *	they cannot find their address library file.
 *	
 *	To work around this problem, version checking has been reimplemented and
 *	no longer calls any code. Plugins declare their compatibility, and F4SE
 *	determines whether to load the plugin. Setting this up is simple, just
 *	add something like this to your project:
 *	

extern "C" {
__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
{
	SKSEPluginVersionData::kVersion,
	
	1,
	"my awesome plugin",
	"my name",

	0,	// not version independent
	0,	// not version independent (extended field)
	{ RUNTIME_VERSION_1_10_980, 0 },	// compatible with 1.10.980

	0,	// works with any version of the script extender. you probably do not need to put anything here
};
};

 *	
 ******************************************************************************/

struct F4SEPluginVersionData
{
	enum
	{
		kVersion = 1,
	};

	enum
	{
		// set this if you exclusively use signature matching to find your addresses and have NO HARDCODED ADDRESSES
		kAddressIndependence_Signatures = 1 << 0,

		// set this if you are using a 1.10.980+ version of the Address Library
		kAddressIndependence_AddressLibrary_1_10_980 = 1 << 1,
	};
	
	enum
	{
		// set this if your plugin doesn't use any game structures
		kStructureIndependence_NoStructs = 1 << 0,

		// works with the structure layout in 1.10.980+
		kStructureIndependence_1_10_980Layout = 1 << 1,
	};

	UInt32	dataVersion;			// set to kVersion

	UInt32	pluginVersion;			// version number of your plugin
	char	name[256];				// null-terminated ASCII plugin name
	char	author[256];			// null-terminated ASCII plugin author name (can be empty)

	// version compatibility
	UInt32	addressIndependence;	// bitfield. describe how you find your addresses using the kAddressIndependence_ enums
	UInt32	structureIndependence;	// bitfield. describe how you handle structure layout using the kStructureIndependence_ enums
	UInt32	compatibleVersions[16];	// zero-terminated list of RUNTIME_VERSION_ defines your plugin is compatible with

	UInt32	seVersionRequired;		// minimum version of the script extender required, compared against PACKED_SKSE_VERSION
									// you probably should just set this to 0 unless you know what you are doing
	
	UInt32	reservedNonBreaking;	// bitfield. set to 0
	UInt32	reservedBreaking;		// bitfield. set to 0
	UInt8	reserved[512];			// set to 0
};

/**** plugin API docs **********************************************************
 *	
 *	The base API is pretty simple. Add version data as shown in the
 *	F4SEPluginVersionData docs above, and export this function:
 *	
 *	bool F4SEPlugin_Load(const F4SEInterface * f4se)
 *	
 *	In this function, use the interfaces above to register your commands, patch
 *	memory, generally do whatever you need to for integration with the runtime.
 *	
 *	At this time, or at any point forward you can call the QueryInterface
 *	callback to retrieve an interface structure for the base services provided
 *	by the F4SE core.
 *	
 *	You may optionally return false from this function to unload your plugin,
 *	but make sure that you DO NOT register any commands if you do.
 *	
 *	Note that all structure versions are backwards-compatible, so you only need
 *	to check against the latest version that you need. New fields will be only
 *	added to the end, and all old fields will remain compatible with their
 *	previous implementations.
 *	
 *	If your plugin needs to make modifications before global initializers, add
 *	and export this:
 *	
 *	bool F4SEPlugin_Preload(const F4SEInterface * f4se)
 *	
 *	Game and F4SE functionality may be limited during preload.
 *	
 ******************************************************************************/
