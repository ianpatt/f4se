#include "f4se/PapyrusScaleformAdapter.h"

#include "f4se/GameTypes.h"

#include "f4se/PapyrusValue.h"
#include "f4se/PapyrusArgs.h"
#include "f4se/ScaleformValue.h"

#include "f4se/PapyrusVM.h"
#include "f4se/ScaleformMovie.h"

namespace PlatformAdapter
{
	bool ConvertPapyrusValue(GFxValue * dest, VMValue * src, GFxMovieRoot * root)
	{
		switch(src->GetTypeEnum())
		{
		case VMValue::kType_String:
			{
				BSFixedString * str = src->data.GetStr();
				root->CreateString(dest, str->c_str());
				return true;
			}
			break;
		case VMValue::kType_Int:
			dest->SetInt(src->data.i);
			return true;
			break;
		case VMValue::kType_Float:
			dest->SetNumber(src->data.f);
			return true;
			break;
		case VMValue::kType_Bool:
			dest->SetBool(src->data.b);
			return true;
			break;
		case VMValue::kType_Identifier:
			{
				VMObjectTypeInfo * objectType = static_cast<VMObjectTypeInfo*>(src->GetComplexType());
				VMIdentifier * id = src->data.id;
				if(objectType && id) {
					UInt64 handle = id->GetHandle();
					BSFixedString typeName = objectType->m_typeName;

					root->CreateObject(dest);
					dest->SetMember("__handleHigh__", &GFxValue((UInt32)(handle >> 32)));
					dest->SetMember("__handleLow__", &GFxValue((UInt32)(handle & 0xFFFFFFFF)));

					GFxValue type;
					root->CreateString(&type, typeName.c_str());

					dest->SetMember("__type__", &type);
					return true;
				}
			}
			break;
		case VMValue::kType_Struct:
			{
				VMStructTypeInfo * structType = static_cast<VMStructTypeInfo*>(src->GetComplexType());
				VMValue::StructData * structData = src->data.strct;
				if(structType && structData) {
					root->CreateObject(dest);
					GFxValue gStructObject;
					root->CreateObject(&gStructObject);
					dest->SetMember("__struct__", &gStructObject);

					GFxValue gStructName;
					root->CreateString(&gStructName, structType->m_typeName.c_str());
					gStructObject.SetMember("__type__", &gStructName);

					GFxValue gStructPairs;
					root->CreateObject(&gStructPairs);
					gStructObject.SetMember("__data__", &gStructPairs);

					structType->m_members.ForEach([&gStructPairs, &structData, &root](VMStructTypeInfo::MemberItem * item)
					{
						GFxValue value;
						ConvertPapyrusValue(&value, &structData->GetStruct()[item->index], root);
						gStructPairs.SetMember(item->name.c_str(), &value);
						return true;
					});
					return true;
				}
			}
			break;
		case VMValue::kType_Variable:
			{
				VMValue * source = src->data.var;
				if(source) {
					root->CreateObject(dest);
					GFxValue type;
					ConvertPapyrusValue(&type, source, root);
					dest->SetMember("__var__", &type);
					return true;
				}
			}
			break;
		case VMValue::kType_StringArray:
		case VMValue::kType_IntArray:
		case VMValue::kType_FloatArray:
		case VMValue::kType_BoolArray:
		case VMValue::kType_IdentifierArray:
		case VMValue::kType_StructArray:
		case VMValue::kType_VariableArray:
			{
				VMValue::ArrayData * arrayData = src->data.arr;
				if(arrayData) {
					root->CreateArray(dest);

					for(UInt32 i = 0; i < arrayData->arr.count; i++) {
						VMValue vmValue;
						arrayData->arr.GetNthItem(i, vmValue);

						GFxValue value;
						ConvertPapyrusValue(&value, &vmValue, root);
						dest->PushBack(&value);
					}

					return true;
				}
			}
			break;
		}

		return false;
	}

	class StructMemberVisitor : public GFxValue::ObjectInterface::ObjVisitor
	{
	public:
		StructMemberVisitor(VirtualMachine * _vm, VMStructTypeInfo * _typeObject, VMValue::StructData * _strct) : vm(_vm), typeObject(_typeObject), strct(_strct) { }

		virtual void Visit(const char * member, GFxValue * value) override
		{
			if(strct)
			{
				BSFixedString memberName(member);
				auto memberItem = typeObject->m_members.Find(&memberName);
				if(memberItem) {
					VMValue * structValues = strct->GetStruct();
					VMValue data;

					// Must be the same type, otherwise we are type clobbering!
					if(ConvertScaleformValue(&data, value, vm) && data.type.value == structValues[memberItem->index].type.value) {
						structValues[memberItem->index] = data;
					}
				}
			}
		}
		VirtualMachine * vm;
		VMStructTypeInfo * typeObject;
		VMValue::StructData * strct;
	};

	bool ConvertScaleformValue(VMValue * dest, GFxValue * src, VirtualMachine * vm)
	{
		switch(src->GetType())
		{
		case GFxValue::kType_Null:
		case GFxValue::kType_Undefined:
			dest->SetVariable(new VMValue); // Var to None
			return true;
			break;
		case GFxValue::kType_Int:
		case GFxValue::kType_UInt:
			dest->SetInt(src->GetInt());
			return true;
			break;
		case GFxValue::kType_Number:
			dest->SetFloat(src->GetNumber());
			return true;
			break;
		case GFxValue::kType_String:
			dest->SetString(src->GetString());
			return true;
			break;
		case GFxValue::kType_Bool:
			dest->SetBool(src->GetBool());
			return true;
			break;
		case GFxValue::kType_Object:
			{
				if(src->HasMember("__var__"))
				{
					VMValue * value = new VMValue;

					GFxValue gValue;
					src->GetMember("__var__", &gValue);

					ConvertScaleformValue(value, &gValue, vm);

					dest->SetVariable(value);
					return true;
				}
				else if(src->HasMember("__type__"))
				{
					UInt64 handle = 0;
					GFxValue gType;
					src->GetMember("__type__", &gType);
					GFxValue gHandle;
					src->GetMember("__handleHigh__", &gHandle);
					handle |= ((UInt64)gHandle.GetUInt()) << 32;
					src->GetMember("__handleLow__", &gHandle);
					handle |= (UInt64)gHandle.GetUInt();

					BSFixedString typeName(gType.GetString());

					if(GetIdentifier(dest, handle, &typeName, vm))
					{
						return true;
					}
				}
				else if(src->HasMember("__struct__"))
				{
					GFxValue gStructObject;
					src->GetMember("__struct__", &gStructObject);

					GFxValue gTypeName;
					gStructObject.GetMember("__type__", &gTypeName);

					GFxValue gStructData;
					gStructObject.GetMember("__data__", &gStructData);

					BSFixedString structName(gTypeName.GetString());
					VMStructTypeInfo * typeObject = nullptr;
					if(vm->GetStructTypeInfo(&structName, &typeObject))
						if(typeObject)
							typeObject->Release();

					dest->SetComplexType(typeObject);

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
						dest->data.strct = strct;
					}

					StructMemberVisitor visitor(vm, typeObject, strct);
					gStructData.VisitMembers(&visitor);
					return true;
				}
				else
				{
					// Our object isn't valid, just make it a var to None
					dest->SetVariable(new VMValue);
				}
			}
			break;
		case GFxValue::kType_Array:
			{
				UInt32 length = src->GetArraySize();
				VMValue::ArrayData * arrayData = nullptr;
				vm->CreateArray(dest, length, &arrayData);

				dest->type.value = VMValue::kType_VariableArray;
				dest->data.arr = arrayData;

				for(UInt32 i = 0; i < length; i++)
				{
					GFxValue value;
					src->GetElement(i, &value);

					VMValue * var = new VMValue;
					ConvertScaleformValue(var, &value, vm);
					arrayData->arr.entries[i].SetVariable(var);
				}

				return true;
			}
			break;
		}

		return false;
	}
};
