#include "f4se/ScaleformAPI.h"

// 2D9A7EEA0F5FCB0ACE8EC9010926351C3F504E26+28
RelocPtr <ScaleformHeap *> g_scaleformHeap(0x06577EB0);

void * ScaleformHeap_Allocate(UInt32 size)
{
	return (*g_scaleformHeap)->Allocate(size);
}

void ScaleformHeap_Free(void * ptr)
{
	(*g_scaleformHeap)->Free(ptr);
}
