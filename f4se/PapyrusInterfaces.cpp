#include "f4se/PapyrusInterfaces.h"

RelocPtr <IObjectHandlePolicy *> g_objectHandlePolicy(0x03E46C70);

RelocAddr <_GetRefFromHandle> GetRefFromHandle(0x01089CB0);

RelocAddr <_GetVMPropertyInfo> GetVMPropertyInfo(0x020E6C00);

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
