#include "PapyrusVM.h"

// 1E364B1377EA84E44CC933976A8BE6D548B7D5AF+C
RelocPtr <GameVM *> g_gameVM(0x058D3308);

bool VirtualMachine::HasStack(UInt32 stackId)
{
	BSReadLocker locker(&stackLock);
	return m_allStacks.Find(&stackId) != NULL;
}
