#include "f4se/ScaleformAPI.h"

// 
RelocPtr <ScaleformHeap *> g_scaleformHeap(0x03D91400);

void * ScaleformHeap_Allocate(UInt32 size)
{
	return (*g_scaleformHeap)->Allocate(size);
}

void ScaleformHeap_Free(void * ptr)
{
	(*g_scaleformHeap)->Free(ptr);
}
