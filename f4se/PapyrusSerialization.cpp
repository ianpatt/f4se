#include "f4se/PapyrusSerialization.h"

#include "f4se/PapyrusArgs.h"

namespace Serialization
{
	bool WriteVMData(const F4SESerializationInterface* intfc, const VMValue * val)
	{
		UInt8 typeId = val->GetTypeEnum();
		if(!WriteData(intfc, &typeId))
			return false;

		switch(typeId)
		{
		case VMValue::kType_Identifier:
			{
				UInt64 handle = 0;
				BSFixedString typeName;

				VMIdentifier * id = val->data.id;
				if(id) {
					handle = id->GetHandle();
					VMObjectTypeInfo * typeObject = id->m_typeInfo;
					if(typeObject)
						typeName = typeObject->m_typeName;
				}

				if(!WriteData(intfc, &typeName))
					return false;
				if(!WriteData(intfc, &handle))
					return false;
			}
			break;
		case VMValue::kType_String:
			{
				if(!WriteData(intfc, val->data.GetStr()))
					return false;
			}
			break;
		case VMValue::kType_Int:
			{
				if(!WriteData(intfc, &val->data.i))
					return false;
			}
			break;
		case VMValue::kType_Float:
			{
				if(!WriteData(intfc, &val->data.f))
					return false;
			}
			break;
		case VMValue::kType_Bool:
			{
				if(!WriteData(intfc, &val->data.b))
					return false;
			}
			break;
		case VMValue::kType_Variable:
			{
				if(!WriteVMData(intfc, val->data.var))
					return false;
			}
			break;
		case VMValue::kType_Struct:
			{
				VMValue::StructData * strct = val->data.strct;
				VMValue * entries = strct->GetStruct();
				VMStructTypeInfo * typeObject = strct->m_type;
				UInt32 members = typeObject->m_members.Size();

				if(!WriteData(intfc, &typeObject->m_typeName))
					return false;

				if(!WriteData(intfc, &members))
					return false;

				typeObject->m_members.ForEach([&intfc, &entries](VMStructTypeInfo::MemberItem * item)
				{
					WriteData(intfc, &item->name);				// Write Key
					WriteVMData(intfc, &entries[item->index]);	// Write Value
					return true;
				});
			}
			break;
		case VMValue::kType_IdentifierArray:
			{
				UInt32 length = 0;
				VMValue::ArrayData * arr = val->data.arr;
				if(arr) {
					length = arr->arr.count;
				}

				if(!WriteData(intfc, &length))
					return false;

				BSFixedString typeName;
				IComplexType * typeObject = val->GetComplexType();
				if(typeObject) {
					typeName = typeObject->m_typeName;
				}

				if(!WriteData(intfc, &typeObject->m_typeName))
					return false;

				if(arr)
				{
					for(UInt32 i = 0; i < length; i++)
					{
						VMValue entry;
						arr->arr.GetNthItem(i, entry);

						UInt64 handle = 0;
						VMIdentifier * id = entry.data.id;
						if(id) {
							handle = id->GetHandle();
						}

						if(!WriteData(intfc, &handle))
							return false;
					}
				}
			}
			break;
		case VMValue::kType_StringArray:
		case VMValue::kType_IntArray:
		case VMValue::kType_FloatArray:
		case VMValue::kType_BoolArray:
			{
				UInt32 length = 0;
				VMValue::ArrayData * arr = val->data.arr;
				if(arr) {
					length = arr->arr.count;
				}

				if(!WriteData(intfc, &length))
					return false;

				if(arr)
				{
					for(UInt32 i = 0; i < length; i++)
					{
						VMValue entry;
						arr->arr.GetNthItem(i, entry);
						switch(typeId) {
						case VMValue::kType_StringArray:
							if(!WriteData(intfc, entry.data.GetStr()))
								return false;
							break;
						case VMValue::kType_IntArray:
							if(!WriteData(intfc, &entry.data.i))
								return false;
							break;
						case VMValue::kType_FloatArray:
							if(!WriteData(intfc, &entry.data.f))
								return false;
							break;
						case VMValue::kType_BoolArray:
							if(!WriteData(intfc, &entry.data.b))
								return false;
							break;
						}
					}
				}
			}
			break;
		case VMValue::kType_VariableArray:
			{
				UInt32 length = 0;
				VMValue::ArrayData * arr = val->data.arr;
				if(arr) {
					length = arr->arr.count;
				}

				if(!WriteData(intfc, &length))
					return false;

				if(arr)
				{
					for(UInt32 i = 0; i < length; i++)
					{
						VMValue entry;
						arr->arr.GetNthItem(i, entry);

						if(!WriteVMData(intfc, &entry))
							return false;
					}
				}
			}
			break;
		case VMValue::kType_StructArray:
			{
				UInt32 length = 0;
				VMValue::ArrayData * arr = val->data.arr;
				IComplexType * typeArrObject = val->GetComplexType();
				if(arr) {
					length = arr->arr.count;
				}

				BSFixedString typeName;
				VMStructTypeInfo * typeObject = nullptr;
				if(typeArrObject && typeArrObject->GetType() == VMValue::kType_Struct) {
					typeObject = static_cast<VMStructTypeInfo *>(typeArrObject);
					typeName = typeObject->m_typeName;
				}

				if(!WriteData(intfc, &length))
					return false;

				if(!WriteData(intfc, &typeName))
					return false;

				UInt32 members = typeObject ? typeObject->m_members.Size() : 0;
				if(!WriteData(intfc, &members))
					return false;

				for(UInt32 i = 0; i < length; i++)
				{
					VMValue entry;
					arr->arr.GetNthItem(i, entry);

					VMValue::StructData * strct = entry.data.strct;
					VMValue * entries = strct->GetStruct();
					if(typeObject)
					{
						typeObject->m_members.ForEach([&intfc, &entries](VMStructTypeInfo::MemberItem * item)
						{
							WriteData(intfc, &item->name);				// Write Key
							WriteVMData(intfc, &entries[item->index]);	// Write Value
							return true;
						});
					}
				}
			}
			break;
		}
		return true;
	}

	bool ReadVMData(const F4SESerializationInterface* intfc, VMValue * val)
	{
		VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;

		UInt8 typeId = 0;
		if(!ReadData(intfc, &typeId))
			return false;

		switch(typeId)
		{
		case VMValue::kType_Identifier:
			{
				BSFixedString typeName;
				if(!ReadData(intfc, &typeName))
					return false;

				UInt64 handle = 0;
				if(!ReadData(intfc, &handle))
					return false;

				if(intfc->ResolveHandle(handle, &handle)) {
					GetIdentifier(val, handle, &typeName, vm);
				}
			}
			break;
		case VMValue::kType_String:
			{
				BSFixedString str;
				if(!ReadData(intfc, &str))
					return false;
				val->SetString(str);
			}
			break;
		case VMValue::kType_Int:
			{
				SInt32 i = 0;
				if(!ReadData(intfc, &i))
					return false;
				val->SetInt(i);
			}
			break;
		case VMValue::kType_Float:
			{
				float f = 0.0f;
				if(!WriteData(intfc, &f))
					return false;
				val->SetFloat(f);
			}
			break;
		case VMValue::kType_Bool:
			{
				bool b = false;
				if(!WriteData(intfc, &b))
					return false;
				val->SetBool(b);
			}
			break;
		case VMValue::kType_Variable:
			{
				VMValue * value = new VMValue;
				if(!ReadVMData(intfc, value)) {
					delete value;
					return false;
				}
				val->SetVariable(value);
			}
			break;
		case VMValue::kType_Struct:
			{
				BSFixedString structName;
				if(!ReadData(intfc, &structName))
					return false;

				UInt32 members;
				if(!ReadData(intfc, &members))
					return false;

				VMStructTypeInfo * typeObject = nullptr;
				if(vm->GetStructTypeInfo(&structName, &typeObject))
					if(typeObject)
						typeObject->Release();

				val->SetComplexType(typeObject);

				// Make sure our type still exists
				VMValue::StructData * strct = nullptr;
				if(typeObject) {
					vm->CreateStruct(&structName, &strct);

					// Setup all the empty types? Need to confirm whether CreateStruct already does this
					typeObject->m_members.ForEach([&strct, &typeObject](VMStructTypeInfo::MemberItem * item)
					{
						strct->GetStruct()[item->index].type.value = typeObject->m_data.entries[item->index].m_type;
						return true;
					});
				}

				for(UInt32 i = 0; i < members; i++)
				{
					BSFixedString member;
					if(!ReadData(intfc, &member))	// Read Key
						return false;

					VMValue data;
					if(!ReadVMData(intfc, &data))	// Read Value
						return false;

					if(strct)
					{
						auto memberItem = typeObject->m_members.Find(&member);
						if(memberItem)
							strct->GetStruct()[memberItem->index] = data;
					}
				}

				val->data.strct = strct;
			}
			break;
		case VMValue::kType_IdentifierArray:
			{
				UInt32 length = 0;
				if(!ReadData(intfc, &length))
					return false;

				BSFixedString typeName;
				if(!ReadData(intfc, &typeName))
					return false;

				VMValue::ArrayData * data = nullptr;
				vm->CreateArray(val, length, &data);

				VMObjectTypeInfo * typeInfo = nullptr;
				if(vm->GetObjectTypeInfoByName(&typeName, &typeInfo))
					if(typeInfo)
						typeInfo->Release();

				for(UInt32 i = 0; i < length; i++)
				{
					UInt64 handle = 0;
					if(!ReadData(intfc, &handle))
						return false;

					if(intfc->ResolveHandle(handle, &handle) && data) {
						GetIdentifier(&data->arr.entries[i], handle, typeInfo, (*g_gameVM)->m_virtualMachine);
					}
				}

				val->SetComplexType(typeInfo);
				val->data.arr = data;
			}
			break;
		case VMValue::kType_StringArray:
		case VMValue::kType_IntArray:
		case VMValue::kType_FloatArray:
		case VMValue::kType_BoolArray:
			{
				UInt32 length = 0;
				if(!ReadData(intfc, &length))
					return false;

				VMValue::ArrayData * data = nullptr;
				vm->CreateArray(val, length, &data);

				val->type.value = typeId;

				for(UInt32 i = 0; i < length; i++)
				{
					switch(typeId) {
					case VMValue::kType_StringArray:
						{
							BSFixedString str;
							if(!ReadData(intfc, &str))
								return false;
							data->arr.entries[i].SetString(str);
						}
						break;
					case VMValue::kType_IntArray:
						{
							SInt32 i = 0;
							if(!ReadData(intfc, &i))
								return false;
							data->arr.entries[i].SetInt(i);
						}
						break;
					case VMValue::kType_FloatArray:
						{
							float f = 0.0f;
							if(!ReadData(intfc, &f))
								return false;
							data->arr.entries[i].SetFloat(f);
						}
						break;
					case VMValue::kType_BoolArray:
						{
							bool b = false;
							if(!ReadData(intfc, &b))
								return false;
							data->arr.entries[i].SetBool(b);
						}
						break;
					}
				}
			}
			break;
		case VMValue::kType_VariableArray:
			{
				UInt32 length = 0;
				if(!WriteData(intfc, &length))
					return false;

				VMValue::ArrayData * data = nullptr;
				vm->CreateArray(val, length, &data);

				val->type.value = typeId;

				for(UInt32 i = 0; i < length; i++)
				{
					if(!ReadVMData(intfc, &data->arr.entries[i]))
						return false;
				}
			}
			break;
		case VMValue::kType_StructArray:
			{
				UInt32 length = 0;
				if(!ReadData(intfc, &length))
					return false;

				BSFixedString structName;
				if(!ReadData(intfc, &structName))
					return false;

				UInt32 members = 0;
				if(!ReadData(intfc, &members))
					return false;

				VMStructTypeInfo * typeObject = nullptr;
				if(vm->GetStructTypeInfo(&structName, &typeObject))
					if(typeObject)
						typeObject->Release();

				if(typeObject)
					val->SetComplexType(reinterpret_cast<IComplexType*>((UInt64)typeObject | 1));

				// Entire array requires the type
				VMValue::ArrayData * data = nullptr;
				if(typeObject)
					vm->CreateArray(val, length, &data);

				for(UInt32 i = 0; i < length; i++)
				{
					VMValue::StructData * strct = nullptr;
					if(typeObject)
						vm->CreateStruct(&structName, &strct);

					for(UInt32 j = 0; j < members; j++)
					{
						BSFixedString member;
						if(!ReadData(intfc, &member))	// Read Key
							return false;

						VMValue data;
						if(!ReadVMData(intfc, &data))	// Read Value
							return false;

						// Set the member data if we both had the type and created the struct
						if(strct) {
							auto memberItem = typeObject->m_members.Find(&member);
							if(memberItem)
								strct->GetStruct()[memberItem->index] = data;
						}
					}

					if(typeObject) {
						data->arr.entries[i].SetComplexType(typeObject);
						data->arr.entries[i].data.strct = strct;
					}
				}
			}
			break;
		}
		return true;
	}
}
