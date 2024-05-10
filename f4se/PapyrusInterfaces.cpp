#include "f4se/PapyrusInterfaces.h"

RelocPtr <IObjectHandlePolicy *> g_objectHandlePolicy(0x03CA1D30);

RelocAddr <_GetRefFromHandle> GetRefFromHandle(0x01004170);

RelocAddr <_GetVMPropertyInfo> GetVMPropertyInfo(0x01F91520);

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
