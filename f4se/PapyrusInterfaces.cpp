#include "f4se/PapyrusInterfaces.h"

RelocPtr <IObjectHandlePolicy *> g_objectHandlePolicy(0x03E46CF0);

RelocAddr <_GetRefFromHandle> GetRefFromHandle(0x01089E50);

RelocAddr <_GetVMPropertyInfo> GetVMPropertyInfo(0x020E6CE0);

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
