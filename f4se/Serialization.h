#pragma once

#include "f4se/PluginAPI.h"
#include "f4se/GameTypes.h"

#include <string>

class GFxValue;

namespace Serialization
{
	struct PluginCallbacks
	{
		PluginCallbacks()
			:revert(NULL)
			,save(NULL)
			,load(NULL)
			,formDelete(NULL)
			,uid(0)
			,hadData(false)
			,hadUID(false) { }

		F4SESerializationInterface::EventCallback	revert;
		F4SESerializationInterface::EventCallback	save;
		F4SESerializationInterface::EventCallback	load;
		F4SESerializationInterface::FormDeleteCallback	formDelete;
		
		UInt32	uid;

		bool	hadData;
		bool	hadUID;
	};

	// plugin API
	void	SetUniqueID(PluginHandle plugin, UInt32 uid);
	void	SetRevertCallback(PluginHandle plugin, F4SESerializationInterface::EventCallback callback);
	void	SetSaveCallback(PluginHandle plugin, F4SESerializationInterface::EventCallback callback);
	void	SetLoadCallback(PluginHandle plugin, F4SESerializationInterface::EventCallback callback);
	void	SetFormDeleteCallback(PluginHandle plugin, F4SESerializationInterface::FormDeleteCallback callback);

	void	SetSaveName(const char * name);
	bool	WriteRecord(UInt32 type, UInt32 version, const void * buf, UInt32 length);
	bool	OpenRecord(UInt32 type, UInt32 version);
	bool	WriteRecordData(const void * buf, UInt32 length);

	bool	GetNextRecordInfo(UInt32 * type, UInt32 * version, UInt32 * length);
	UInt32	ReadRecordData(void * buf, UInt32 length);

	bool	ResolveFormId(UInt32 formId, UInt32 * formIdOut);
	bool	ResolveHandle(UInt64 handle, UInt64 * handleOut);

	// internal event handlers
	void	HandleRevertGlobalData(void);
	void	HandleSaveGlobalData(void);
	void	HandleLoadGlobalData(void);

	void	HandleDeleteSave(std::string saveName);
	void	HandleDeletedForm(UInt64 handle);

	// template helper functions
	template <typename T>
	bool WriteData(const F4SESerializationInterface * intfc, const T * data)
	{
		return intfc->WriteRecordData(data, sizeof(T));
	}

	template <typename T>
	bool ReadData(const F4SESerializationInterface * intfc, T * data)
	{
		return intfc->ReadRecordData(data, sizeof(T)) > 0;
	}

	template <> bool WriteData<BSFixedString>(const F4SESerializationInterface * intfc, const BSFixedString * data);
	template <> bool ReadData<BSFixedString>(const F4SESerializationInterface * intfc, BSFixedString * data);

	template <> bool WriteData<std::string>(const F4SESerializationInterface * intfc, const std::string * data);
	template <> bool ReadData<std::string>(const F4SESerializationInterface * intfc, std::string * data);

	// Note: Read would have to allocate somehow. You have to do that manually.
	template <> bool WriteData<const char>(const F4SESerializationInterface * intfc, const char* data);

	template <typename T>
	bool SaveClassHelper(const F4SESerializationInterface* intfc, UInt32 type, T& instance)
	{
		if (! intfc->OpenRecord(type, T::kSaveVersion))
			return false;

		return instance.Save(intfc);
	}
}
