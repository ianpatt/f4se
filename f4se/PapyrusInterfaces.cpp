#include "f4se/PapyrusInterfaces.h"

// 84C7A0FD630AF430626C929A1B9A7FC62FDC358A+1B1
RelocPtr <IObjectHandlePolicy *> g_objectHandlePolicy(0x0671E578);

RelocAddr <_GetRefFromHandle> GetRefFromHandle(0x012C6590);

RelocAddr <_GetVMPropertyInfo> GetVMPropertyInfo(0x026F3970);

void IComplexType::AddRef(void)
{
	InterlockedIncrement(&m_refCount);
}

void IComplexType::Release(void)
{
	if(!InterlockedDecrement(&m_refCount))
	{
		delete this;
	}
}
