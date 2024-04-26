#include "f4se/PapyrusInterfaces.h"

RelocPtr <IObjectHandlePolicy *> g_objectHandlePolicy(0x03CA0D30);

RelocAddr <_GetRefFromHandle> GetRefFromHandle(0x01003DA0);

RelocAddr <_GetVMPropertyInfo> GetVMPropertyInfo(0x01F90E00);

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
