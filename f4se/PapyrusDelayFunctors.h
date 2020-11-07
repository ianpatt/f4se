#pragma once

#include "common/ICriticalSection.h"

#include "f4se/GameTypes.h"
#include "f4se/PapyrusObjects.h"

#include <deque>
#include <new>
#include <utility>
#include <vector>

class VMValue;
struct F4SESerializationInterface;

///
/// IF4SEDelayFunctor
///
class IF4SEDelayFunctor : public IF4SEObject
{
public:
	virtual ~IF4SEDelayFunctor() {}
	
	virtual bool Run(VMValue& resultOut) = 0;
	virtual IF4SEDelayFunctor * GetAsDelayFunctor() { return this; }
	
	virtual bool ShouldReschedule(SInt32& delayMSOut)  = 0;
	virtual bool ShouldResumeStack(UInt32& stackIdOut) = 0;
};

///
/// LatentF4SEDelayFunctor
///
class LatentF4SEDelayFunctor : public IF4SEDelayFunctor
{
public:
	explicit LatentF4SEDelayFunctor(UInt32 stackId);

	explicit LatentF4SEDelayFunctor(SerializationTag);

	virtual ~LatentF4SEDelayFunctor() {}

	virtual const char*	ClassName() const    = 0;
	virtual UInt32		ClassVersion() const = 0;
	
	virtual bool Run(VMValue& resultOut) = 0;
	
	virtual bool ShouldReschedule(SInt32& delayMSOut) override;
	virtual bool ShouldResumeStack(UInt32& stackIdOut) override;

	virtual bool Save(const F4SESerializationInterface* intfc) override;
	virtual bool Load(const F4SESerializationInterface* intfc, UInt32 version) override;

	UInt32 StackId() const { return stackId_; }

	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free);

protected:
	UInt32 stackId_;
};

///
/// F4SEDelayFunctorQueue
///

class F4SEDelayFunctorQueue
{
	typedef std::deque<IF4SEDelayFunctor*> DataT;

public:
	~F4SEDelayFunctorQueue();

	void				Push(IF4SEDelayFunctor* func);
	IF4SEDelayFunctor*	Pop();

	void ClearAndRelease();

	enum { kSaveVersion = 1 };

	bool Save(const F4SESerializationInterface* intfc);
	bool Load(const F4SESerializationInterface* intfc, UInt32 version);

private:
	ICriticalSection	lock_;
	DataT				data_;
};

///
/// F4SEDelayFunctorWaitList
///

class F4SEDelayFunctorWaitList
{
private:
	typedef std::pair<SInt64,IF4SEDelayFunctor*>	WaitEntryT;
	typedef std::vector<WaitEntryT>					WaitDataT;
	typedef std::vector<IF4SEDelayFunctor*>			ReadyDataT;

public:
	F4SEDelayFunctorWaitList();

	~F4SEDelayFunctorWaitList();

	void	Add(SInt32 delayMS, IF4SEDelayFunctor* func);

	// 1. Update
	// 2. PopReady until NULL

	void				Update();
	IF4SEDelayFunctor*	PopReady();

	void ClearAndRelease();

	enum { kSaveVersion = 1 };

	bool Save(const F4SESerializationInterface* intfc);
	bool Load(const F4SESerializationInterface* intfc, UInt32 version);

private:
	SInt64				lastTickTime_;
	SInt64				msToCountMult_;

	ICriticalSection	inLock_;
	WaitDataT			inData_;
	WaitDataT			waitData_;
	ReadyDataT			readyData_;
};

///
/// F4SEDelayFunctorManager
///

class F4SEDelayFunctorManager
{
public:
	F4SEDelayFunctorManager();

	// Takes ownership of passed pointer.
	virtual void Enqueue(IF4SEDelayFunctor* func, SInt32 delayMS = 0);

	void OnPreTick();
	void OnTick(SInt64 startTime, float budget);
	void OnRevert();

	enum { kSaveVersion = 1 };

	bool Save(const F4SESerializationInterface* intfc);
	bool Load(const F4SESerializationInterface* intfc, UInt32 version);

private:
	float	budgetFreqScale_;

	F4SEDelayFunctorQueue		queue_;
	F4SEDelayFunctorWaitList	waitList_;
};

///
/// Global instances
///

F4SEDelayFunctorManager&  F4SEDelayFunctorManagerInstance();

#include "f4se/PapyrusSerialization.h"
#include "f4se/PapyrusArgs.h"

#define DECLARE_DELAY_FUNCTOR_TYPE(functorName, ...) <Name_##functorName##, __VA_ARGS__> ##functorName##;
#define DECLARE_DELAY_FUNCTOR_STRING(functorName) #functorName

// FunctorName, Params, RunFunc, CallingClass, ReturnValue, Arguments...
#define DECLARE_DELAY_FUNCTOR(functorName, numParams, ...) \
	char FunctorName_##functorName##[] = "" DECLARE_STRUCT_STRING(structName); \
	typedef F4SEDelayFunctor##numParams##<FunctorName_##functorName##, __VA_ARGS__> ##functorName##;

#define NUM_PARAMS 0
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 1
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 2
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 3
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 4
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 5
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 6
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 7
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 8
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 9
#include "PapyrusDelayFunctorsDef.inl"

#define NUM_PARAMS 10
#include "PapyrusDelayFunctorsDef.inl"