#pragma once

#include "f4se/PapyrusArgs.h"

bool CreateStruct(VMValue * dst, BSFixedString * structName, VirtualMachine * vm, bool bNone);

template<const char* T_structName>
class VMStruct
{
public:
	VMStruct() : m_none(false), m_struct(nullptr) { };
	~VMStruct() { }

	enum { kTypeID = 0 };

	// Will make the VM return None instead of a structure with nothing defined
	void SetNone(bool bNone) { m_none = bNone; }
	bool IsNone() const { return m_none; }

	template<typename T>
	bool Get(BSFixedString name, T * value)
	{
		BSFixedString structName(T_structName);
		VMStructTypeInfo * typeInfo = nullptr;

		VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;
		if(vm->GetStructTypeInfo(&structName, &typeInfo))
		{
			VMStructTypeInfo::MemberItem * item = typeInfo->m_members.Find(&name);
			if(item) {
				UnpackValue(value, &m_data[item->name]);
				typeInfo->Release();
				structName.Release();
				return true;
			}
#if _DEBUG
			else {

				_DMESSAGE("Failed to unpack %s argument (%s) struct member not found.", structName.c_str(), name.c_str());
			}
#endif
			typeInfo->Release();
		}

		structName.Release();
		return false;
	};

	template<typename T>
	bool Set(BSFixedString name, T a1, bool bReference = true)
	{ 
		BSFixedString structName(T_structName);
		VMStructTypeInfo * typeInfo = nullptr;
		VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;
		if(vm->GetStructTypeInfo(&structName, &typeInfo))
		{
			VMStructTypeInfo::MemberItem * item = typeInfo->m_members.Find(&name);
			if(item) {
				if(m_struct && bReference) {
					VMValue * value = m_struct->GetStruct();
					PackValue(&value[item->index], &a1, vm);
				}

				PackValue(&m_data[item->name], &a1, vm);

				typeInfo->Release();
				structName.Release();
				return true;
			}
#if _DEBUG
			else {

				_DMESSAGE("Failed to pack %s argument (%s) struct member not found.", structName.c_str(), name.c_str());
			}
#endif

			typeInfo->Release();
		}

		structName.Release();
		return false;
	}

	void PackStruct(VMValue * dst, VirtualMachine * vm)
	{
		// Clean out the old value
		dst->SetNone();

		BSFixedString structName(T_structName);
		VMStructTypeInfo * typeInfo = nullptr;
		if(vm->GetStructTypeInfo(&structName, &typeInfo))
		{
			if(CreateStruct(dst, &structName, vm, m_none))
			{
				VMValue * values = dst->data.strct->GetStruct();

				typeInfo->m_members.ForEach([&](VMStructTypeInfo::MemberItem * item)
				{
					UInt64 memberType = typeInfo->m_data[item->index].m_type;
					if(memberType == m_data[item->name].type.value)
					{
						values[item->index] = m_data[item->name];
					}
#if _DEBUG
					else {

						_DMESSAGE("Failed to pack %s argument (%s) struct member type mismatch got (%016I64X) expected (%016I64X).", structName.c_str(), item->name.c_str(), m_data[item->name].type, memberType);
					}
#endif

					values[item->index].type.value = memberType; // Always force the type so that we don't get None types on struct
					return true;
				});
			}

			typeInfo->Release();
		}

		structName.Release();
	}

	void UnpackStruct(VMValue * src)
	{
		IComplexType * complexType = src->GetComplexType();
		BSFixedString structName(T_structName);
		VMStructTypeInfo * typeInfo = nullptr;
		VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;
		if(vm->GetStructTypeInfo(&structName, &typeInfo) && complexType == typeInfo)
		{
			if(src->data.strct)
			{
				VMValue * values = src->data.strct->GetStruct();

				typeInfo->m_members.ForEach([&](VMStructTypeInfo::MemberItem * item)
				{
					m_data[item->name] = values[item->index];
					return true;
				});

				m_struct = src->data.strct;
			}
			else
			{
				typeInfo->m_members.ForEach([&](VMStructTypeInfo::MemberItem * item)
				{
					m_data[item->name].type.value = typeInfo->m_data[item->index].m_type;
					return true;
				});
				m_none = true;
			}

			typeInfo->Release();
		}

		structName.Release();
	}

protected:
	bool m_none;
	VMValue::StructData * m_struct;
	std::map<BSFixedString, VMValue> m_data;
};

template <class T>
struct IsStructType
{
	static const bool value = false;
	static inline const char* name() { return nullptr; };
};

template <const char * NAME>
struct IsStructType<VMStruct<NAME>>
{
	static const bool value = true;
	static inline const char* name() { return NAME; };
};

#define DECLARE_STRUCT_STRING(structName) #structName
#define DECLARE_STRUCT(structName, scriptOwner) \
	char StructName_##structName##[] = ##scriptOwner## "#" DECLARE_STRUCT_STRING(structName); \
	typedef VMStruct<StructName_##structName##> ##structName##;
#define DECLARE_EXTERN_STRUCT(structName) \
	extern char StructName_##structName##[]; \
	typedef VMStruct<StructName_##structName##> ##structName##;

template <const char * T>
void PackValue(VMValue * dst, VMStruct<T> * src, VirtualMachine * vm)
{
	src->PackStruct(dst, vm);
}


template <const char * T>
void UnpackValue(VMStruct<T> * dst, VMValue * src)
{
	dst->UnpackStruct(src);
}
