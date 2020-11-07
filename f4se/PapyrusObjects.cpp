#include "PapyrusObjects.h"

#include "Serialization.h"

#include "PapyrusVM.h"

namespace
{
	static const size_t kMaxNameLen  = 1024;
}

///
/// F4SEObjectRegistry
///

bool F4SEObjectRegistry::RegisterFactory(IF4SEObjectFactory * factory)
{
	uintptr_t vtbl = *reinterpret_cast<uintptr_t*>(factory);
	std::string className( factory->ClassName() );
	auto it = factoryMap_.emplace(className, vtbl);
	return it.second;
}

const IF4SEObjectFactory* F4SEObjectRegistry::GetFactoryByName(const char* name) const
{
	std::string t( name );
	const IF4SEObjectFactory* result = NULL;
	FactoryMapT::const_iterator it = factoryMap_.find(t);
	if (it != factoryMap_.end())
	{
		result = reinterpret_cast<const IF4SEObjectFactory*>(&it->second);
	}

	return result;
}

///
/// F4SEPersistentObjectStorage
///

void F4SEPersistentObjectStorage::CleanDroppedStacks()
{
	VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;

	for (UInt32 i=0; i<data_.size(); i++)
	{
		Entry& e = data_[i];

		if (e.obj == NULL)
			continue;

		if (vm->HasStack(e.owningStackId) != NULL)
			continue;

		// Stack no longer active, drop this entry

		delete e.obj;
		e.obj = NULL;

		freeIndices_.push_back(i);

		_MESSAGE("F4SEPersistentObjectStorage::CleanDroppedStacks: Freed object at index %d.", i);
	}
}

void F4SEPersistentObjectStorage::ClearAndRelease()
{
	freeIndices_.clear();

	for (DataT::iterator it = data_.begin(); it != data_.end(); ++it)
	{
		Entry& e = *it;
		if (e.obj != NULL)
			delete e.obj;
	}

	data_.clear();
}

bool F4SEPersistentObjectStorage::Save(const F4SESerializationInterface* intfc)
{
	using namespace Serialization;

	// Before saving, purge entries whose owning stack is no longer running.
	// This can happen if someone forgot to release an object.
	// We don't want these resource leaks to pile up in the co-save.
	CleanDroppedStacks();

	// Save data
	UInt32 dataSize = data_.size();
	if (! WriteData(intfc, &dataSize))
		return false;

	UInt32 filledSize = data_.size() - freeIndices_.size();
	if (! WriteData(intfc, &filledSize))
		return false;

	for (UInt32 i=0; i<dataSize; i++)
	{
		Entry& e = data_[i];

		// Data of free indices is null, so we skip these
		if (e.obj == NULL)
			continue;
		
		// Skip to next entry if write failed
		if (! WriteF4SEObject(intfc, e.obj))
			continue;

		WriteData(intfc, &e.owningStackId);

		UInt32 index = i;
		WriteData(intfc, &index);
	}

	return true;
}

bool F4SEPersistentObjectStorage::Load(const F4SESerializationInterface* intfc, UInt32 loadedVersion)
{
	using namespace Serialization;

	// Load data
	UInt32 dataSize;
	if (! ReadData(intfc,&dataSize))
		return false;

	data_.resize(dataSize);

	UInt32 filledSize;
	if (! ReadData(intfc,&filledSize))
		return false;

	for (UInt32 i=0; i<filledSize; i++)
	{
		Entry e = { 0 };

		if (! ReadF4SEObject(intfc, e.obj))
			continue;

		ReadData(intfc, &e.owningStackId);

		UInt32 index;
		ReadData(intfc, &index);

		data_[index] = e;
	}
	
	// Rebuild free index list
	for (UInt32 i=0; i<data_.size(); i++)
		if (data_[i].obj == NULL)
			freeIndices_.push_back(i);

	return true;
}

SInt32 F4SEPersistentObjectStorage::Store(IF4SEObject* obj, UInt32 owningStackId)
{
	IScopedCriticalSection scopedLock( &lock_ );

	Entry e = { obj, owningStackId };

	SInt32 index;

	if (freeIndices_.empty())
	{
		index = data_.size();
		data_.push_back(e);
	}
	else
	{
		index = freeIndices_.back();
		freeIndices_.pop_back();
		data_[index] = e;
	}

	return index + 1;
}

IF4SEObject* F4SEPersistentObjectStorage::Access(SInt32 handle)
{
	IScopedCriticalSection scopedLock( &lock_ );

	SInt32 index = handle - 1;

	if (index < 0 || index >= data_.size())
	{
		_MESSAGE("F4SEPersistentObjectStorage::AccessObject(%d): Invalid handle.", handle);
		return NULL;
	}

	Entry& e = data_[index];

	if (e.obj == NULL)
	{
		_MESSAGE("F4SEPersistentObjectStorage::AccessObject(%d): Object was NULL.", handle);
		return NULL;
	}

	IF4SEObject* result = e.obj;
	if (result == NULL)
	{
		_MESSAGE("F4SEPersistentObjectStorage::AccessObject(%d): Invalid type (%s).", handle, e.obj->ClassName());
		return NULL;
	}

	return result;
}

IF4SEObject* F4SEPersistentObjectStorage::Take(SInt32 handle)
{
	IScopedCriticalSection scopedLock( &lock_ );

	SInt32 index = handle - 1;

	if (index < 0 || index >= data_.size())
	{
		_MESSAGE("F4SEPersistentObjectStorage::AccessObject(%d): Invalid handle.", handle);
		return NULL;
	}

	Entry& e = data_[index];

	if (e.obj == NULL)
	{
		_MESSAGE("F4SEPersistentObjectStorage::TakeObject(%d): Object was NULL.", handle);
		return NULL;
	}

	IF4SEObject* result = e.obj;
	if (result != NULL)
	{
		e.obj = NULL;
		freeIndices_.push_back(index);
	}
	else
	{
		_MESSAGE("F4SEPersistentObjectStorage::TakeObject(%d): Invalid type (%s).", handle, e.obj->ClassName());
		return NULL;
	}

	return result;
}

///
/// Serialization helpers
///

bool WriteF4SEObject(const F4SESerializationInterface* intfc, IF4SEObject* obj)
{
	using namespace Serialization;

	const char*  name	 = obj->ClassName();
	const UInt32 version = obj->ClassVersion();

	intfc->OpenRecord('OBJE', version);

	size_t rawLen = strlen(name);
	UInt32 len    = min(rawLen, kMaxNameLen);

	if (! WriteData(intfc, &len))
		return false;

	if (! intfc->WriteRecordData(name, len))
		return false;

	return obj->Save(intfc);
}

bool ReadF4SEObject(const F4SESerializationInterface* intfc, IF4SEObject*& objOut)
{
	UInt32 type, length, objVersion;

	if (! intfc->GetNextRecordInfo(&type, &objVersion, &length))
		return false;

	if (type != 'OBJE')
	{
		_MESSAGE("ReadF4SEObject: Error loading unexpected chunk type %08X (%.4s)", type, &type);
		return false;
	}

	// Read the name of the serialized class
	UInt32 len;
	if (! intfc->ReadRecordData(&len, sizeof(len)))
		return false;

	if (len > kMaxNameLen)
	{
		_MESSAGE("ReadF4SEObject: Serialization error. Class name len extended kMaxNameLen.");
		return false;
	}

	char buf[kMaxNameLen+1] = { 0 };
	if (! intfc->ReadRecordData(&buf, len))
		return false;

	// Get the factory
	const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName(buf);
	if (factory == NULL)
	{
		_MESSAGE("ReadF4SEObject: Serialization error. Factory missing for %s.", &buf);
		return false;
	}

	// Intantiate and load the actual data
	IF4SEObject* obj = factory->Create();
	if (! obj->Load(intfc, objVersion))
	{
		// Load failed. clean up.
		objOut = NULL;
		factory->Free(obj);
		return false;
	}

	objOut = obj;
	return true;
}

///
/// Global instances
///

F4SEObjectRegistry& F4SEObjectRegistryInstance()
{
	static F4SEObjectRegistry instance;
	return instance;
}

F4SEPersistentObjectStorage& F4SEObjectStorageInstance()
{
	static F4SEPersistentObjectStorage instance;
	return instance;
}
