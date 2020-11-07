#include "f4se/PapyrusDelayFunctors.h"
#include "f4se/PapyrusObjects.h"

#include <algorithm>

#include "f4se/GameAPI.h"
#include "f4se/PapyrusVM.h"
#include "f4se/Serialization.h"

///
/// LatentF4SEDelayFunctor
///

LatentF4SEDelayFunctor::LatentF4SEDelayFunctor(UInt32 stackId) :
	stackId_( stackId )
{}

LatentF4SEDelayFunctor::LatentF4SEDelayFunctor(SerializationTag) :
	stackId_( 0 )
{}

bool LatentF4SEDelayFunctor::ShouldReschedule(SInt32& delayMSOut)
{
	return false;
}

bool LatentF4SEDelayFunctor::ShouldResumeStack(UInt32& stackIdOut)
{
	stackIdOut = stackId_;
	return true;
}

bool LatentF4SEDelayFunctor::Save(const F4SESerializationInterface* intfc)
{
	using namespace Serialization;

	WriteData(intfc, &stackId_);

	//_MESSAGE("Serialized STACKID %d", stackId_);

	return true;
}

bool LatentF4SEDelayFunctor::Load(const F4SESerializationInterface* intfc, UInt32 version)
{
	using namespace Serialization;

	if (! ReadData(intfc, &stackId_))
		return false;

	//_MESSAGE("De-serialized STACKID %d", stackId_);

	return true;
}
///
/// F4SEDelayFunctorQueue
///

F4SEDelayFunctorQueue::~F4SEDelayFunctorQueue()
{
	ClearAndRelease();
}

void F4SEDelayFunctorQueue::Push(IF4SEDelayFunctor* func)
{// lock_
	IScopedCriticalSection scopedLock( &lock_ );

	data_.push_back(func);
}// ~lock_

IF4SEDelayFunctor* F4SEDelayFunctorQueue::Pop()
{
	IF4SEDelayFunctor* result = NULL;

	{// lock_
		IScopedCriticalSection scopedLock( &lock_ );

		if (! data_.empty())
		{
			result = data_.front();
			data_.pop_front();
		}
	}// ~lock_			

	return result;
}

void F4SEDelayFunctorQueue::ClearAndRelease()
{
	for (DataT::iterator it = data_.begin(); it != data_.end(); ++it)
		delete *it;

	data_.clear();
}

bool F4SEDelayFunctorQueue::Save(const F4SESerializationInterface* intfc)
{
	using namespace Serialization;

	// Save data
	UInt32 dataSize = data_.size();
	if (! WriteData(intfc,&dataSize))
		return false;

	for (UInt32 i=0; i<dataSize; i++)
	{
		IF4SEDelayFunctor* functor = data_[i];
	
		if (! WriteF4SEObject(intfc, functor))
			return false;
	}

	return true;
}

bool F4SEDelayFunctorQueue::Load(const F4SESerializationInterface* intfc, UInt32 loadedVersion)
{
	using namespace Serialization;

	UInt32 dataSize;
	if (! ReadData(intfc,&dataSize))
		return false;

	for (UInt32 i=0; i<dataSize; i++)
	{
		IF4SEObject* obj = NULL;
		if (! ReadF4SEObject(intfc, obj))
			continue;

		const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName(obj->ClassName());
		if (factory == NULL)
		{
			continue;
		}

		// Wront type? Can't happen but who knows.
		IF4SEDelayFunctor* functor = obj->GetAsDelayFunctor();
		if (functor == NULL)
		{
			// Throw the loaded object away.
			factory->Free(obj);
			continue;
		}

		data_.push_back(functor);
	}

	return true;
}

///
/// F4SEDelayFunctorWaitList
///

F4SEDelayFunctorWaitList::F4SEDelayFunctorWaitList() :
	lastTickTime_( GetPerfCounter() )
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq); 
	freq.QuadPart /= 1000;

	msToCountMult_ = freq.QuadPart;
}

F4SEDelayFunctorWaitList::~F4SEDelayFunctorWaitList()
{
	ClearAndRelease();
}

void F4SEDelayFunctorWaitList::Add(SInt32 delayMS, IF4SEDelayFunctor* func)
{// inLock_
	IScopedCriticalSection scopedLock( &inLock_ );

	WaitEntryT t( msToCountMult_ * delayMS, func );
	inData_.push_back(t);
}// ~inLock_

void F4SEDelayFunctorWaitList::Update()
{
	// Move items from thread-safe input buffer to non-thread safe main queue

	{// inLock_
		IScopedCriticalSection scopedLock( &inLock_ );

		waitData_.insert(waitData_.end(), inData_.begin(), inData_.end());
		inData_.clear();
	}// ~inLock

	SInt64 curTime = GetPerfCounter();
	SInt64 dt      = curTime - lastTickTime_;

	lastTickTime_ = curTime;

	if (dt <= 0)
		return;

	// Decrement wait times by time delta
	for (WaitDataT::iterator it = waitData_.begin(); it != waitData_.end(); ++it)
		if (it->first > 0)
			it->first -= dt;

	struct IsStillWaiting_
	{
		bool operator()(const WaitEntryT& e) { return e.first > 0; }
	};
	IsStillWaiting_ pred;

	// Swap all ready entries to end of the vector#1, add them to vector#2, and truncate #1
	WaitDataT::iterator r = std::partition(waitData_.begin(), waitData_.end(), pred);

	for (WaitDataT::iterator it = r; it != waitData_.end(); ++it)
		readyData_.push_back(it->second );

	waitData_.resize(std::distance(waitData_.begin(), r));
}

IF4SEDelayFunctor* F4SEDelayFunctorWaitList::PopReady()
{
	IF4SEDelayFunctor* result = NULL;

	if (! readyData_.empty())
	{
		result = readyData_.back();
		readyData_.pop_back();
	}			

	return result;
}

void F4SEDelayFunctorWaitList::ClearAndRelease()
{
	for (WaitDataT::iterator it = inData_.begin(); it != inData_.end(); ++it)
	{
		const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName(it->second->ClassName());
		if (factory == NULL)
		{
			continue;
		}

		factory->Free(it->second);
	}

	inData_.clear();

	for (WaitDataT::iterator it = waitData_.begin(); it != waitData_.end(); ++it)
	{
		const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName(it->second->ClassName());
		if (factory == NULL)
		{
			continue;
		}

		factory->Free(it->second);
	}

	waitData_.clear();

	for (ReadyDataT::iterator it = readyData_.begin(); it != readyData_.end(); ++it)
	{
		const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName((*it)->ClassName());
		if (factory == NULL)
		{
			continue;
		}
		factory->Free(*it);
	}

	readyData_.clear();

	// Avoid interval spanning two sessions
	lastTickTime_= GetPerfCounter();
}

bool F4SEDelayFunctorWaitList::Save(const F4SESerializationInterface* intfc)
{
	using namespace Serialization;

	// inData_
	UInt32 inDataSize = inData_.size();
	if (! WriteData(intfc,&inDataSize))
		return false;

	for (UInt32 i=0; i<inDataSize; i++)
	{
		SInt64				delay	= inData_[i].first;
		IF4SEDelayFunctor*	functor = inData_[i].second;
	
		if (! WriteF4SEObject(intfc, functor))
			return false;

		if (! WriteData(intfc, &delay))
			return false;
	}

	// waitData_
	UInt32 waitDataSize = waitData_.size();
	if (! WriteData(intfc,&waitDataSize))
		return false;

	for (UInt32 i=0; i<waitDataSize; i++)
	{
		SInt64				delay	= waitData_[i].first;
		IF4SEDelayFunctor*	functor = waitData_[i].second;
	
		if (! WriteF4SEObject(intfc, functor))
			return false;

		if (! WriteData(intfc, &delay))
			return false;
	}

	// readyData_
	UInt32 readyDataSize = readyData_.size();
	if (! WriteData(intfc,&readyDataSize))
		return false;

	for (UInt32 i=0; i<readyDataSize; i++)
	{
		IF4SEDelayFunctor* functor = readyData_[i];
	
		if (! WriteF4SEObject(intfc, functor))
			return false;
	}

	return true;
}

bool F4SEDelayFunctorWaitList::Load(const F4SESerializationInterface* intfc, UInt32 loadedVersion)
{
	using namespace Serialization;

	// inData_
	UInt32 inDataSize;
	if (! ReadData(intfc,&inDataSize))
		return false;

	inData_.reserve(inDataSize);

	for (UInt32 i=0; i<inDataSize; i++)
	{
		IF4SEObject* obj = NULL;
		if (! ReadF4SEObject(intfc, obj))
			continue;

		const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName(obj->ClassName());
		if (factory == NULL)
		{
			continue;
		}

		// Wront type? Can't happen but who knows.
		IF4SEDelayFunctor* functor = obj->GetAsDelayFunctor();
		if (functor == NULL)
		{
			// Throw the loaded object away.
			factory->Free(obj);
			continue;
		}

		SInt64 delay;
		if (! ReadData(intfc, &delay))
			return false;
		
		WaitEntryT t( delay, functor );
		inData_.push_back(t);
	}

	// waitData_
	UInt32 waitDataSize;
	if (! ReadData(intfc,&waitDataSize))
		return false;

	waitData_.reserve(waitDataSize);

	for (UInt32 i=0; i<waitDataSize; i++)
	{
		IF4SEObject* obj = NULL;
		if (! ReadF4SEObject(intfc, obj))
			continue;

		const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName(obj->ClassName());
		if (factory == NULL)
		{
			continue;
		}

		// Wront type? Can't happen but who knows.
		IF4SEDelayFunctor* functor = obj->GetAsDelayFunctor();
		if (functor == NULL)
		{
			// Throw the loaded object away.
			factory->Free(obj);
			continue;
		}

		SInt64 delay;
		if (! ReadData(intfc, &delay))
			return false;
		
		WaitEntryT t( delay, functor );
		waitData_.push_back(t);
	}

	// readyData_
	UInt32 readyDataSize;
	if (! ReadData(intfc,&readyDataSize))
		return false;

	readyData_.reserve(readyDataSize);

	for (UInt32 i=0; i<readyDataSize; i++)
	{
		IF4SEObject* obj = NULL;
		if (! ReadF4SEObject(intfc, obj))
			continue;

		const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName(obj->ClassName());
		if (factory == NULL)
		{
			continue;
		}

		// Wront type? Can't happen but who knows.
		IF4SEDelayFunctor* functor = obj->GetAsDelayFunctor();
		if (functor == NULL)
		{
			// Throw the loaded object away.
			factory->Free(obj);
			continue;
		}

		readyData_.push_back(functor);
	}

	return true;
}

///
/// F4SEDelayFunctorManager
///

F4SEDelayFunctorManager::F4SEDelayFunctorManager()
{
	LARGE_INTEGER t;
	QueryPerformanceFrequency(&t);
	budgetFreqScale_ = double(t.QuadPart) * 0.001;
}

void F4SEDelayFunctorManager::Enqueue(IF4SEDelayFunctor* func, SInt32 delayMS)
{
	if (delayMS < 0)
		queue_.Push(func);
	else
		waitList_.Add(delayMS, func);
}

void F4SEDelayFunctorManager::OnPreTick()
{
	waitList_.Update();

	IF4SEDelayFunctor* func;
	while ((func = waitList_.PopReady()) != NULL)
		queue_.Push(func);
}

void F4SEDelayFunctorManager::OnTick(SInt64 startTime, float budget)
{
	SInt64 budgetTime = budget * budgetFreqScale_;

	do
	{
		IF4SEDelayFunctor* functor = queue_.Pop();
		if (functor == NULL)
			break;
	
		VMValue result;
		functor->Run(result);

		SInt32 delayMS;
		if (functor->ShouldReschedule(delayMS))
		{
			if (delayMS > 0)
				waitList_.Add(delayMS, functor);
			else
				queue_.Push(functor);
			continue;
		}

		UInt32 stackId;
		if (functor->ShouldResumeStack(stackId))
		{
			(*g_gameVM)->m_virtualMachine->ResumeStack(stackId, &result);
		}

		const IF4SEObjectFactory* factory = F4SEObjectRegistryInstance().GetFactoryByName(functor->ClassName());
		if (factory == NULL)
		{
			continue;
		}

		factory->Free(functor);
	}
	while (GetPerfCounter() - startTime <= budgetTime);
}

void F4SEDelayFunctorManager::OnRevert()
{
	queue_.ClearAndRelease();
	waitList_.ClearAndRelease();
}

bool F4SEDelayFunctorManager::Save(const F4SESerializationInterface* intfc)
{
	using namespace Serialization;

	if (! SaveClassHelper(intfc, 'FUNQ', queue_))
		return false;

	if (! SaveClassHelper(intfc, 'WLST', waitList_))
		return false;

	intfc->OpenRecord('____', 1);

	return true;
}

bool F4SEDelayFunctorManager::Load(const F4SESerializationInterface* intfc, UInt32 loadedVersion)
{
	using namespace Serialization;

	UInt32 type, length, version;

	while (intfc->GetNextRecordInfo(&type, &version, &length))
	{
		switch (type)
		{
		case 'FUNQ':
			if (! queue_.Load(intfc, version))
				return false;
			break;

		case 'WLST':
			if (! waitList_.Load(intfc, version))
				return false;
			break;

		// Done
		case '____':	
			return true;

		default:
			_MESSAGE("Error loading unexpected chunk type %08X (%.4s)", type, &type);
			break;
		}
	}

	_MESSAGE("Missing record data for F4SEDelayFunctorManager");
	return false;
}

///
/// Global instances
///

F4SEDelayFunctorManager& F4SEDelayFunctorManagerInstance()
{
	static F4SEDelayFunctorManager instance;
	return instance;
}
