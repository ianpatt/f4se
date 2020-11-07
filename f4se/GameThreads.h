#pragma once

class ITaskDelegate
{
public:
	virtual ~ITaskDelegate() { }

	virtual void Run() = 0;
};

// 50
class BSThread
{
public:
	virtual ~BSThread();

	virtual UInt64 Run()
	{
		return 0;
	}

	CRITICAL_SECTION cs;	// 08
	/*
	LONG LockCount;			// 08
	LONG RecursionCount;	// 0C
	HANDLE OwningThread;	// 10
	HANDLE LockSemaphore;	// 18
	ULONG_PTR SpinCount;	// 20
	*/
	UInt64		unk28;		// 28
	HANDLE		handle;		// 30
	UInt64		unk38;		// 38
	UInt64		unk40;		// 40
	UInt8		unk48;		// 48
	UInt8		unk49[7];	// 49
};

class VMInitThread : public BSThread
{
public:
	virtual UInt64 Run();

	HANDLE	eventHandle;	// 50
};

// 58
class InitGameDataThread : public BSThread
{
public:
	virtual UInt64 Run();

	HANDLE	eventHandle;	// 50
};
