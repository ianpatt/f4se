#include "f4se/PapyrusInterfaces.h"

RelocPtr <IObjectHandlePolicy *> g_objectHandlePolicy(0x03E5D630);

RelocAddr <_GetRefFromHandle> GetRefFromHandle(0x0108A1A0);

RelocAddr <_GetVMPropertyInfo> GetVMPropertyInfo(0x020EE710);

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
