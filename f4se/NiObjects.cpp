#include "f4se/NiObjects.h"
#include "f4se/NiExtraData.h"
#include "f4se/NiNodes.h"

RelocAddr <_WorldToScreen> WorldToScreen_Internal(0x00A349E0);

void NiRefObject::IncRef(void)
{
	InterlockedIncrement(&m_uiRefCount);
}

bool NiRefObject::Release(void)
{
	return InterlockedDecrement(&m_uiRefCount) == 0;
}

void NiRefObject::DecRef(void)
{
	if(Release())
		DeleteThis();
}

NiExtraData * NiObjectNET::GetExtraData(const BSFixedString & name)
{
	if(!m_extraData)
		return false;

	SimpleLocker locker(&m_extraData->lock);
	for(UInt32 i = 0; i < m_extraData->count; i++)
	{
		NiPointer<NiExtraData> data = m_extraData->entries[i];
		if(data && data->m_name == name)
			return data;
	}

	return nullptr;
}

bool NiObjectNET::AddExtraData(NiExtraData * extraData)
{
	return CALL_MEMBER_FN(this, Internal_AddExtraData)(extraData);
}

bool NiAVObject::Visit(const std::function<bool(NiAVObject*)>& functor)
{
	if (functor(this))
		return true;

	NiPointer<NiNode> node(GetAsNiNode());
	if (node) {
		for (UInt32 i = 0; i < node->m_children.m_emptyRunStart; i++) {
			NiPointer<NiAVObject> object(node->m_children.m_data[i]);
			if (object) {
				if (object->Visit(functor))
					return true;
			}
		}
	}

	return false;
}
