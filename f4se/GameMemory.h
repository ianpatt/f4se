#pragma once

// 08
class IMemoryStoreBase
{
public:
	struct MemInfo
	{
		char	* debugName;		// 00
		UInt64	bytesAllocated;		// 08
		UInt64	allocChunkSize2;	// 10
		UInt64	maxSize;			// 18
		UInt32	unk20;				// 20 - heap overhead? AbstractHeap::unk88 * 0x20 + 0x2A8
		UInt32	pad24;				// 24
		UInt64	remaining;			// 28
	};

	virtual ~IMemoryStoreBase();

	virtual UInt64	ConvertAddr(UInt64 addr) = 0;
	virtual void	GetStats(MemInfo * dst) = 0;
	virtual bool	OwnedBy(void * ptr) = 0;	// returns true if ptr is managed by this

//	void ** _vtbl;	// 00
};

// 08
class IMemoryStore : public IMemoryStoreBase
{
public:
	virtual ~IMemoryStore();

	virtual void *	Alloc(UInt64 len, UInt32 align) = 0;	// probably alloc
	virtual void	Free(void * buf) = 0;	// probably free
	virtual UInt32	Unk06() = 0;
};

// 08
class IMemoryHeap : public IMemoryStore
{
public:
	struct HeapInfo
	{
		char	* debugName;		// 00
		UInt64	maxSize;			// 08
		UInt64	allocChunkSize2;	// 10
		UInt64	unk18;				// 18
		UInt32	unk20;				// 20
		UInt32	unk24;				// 24
		UInt64	unk28;				// 28
		UInt64	unk30;				// 30
		UInt64	unk38;				// 38
		UInt64	unk40;				// 40
		UInt64	unk48;				// 48
		UInt64	unk50;				// 50
		UInt64	unk58;				// 58
		UInt64	unk60;				// 60
	};

	virtual ~IMemoryHeap();

	virtual char *	GetDebugName();
	virtual void *	InternalAlloc(UInt64 len) = 0;
	virtual void	InternalFree(void * buf) = 0;
	virtual bool	OwnedBy(void  * buf);
	virtual void	Unk0B() = 0;
	virtual void	GetHeapStats(HeapInfo * stats);
	virtual bool	Unk0D();
	virtual UInt32	Unk0E();
};

// A8
class AbstractHeap : public IMemoryStore
{
public:
	virtual ~AbstractHeap();

	virtual void *	RawAlloc(UInt32 unk0, UInt64 size) = 0;
	virtual void	RawFree(void * buf) = 0;
	virtual UInt32	Unk11();
	virtual UInt32	Unk12();
	virtual void	Unk13();
	virtual void	Unk14();

	CRITICAL_SECTION	lock;	// 08

	char	* debugName;		// 30 - ctor arg2
	UInt64	unk38;				// 38 - init'd to 0x30 in ctor
	UInt32	unk40;				// 40 - ctor arg3
	UInt32	pad44;				// 44
	UInt64	maxSize;			// 48 - ctor arg0, guessing based on context
	UInt64	allocChunkSize1;	// 50 - ctor arg1
	UInt64	unk58;				// 58 - ctor arg1
	UInt64	unk60;				// 60 - init'd to 0 in ctor
	UInt64	unk68;				// 68 - init'd to 0 in ctor
	UInt64	unk70;				// 70 - init'd to 0 in ctor
	UInt64	unk78;				// 78 - init'd to 0 in ctor
	UInt64	unk80;				// 80 - init'd to 0 in ctor
	UInt32	unk88;				// 88 - init'd to 0 in ctor
	UInt32	pad8C;				// 8C
	UInt64	unk90;				// 90 - init'd to 0 in ctor
	UInt64	unk98;				// 98 - init'd to 0 in ctor
	UInt32	unkA0;				// A0 - init'd to 0 in ctor
	UInt8	unkA4;				// A4 - ctor arg5
	UInt8	unkA5;				// A5 - ctor arg4
	UInt16	padA6;				// A6
};

// 2B0
class MemoryHeap : public AbstractHeap
{
public:
	virtual ~MemoryHeap();

	UInt8	unk0A8[0x100];	// 0A8
	UInt8	unk1A8[0x100];	// 1A8
	UInt8	unk2A8;			// 2A8
	UInt8	pad2A9[7];		// 2A9
};

// 38
// heap designed for single-time allocs. freed memory is never reclaimed
class ZeroOverheadHeap : public IMemoryHeap
{
public:
	virtual ~ZeroOverheadHeap();

	UInt64	size;			// 08
	char	* debugName;	// 10
	void	* buffer;		// 18
	void	* curPtr;		// 20
	UInt64	unk28;			// 28
	UInt32	unk30;			// 30
	UInt32	pad34;			// 34
};
