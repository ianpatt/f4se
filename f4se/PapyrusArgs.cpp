#include "PapyrusArgs.h"

#include "f4se/GameReferences.h"
#include "f4se/GameExtraData.h"

template <> void PackValue <void>(VMValue * dst, void * src, VirtualMachine * vm)
{
	dst->SetNone();
}

template <> void PackValue <UInt32>(VMValue * dst, UInt32 * src, VirtualMachine * vm)
{
	dst->SetInt(*src);
}

template <> void PackValue <SInt32>(VMValue * dst, SInt32 * src, VirtualMachine * vm)
{
	dst->SetInt(*src);
}

template <> void PackValue <float>(VMValue * dst, float * src, VirtualMachine * vm)
{
	dst->SetFloat(*src);
}

template <> void PackValue <bool>(VMValue * dst, bool * src, VirtualMachine * vm)
{
	dst->SetBool(*src);
}

template <> void PackValue <BSFixedString>(VMValue * dst, BSFixedString * src, VirtualMachine * vm)
{
	dst->SetString(src->c_str());
}

template <> void PackValue <VMVariable>(VMValue * dst, VMVariable * src, VirtualMachine * vm)
{
	src->PackVariable(dst);
}

template <> void PackValue <VMObject>(VMValue * dst, VMObject * src, VirtualMachine * vm)
{
	src->PackObject(dst);
}

template <> void PackValue <VMRefOrInventoryObj>(VMValue * dst, VMRefOrInventoryObj * src, VirtualMachine * vm)
{
	src->PackObject(dst);
}
template <> void PackValue <VMRefOrInventoryObj>(VMValue * dst, VMRefOrInventoryObj ** src, VirtualMachine * vm)
{
	(*src)->PackObject(dst);
}

void BindID(VMIdentifier ** identifier, void * srcData, VirtualMachine * vm, IObjectHandlePolicy * handlePolicy, UInt32 typeID)
{
	UInt32	unk = 0;

	VMObjectTypeInfo	* typeInfo = (*identifier)->m_typeInfo;
	if(typeInfo)
		typeInfo->AddRef();

	if(vm->Unk_0C(&typeInfo->m_typeName, &unk))
	{
		UInt64	handle = handlePolicy->Create(typeID, srcData);

		if(	handlePolicy->IsType(unk, handle) ||
			(handle == handlePolicy->GetInvalidHandle()))
		{
			CALL_MEMBER_FN(vm->GetObjectBindPolicy(), BindObject)(identifier, handle);
		}
	}

	if(typeInfo)
		typeInfo->Release();
}

bool GetIdentifier(VMValue * dst, UInt64 handle, VMObjectTypeInfo * typeInfo, VirtualMachine * vm)
{
	dst->SetNone();

	if(!typeInfo) return false;

	IObjectHandlePolicy	* handlePolicy = vm->GetHandlePolicy();
	VMIdentifier	* identifier = NULL;

	// find existing identifier
	if(vm->GetObjectIdentifier(handle, typeInfo->m_typeName, 0, &identifier, 0))
	{
		// copy the identifier out
		if(identifier)
		{
			VMValue	tempValue;
			tempValue.SetComplexType(typeInfo);
			CALL_MEMBER_FN(dst, Set)(&tempValue);
			dst->SetIdentifier(&identifier);
		}

		// release our reference
		if(identifier)
		{
			if(!identifier->DecrementLock())
			{
				identifier->Destroy();
			}
		}

		return true;
	}

	return false;
}

bool GetIdentifier(VMValue * dst, UInt64 handle, const BSFixedString * typeName, VirtualMachine * vm)
{
	VMObjectTypeInfo	* typeInfo = nullptr;

	// get class info
	if(vm->GetObjectTypeInfoByName(typeName, &typeInfo))
		if(typeInfo)
			typeInfo->Release();

	return GetIdentifier(dst, handle, typeInfo, vm);
}

void PackHandle(VMValue * dst, void * src, UInt32 typeID, VirtualMachine * vm)
{
	dst->SetNone();

	if(!src) return;

	VMObjectTypeInfo	* typeInfo = nullptr;

	// get class info
	if(vm->GetObjectTypeInfo(typeID, &typeInfo))
		if(typeInfo)
			typeInfo->Release();

	if(!typeInfo) return;

	IObjectHandlePolicy	* handlePolicy = vm->GetHandlePolicy();

	UInt64			handle = handlePolicy->Create(typeID, src);
	VMIdentifier	* identifier = NULL;

	// find existing identifier
	if(!vm->GetObjectIdentifier(handle, typeInfo->m_typeName, 0, &identifier, 0))
	{
		if(vm->CreateObjectIdentifier(&typeInfo->m_typeName, &identifier))
		{
			if(identifier)
			{
				BindID(&identifier, src, vm, handlePolicy, typeID);
			}
		}
	}

	// copy the identifier out
	if(identifier)
	{
		VMValue	tempValue;
		tempValue.SetComplexType(typeInfo);
		CALL_MEMBER_FN(dst, Set)(&tempValue);
		dst->SetIdentifier(&identifier);
	}

	// release our reference
	if(identifier)
	{
		if(!identifier->DecrementLock())
		{
			identifier->Destroy();
		}
	}
}

//// VMValue -> type

template <> void UnpackValue <float>(float * dst, VMValue * src)
{
	switch(src->type.value)
	{
	case VMValue::kType_Int:
		*dst = src->data.i;
		break;

	case VMValue::kType_Float:
		*dst = src->data.f;
		break;

	case VMValue::kType_Bool:
		*dst = src->data.b;
		break;

	default:
		*dst = 0;
		break;
	}
}

template <> void UnpackValue <UInt32>(UInt32 * dst, VMValue * src)
{
	switch(src->type.value)
	{
	case VMValue::kType_Int:
		*dst = src->data.u;
		break;

	case VMValue::kType_Float:
		*dst = src->data.f;
		break;

	case VMValue::kType_Bool:
		*dst = src->data.b;
		break;

	default:
		*dst = 0;
		break;
	}
}

template <> void UnpackValue <SInt32>(SInt32 * dst, VMValue * src)
{
	switch(src->type.value)
	{
	case VMValue::kType_Int:
		*dst = src->data.u;
		break;

	case VMValue::kType_Float:
		*dst = src->data.f;
		break;

	case VMValue::kType_Bool:
		*dst = src->data.b;
		break;

	default:
		*dst = 0;
		break;
	}
}

template <> void UnpackValue <bool>(bool * dst, VMValue * src)
{
	switch(src->type.value)
	{
	case VMValue::kType_Int:
		*dst = src->data.u != 0;
		break;

	case VMValue::kType_Float:
		*dst = src->data.f != 0;
		break;

	case VMValue::kType_Bool:
		*dst = src->data.b;
		break;

	default:
		*dst = 0;
		break;
	}
}

template <> void UnpackValue <BSFixedString>(BSFixedString * dst, VMValue * src)
{
	StringCache::Entry * entry = NULL;

	if(src->type.value == VMValue::kType_String)
		entry = src->data.str;
	
	CALL_MEMBER_FN(dst, Set)(entry ? entry->Get<char>() : "");
}

template <> void UnpackValue <VMVariable>(VMVariable * dst, VMValue * src)
{
	dst->UnpackVariable(src);
}

template <> void UnpackValue <VMObject>(VMObject * dst, VMValue * src)
{
	dst->UnpackObject(src);
}

template <> void UnpackValue <VMRefOrInventoryObj>(VMRefOrInventoryObj * dst, VMValue * src)
{
	dst->UnpackObject(src);
}

template <> void UnpackValue <VMArray<UInt32>>(VMArray<UInt32> * dst, VMValue * src)
{
	UnpackArray(dst, src, VMValue::kType_IntArray);
}

template <> void UnpackValue <VMArray<SInt32>>(VMArray<SInt32> * dst, VMValue * src)
{
	UnpackArray(dst, src, VMValue::kType_IntArray);
}

template <> void UnpackValue <VMArray<float>>(VMArray<float> * dst, VMValue * src)
{
	UnpackArray(dst, src, VMValue::kType_FloatArray);
}

template <> void UnpackValue <VMArray<bool>>(VMArray<bool> * dst, VMValue * src)
{
	UnpackArray(dst, src, VMValue::kType_BoolArray);
}

template <> void UnpackValue <VMArray<BSFixedString>>(VMArray<BSFixedString> * dst, VMValue * src)
{
	UnpackArray(dst, src, VMValue::kType_StringArray);
}

template <> void UnpackValue <VMArray<VMVariable>>(VMArray<VMVariable> * dst, VMValue * src)
{
	UnpackArray(dst, src, VMValue::kType_VariableArray);
}

template <> void UnpackValue <VMArray<VMObject>>(VMArray<VMObject> * dst, VMValue * src)
{
	UnpackArray(dst, src, GetTypeID<VMArray<VMObject>>((*g_gameVM)->m_virtualMachine));
}

template <> void UnpackValue <VMArray<VMRefOrInventoryObj>>(VMArray<VMRefOrInventoryObj> * dst, VMValue * src)
{
	UnpackArray(dst, src, GetTypeID<VMArray<VMRefOrInventoryObj>>((*g_gameVM)->m_virtualMachine));
}

template <> void UnpackValue(VMObject ** dst, VMValue * src)
{
	if(src->IsIdentifier() && src->data.id)
	{
		*dst = new VMObject;
		UnpackValue(*dst, src);
	}
}

template <> void DestroyValue(VMObject ** dst)
{
	if(*dst)
		delete (*dst);
}

template <> void UnpackValue(VMRefOrInventoryObj ** dst, VMValue * src)
{
	if(src->IsIdentifier() && src->data.id)
	{
		*dst = new VMRefOrInventoryObj;
		UnpackValue(*dst, src);
	}
}

template <> void DestroyValue(VMRefOrInventoryObj ** dst)
{
	if(*dst)
		delete (*dst);
}

void * UnpackHandle(VMValue * src, UInt32 typeID)
{
	if(!src->IsIdentifier()) return NULL;
	if(!src->data.id) return NULL;

	UInt64	handle = src->data.id->GetHandle();

	if(!(*g_objectHandlePolicy)->IsType(typeID, handle)) return NULL;
	if(!(*g_objectHandlePolicy)->Unk_03(handle)) return NULL;

	return (*g_objectHandlePolicy)->Resolve(typeID, handle);
}

template <> UInt64 GetTypeID <void>(VirtualMachine * vm)					{ return VMValue::kType_None; }
template <> UInt64 GetTypeID <UInt32>(VirtualMachine * vm)					{ return VMValue::kType_Int; }
template <> UInt64 GetTypeID <SInt32>(VirtualMachine * vm)					{ return VMValue::kType_Int; }
template <> UInt64 GetTypeID <int>(VirtualMachine * vm)						{ return VMValue::kType_Int; }
template <> UInt64 GetTypeID <float>(VirtualMachine * vm)					{ return VMValue::kType_Float; }
template <> UInt64 GetTypeID <bool>(VirtualMachine * vm)					{ return VMValue::kType_Bool; }
template <> UInt64 GetTypeID <BSFixedString>(VirtualMachine * vm)			{ return VMValue::kType_String; }
template <> UInt64 GetTypeID <VMVariable>(VirtualMachine * vm)				{ return VMValue::kType_Variable; }

template <> UInt64 GetTypeID <VMArray<UInt32>>(VirtualMachine * vm)			{ return VMValue::kType_IntArray; }
template <> UInt64 GetTypeID <VMArray<SInt32>>(VirtualMachine * vm)			{ return VMValue::kType_IntArray; }
template <> UInt64 GetTypeID <VMArray<int>>(VirtualMachine * vm)			{ return VMValue::kType_IntArray; }
template <> UInt64 GetTypeID <VMArray<float>>(VirtualMachine * vm)			{ return VMValue::kType_FloatArray; }
template <> UInt64 GetTypeID <VMArray<bool>>(VirtualMachine * vm)			{ return VMValue::kType_BoolArray; }
template <> UInt64 GetTypeID <VMArray<BSFixedString>>(VirtualMachine * vm)	{ return VMValue::kType_StringArray; }
template <> UInt64 GetTypeID <VMArray<VMVariable>>(VirtualMachine * vm)		{ return VMValue::kType_VariableArray; }

template <> UInt64 GetTypeID <VMObject>(VirtualMachine * vm)
{
	return GetTypeIDFromFormTypeID(VMObject::kTypeID, vm);
}

template <> UInt64 GetTypeID <VMArray<VMObject>>(VirtualMachine * vm)
{
	return GetTypeIDFromFormTypeID(VMObject::kTypeID, vm) | VMValue::kType_Identifier;
}

template <> UInt64 GetTypeID <VMRefOrInventoryObj>(VirtualMachine * vm)
{
	return GetTypeIDFromFormTypeID(VMRefOrInventoryObj::kTypeID, vm);
}

template <> UInt64 GetTypeID <VMArray<VMRefOrInventoryObj>>(VirtualMachine * vm)
{
	return GetTypeIDFromFormTypeID(VMRefOrInventoryObj::kTypeID, vm) | VMValue::kType_Identifier;
}

UInt64 GetTypeIDFromFormTypeID(UInt32 formTypeID, VirtualMachine * vm)
{
	UInt64 result = 0;
	VMObjectTypeInfo * typeInfo = nullptr;
	if(vm->GetObjectTypeInfo(formTypeID, &typeInfo))
	{
		result = (UInt64)typeInfo;
		typeInfo->Release();
	}

	return result;
}

UInt64 GetTypeIDFromStructName(const char * name, VirtualMachine * vm)
{
	UInt64 result = 0;
	VMStructTypeInfo * typeInfo = nullptr;
	BSFixedString structName(name);
	if(vm->GetStructTypeInfo(&structName, &typeInfo))
	{
		result = (UInt64)typeInfo;
		typeInfo->Release();
	}

	return result;
}

TESObjectREFR * VMRefOrInventoryObj::GetObjectReference()
{
	return m_refData.refr;
}

TESObjectREFR * VMRefOrInventoryObj::GetOwner()
{
	return m_refData.owner;
}

bool VMRefOrInventoryObj::GetExtraData(TESForm ** baseForm, ExtraDataList ** extraData)
{
	TESObjectREFR * refr = m_refData.refr;
	if(refr) {
		*baseForm = refr->baseForm;
		*extraData = refr->extraDataList;
		return true;
	}

	TESObjectREFR * owner = m_refData.owner;
	if(!owner)
		return false;

	BGSInventoryList * inventoryList = owner->inventoryList;
	if(!inventoryList)
		return false;

	for(UInt32 i = 0; i < inventoryList->items.count; i++)
	{
		BGSInventoryItem item;
		inventoryList->items.GetNthItem(i, item);
		if(!item.stack)
			continue;

		item.stack->Visit([&](BGSInventoryItem::Stack * stack)
		{
			ExtraDataList * stackDataList = stack->extraData;
			if(stackDataList) {
				ExtraUniqueID * extraUID = static_cast<ExtraUniqueID*>(stackDataList->GetByType(kExtraData_UniqueID));
				if(extraUID && extraUID->uniqueId == m_refData.uniqueId) {
					*baseForm = item.form;
					*extraData = stackDataList;
					return false;
				}
			}

			return true;
		});
	}

	return (*baseForm) != nullptr;
}
