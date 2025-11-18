#include "PapyrusVM.h"

RelocPtr <GameVM *> g_gameVM(0x030CFF08);

bool VirtualMachine::HasStack(UInt32 stackId)
{
	BSReadLocker locker(&stackLock);
	return m_allStacks.Find(&stackId) != NULL;
}
