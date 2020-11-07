#include "Hooks_Memory.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"
#include "xbyak/xbyak.h"
#include "GameMemory.h"
#include "GameAPI.h"
#include <set>

#if 0

static RelocAddr <uintptr_t> kHook_IMemoryStoreCtor_Offset(0x00000000);
static RelocAddr <void *> kHook_IMemoryStoreCtor_VTable(0x00000000);

static RelocAddr <uintptr_t> kHook_IMemoryStoreDtor_Offset(0x00000000);

typedef std::set <IMemoryStoreBase *> HeapList;
static HeapList s_heaps;

void Hooks_Memory_Init()
{
	//
}

static IMemoryStore * Hook_IMemoryStoreCtor(IMemoryStore * obj)
{
	s_heaps.insert(obj);

	// write the vtable
	void ** rawObj = (void **)obj;

	*rawObj = kHook_IMemoryStoreCtor_VTable;

	return obj;
}

static void Hook_IMemoryStoreDtor(IMemoryStoreBase * obj)
{
	// this should update the vtable, but it shouldn't matter

	s_heaps.erase(obj);
}

void Hooks_Memory_Commit()
{
	g_branchTrampoline.Write6Branch(kHook_IMemoryStoreCtor_Offset, uintptr_t(Hook_IMemoryStoreCtor));
	g_branchTrampoline.Write6Branch(kHook_IMemoryStoreDtor_Offset, uintptr_t(Hook_IMemoryStoreDtor));
}

std::string FriendlyMemSize(UInt64 size)
{
	char result[256];

	if(!size)
		sprintf_s(result, sizeof(result), "0");
	else if(size < 1024)
		sprintf_s(result, sizeof(result), "%dB", size);
	else if(size < 1024 * 1024)
		sprintf_s(result, sizeof(result), "%dKB", size / 1024);
	else if(size < 1024 * 1024 * 1024)
		sprintf_s(result, sizeof(result), "%dMB", size / (1024 * 1024));
	else if(size < 1024LL * 1024 * 1024 * 1024)
		sprintf_s(result, sizeof(result), "%dGB", size / (1024 * 1024 * 1024));
	else
		sprintf_s(result, sizeof(result), "%dTB", size / (1024LL * 1024 * 1024 * 1024));

	return result;
}

void PrintHeapStatus()
{
	Console_Print("%d heaps", s_heaps.size());

	_MESSAGE("%d heaps", s_heaps.size());

	for(HeapList::iterator iter = s_heaps.begin(); iter != s_heaps.end(); ++iter)
	{
		IMemoryStoreBase * heap = *iter;

		_MESSAGE("[ heap %016I64X ]", heap);
		gLog.Indent();

		void ** rawObj = (void **)heap;

		_MESSAGE("vtbl:             %016I64X", *rawObj);

		IMemoryStore::MemInfo info = { 0 };
		heap->GetStats(&info);

		_MESSAGE("name:             %s", info.debugName);
		_MESSAGE("allocated:        %016I64X (%s)", info.bytesAllocated, FriendlyMemSize(info.bytesAllocated).c_str());
		_MESSAGE("alloc chunk size: %016I64X (%s)", info.allocChunkSize2, FriendlyMemSize(info.allocChunkSize2).c_str());
		_MESSAGE("max size:         %016I64X (%s)", info.maxSize, FriendlyMemSize(info.maxSize).c_str());
		_MESSAGE("heap overhead:    %08X (%s)", info.unk20, FriendlyMemSize(info.unk20).c_str());
		_MESSAGE("remaining:        %016I64X (%s)", info.remaining, FriendlyMemSize(info.remaining).c_str());

		gLog.Outdent();
	}
}

#endif
