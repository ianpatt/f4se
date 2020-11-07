#pragma once

#include <map>
#include <string>
#include <vector>

#include "common/ICriticalSection.h"

struct F4SESerializationInterface;
class TESForm;
class IF4SEDelayFunctor;

// Serializable classes deriving from IF4SEObject must define a
// serialization constructor that takes SerializationTag as the single argument.
// After it has been called, either Load() is used to fill in the data,
// or in case the load failed, the instance is destroyed again.
struct SerializationTag {};

///
/// IF4SEObject - Interface for a heap allocated, co-save serializable object.
///

class IF4SEObject
{
public:
	virtual ~IF4SEObject() {}

	virtual const char*	ClassName() const    = 0;
	virtual UInt32		ClassVersion() const = 0;
	virtual IF4SEDelayFunctor * GetAsDelayFunctor() { return nullptr; }

	virtual bool Save(const F4SESerializationInterface* intfc) = 0;
	virtual bool Load(const F4SESerializationInterface* intfc, UInt32 version) = 0;
};

///
/// F4SEObjectFactory
///

class IF4SEObjectFactory
{
public:
	virtual ~IF4SEObjectFactory() {}

	virtual IF4SEObject* Create() const = 0;
	virtual void Free(IF4SEObject* obj) const = 0;
	virtual const char*	ClassName() const = 0;
};

template <typename T>
class ConcreteF4SEObjectFactory : public IF4SEObjectFactory
{
public:
	virtual IF4SEObject* Create() const
	{
		SerializationTag tag;
		return new T( tag );
	}
	virtual const char*	ClassName() const
	{
		SerializationTag tag;
		T tempInstance(tag);
		return tempInstance.ClassName();
	}
	virtual void Free(IF4SEObject* obj) const
	{
		delete obj;
	}
};

///
/// F4SEObjectRegistry
///

class F4SEObjectRegistry
{
private:
	typedef std::map<std::string,uintptr_t> FactoryMapT;

public:
	template <typename T>
	bool RegisterClass()
	{
		ConcreteF4SEObjectFactory<T> factory;
		return RegisterFactory(&factory);
	}

	virtual bool RegisterFactory(IF4SEObjectFactory * factory);
	virtual const IF4SEObjectFactory* GetFactoryByName(const char* name) const;

private:
	// Stores the vtables directly
	FactoryMapT factoryMap_;
};

///
/// F4SEPersistentObjectStorage
///

class F4SEPersistentObjectStorage
{
	struct Entry;

	// Note: handle = index + 1
	// +1, because in previous versions the invalid handle was 0 so people
	// might test for != 0
public:

	// Transfer ownership to registry
	template <typename T>
	SInt32 StoreObject(T* obj, UInt32 owningStackId)
	{// lock_
		return Store(obj, owningStackId);
	}// ~lock_

	// Access object while keeping in storage
	template <typename T>
	T* AccessObject(SInt32 handle)
	{// lock_
		return reinterpret_cast<T*>(Access(handle));
	}// ~lock_

	// Remove object from storage and take ownership of the pointer
	template <typename T>
	T* TakeObject(SInt32 handle)
	{// lock_
		return reinterpret_cast<T*>(Take(handle));
	}// lock_

	virtual SInt32 Store(IF4SEObject* obj, UInt32 owningStackId);
	virtual IF4SEObject* Access(SInt32 handle);
	virtual IF4SEObject* Take(SInt32 handle);

	bool Save(const F4SESerializationInterface* intfc);
	bool Load(const F4SESerializationInterface* intfc, UInt32 version);

	void CleanDroppedStacks();
	void ClearAndRelease();

	enum { kSaveVersion = 1 };

private:
	struct Entry
	{
		IF4SEObject*	obj;
		UInt32			owningStackId;
	};

	typedef std::vector<Entry>		DataT;
	typedef std::vector<size_t>		IndexCacheT;

	ICriticalSection	lock_;
	DataT				data_;
	IndexCacheT			freeIndices_;
};

///
/// Serialization helpers
///

bool WriteF4SEObject(const F4SESerializationInterface* intfc, IF4SEObject* obj);
bool ReadF4SEObject(const F4SESerializationInterface* intfc, IF4SEObject*& objOut);

///
/// Global instances
///

F4SEObjectRegistry& F4SEObjectRegistryInstance();

F4SEPersistentObjectStorage& F4SEObjectStorageInstance();
