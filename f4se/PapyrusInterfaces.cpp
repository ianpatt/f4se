#include "f4se/PapyrusInterfaces.h"

RelocPtr <IObjectHandlePolicy *> g_objectHandlePolicy(0x03E36DE0);

RelocAddr <_GetRefFromHandle> GetRefFromHandle(0x01085810);

RelocAddr <_GetVMPropertyInfo> GetVMPropertyInfo(0x020E2110);

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
