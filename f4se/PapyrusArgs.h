#pragma once

#include "f4se/GameForms.h"
#include "f4se/GameTypes.h"
#include "f4se/PapyrusVM.h"
#include "f4se_common/Utilities.h"
#include "f4se/PapyrusStruct.h"

#include <vector>

class VMState;
class VMValue;
class VMArgList;

struct StaticFunctionTag;

class VMState
{
public:
	VMState();
	~VMState();

	VMArgList	* argList;					// 00
	UInt64		pad08[(0x38 - 0x08) >> 2];	// 08
	UInt32		numArgs;					// 38
};

class VMArgList
{
public:
	MEMBER_FN_PREFIX(VMArgList);
	DEFINE_MEMBER_FN(GetOffset, UInt32, 0x02704000, VMState * state);
	// 4C8A9FE3A95B9DED322C9C0A78312E29F0A2CC7A+5B
	DEFINE_MEMBER_FN(Get, VMValue *, 0x02704060, VMState * state, UInt32 idx, UInt32 offset);
};

template <typename T>
class VMArray
{
public:
	VMArray() : m_arr(nullptr), m_none(false) { }
	~VMArray() { }

	enum { kTypeID = 0 };

	VMArray(std::vector<T> & vec) : m_arr(nullptr), m_none(false)
	{
		for(auto & v : vec)
		{
			Push(&v);
		}
	}

	VMArray<T> & operator=(std::vector<T> & vec)
	{
		Clear();

		for(auto & v : vec)
		{
			Push(&v);
		}
		return *this;
	}

	UInt32 Length() const
	{
		return m_data.size();
	}
	void Get(T * dst, const UInt32 idx)
	{
		UnpackValue(dst, &m_data[idx]);
	}
	void Set(T * src, const UInt32 idx, bool bReference = true)
	{
		PackValue(&m_data[idx], src, (*g_gameVM)->m_virtualMachine);
		if(m_arr && bReference)
			PackValue(&m_arr->arr.entries[idx], src, (*g_gameVM)->m_virtualMachine);
	}
	void Push(T * src, bool bReference = true)
	{
		VMValue tmp;
		PackValue(&tmp, src, (*g_gameVM)->m_virtualMachine);
		m_data.push_back(tmp);
		if(m_arr && bReference)
		{
			m_arr->arr.Push(tmp);
		}
	}
	void Remove(const UInt32 idx, bool bReference = true)
	{
		m_data.erase(m_data.begin() + idx);
		if(m_arr && bReference)
		{
			m_arr->arr.Remove(idx);
		}
	}

	void Resize(const UInt32 size)
	{
		m_data.resize(size);
		if(m_arr)
			m_arr->arr.Resize(size);
	}

	void Clear()
	{
		m_data.clear();
		if(m_arr)
			m_arr->arr.Clear();
	}

	void PackArray(VMValue * dst, VirtualMachine * vm)
	{
		// Clear out old contents if any
		dst->SetNone();
		dst->type.value = GetTypeID<VMArray<T>>(vm); // Always set the type

		if(m_data.size() > 0 && !m_none)
		{
			VMValue::ArrayData * data = nullptr;
			// Request the VM allocate a new array
			vm->CreateArray(dst, m_data.size(), &data);
			if(data) {
				// Set the appropriate TypeID and assign the new data array
				dst->data.arr = data;

				// Copy from vector
				for(int i = 0; i < data->arr.count; ++i)
				{
					data->arr.entries[i] = m_data[i];
				}
			}
		}

		// Clear the temp contents of the reference array
		m_data.clear();
	}

	void UnpackArray(VMValue * src, const UInt64 type)
	{
		VMValue::ArrayData * arrData;

		if (src->type.value != type || (arrData = src->data.arr, !arrData))
		{
			m_none = true;
			m_arr = nullptr;
			return;
		}

		m_arr = arrData;

		m_data.resize(arrData->arr.count);
		for(int i = 0; i < arrData->arr.count; ++i)
		{
			// Copy into vector
			m_data[i] = arrData->arr.entries[i];
		}
	}

	// Will make the VM return None instead of a zero sized array
	void SetNone(bool bNone) { m_none = bNone; }
	bool IsNone() const { return m_none; }

protected:
	VMValue::ArrayData			* m_arr;	// Original reference
	std::vector<VMValue>		m_data;		// Temporary copies
	bool						m_none;
};

class VMVariable
{
public:
	VMVariable() : m_var(nullptr) { }
	~VMVariable() {  }

	template<typename T>
	void Set(T * src, bool bReference = true)
	{
		PackValue(&m_value, src, (*g_gameVM)->m_virtualMachine);
		if(m_var && bReference)
			PackValue(m_var, src, (*g_gameVM)->m_virtualMachine);
	}

	// Fails on invalid type unpack
	template<typename T>
	bool Get(T * dst)
	{
		if(Is<T>())
		{
			UnpackValue(dst, &m_value);
			return true;
		}

		return false;
	}

	// Skips type-check
	template<typename T>
	T As()
	{
		T tmp;
		UnpackValue(&tmp, &m_value);
		return tmp;
	}

	template<typename T>
	bool Is()
	{
		return m_value.type.value == GetTypeID<T>((*g_gameVM)->m_virtualMachine);
	}

	void PackVariable(VMValue * dst)
	{
		VMValue * newValue = new VMValue(m_value);
		dst->SetVariable(newValue);
	}

	void UnpackVariable(VMValue * value)
	{
		m_var = value->data.var;
		m_value = *m_var;
	}

	bool IsNone()
	{
		return m_value.GetTypeEnum() == 0;
	}

	// Provides direct access to the VM data, advanced use only
	VMValue & GetValue()
	{
		return m_value;
	}

protected:
	VMValue			* m_var;	// Original reference
	VMValue			m_value;	// Copied data
};

class VMObject
{
public:
	enum { kTypeID = -1 };

	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free);

	template<typename T>
	void Set(T * src)
	{
		PackValue(&m_value, src, (*g_gameVM)->m_virtualMachine);
	}

	template<typename T>
	bool Get(T * dst)
	{
		if(Is<T>())
		{
			UnpackValue(dst, &m_value);
			return true;
		}

		return false;
	}

	template<typename T>
	bool Is()
	{
		if(m_value.IsIdentifier() && m_value.data.id) {
			UInt64	handle = m_value.data.id->GetHandle();

			typedef std::remove_pointer <T>::type	BaseType;
			return (*g_objectHandlePolicy)->IsType(BaseType::kTypeID, handle);
		}

		return false;
	}

	UInt64 GetHandle()
	{
		if(m_value.IsIdentifier() && m_value.data.id) {
			return m_value.data.id->GetHandle();
		}

		return (*g_objectHandlePolicy)->GetInvalidHandle();
	}

	BSFixedString GetObjectType()
	{
		if(m_value.IsIdentifier() && m_value.data.id && m_value.data.id->m_typeInfo) {
			VMIdentifier * id = m_value.data.id;
			if(id) {
				IComplexType * type = m_value.data.id->m_typeInfo;
				if(type)
					return type->m_typeName;
			}
		}

		return BSFixedString("ScriptObject");
	}

	void PackObject(VMValue * dst)
	{
		*dst = m_value;
	}

	void UnpackObject(VMValue * value)
	{
		m_value = *value;
	}

protected:
	VMValue		m_value;
};

class VMRefOrInventoryObj
{
public:
	enum { kTypeID = kFormType_REFR };

	TESObjectREFR * GetObjectReference();
	TESObjectREFR * GetOwner();
	bool GetExtraData(TESForm ** baseForm, ExtraDataList ** extraData);

	void PackObject(VMValue * dst)
	{
		*dst = m_value;
	}

	void UnpackObject(VMValue * value)
	{
		if(value->IsIdentifier() && value->data.id) {
			UInt64	handle = value->data.id->GetHandle();
			GetRefFromHandle(&m_refData, handle);
		} else {
			m_refData.owner = nullptr;
			m_refData.refr = nullptr;
			m_refData.uniqueId = 0;
		}
		m_value = *value;
	}

protected:
	VMValue     m_value;
	VMRefHandle m_refData;
};

template <typename T>
void UnpackValue(VMArray<T> * dst, VMValue * src)
{
	UnpackArray(dst, src, GetTypeID<VMArray<T>>((*g_gameVM)->m_virtualMachine));
}

template <typename T>
void PackValue(VMValue * dst, T * src, VirtualMachine * vm);

template <typename T>
void PackValue(VMValue * dst, VMArray<T> * src, VirtualMachine * vm)
{
	src->PackArray(dst, vm);
}

template <typename T>
void UnpackValue(T * dst, VMValue * src);

template <typename T>
UInt64 GetTypeID(VirtualMachine * vm);

template <> void PackValue <void>(VMValue * dst, void * src, VirtualMachine * registry);
template <> void PackValue <UInt32>(VMValue * dst, UInt32 * src, VirtualMachine * registry);
template <> void PackValue <SInt32>(VMValue * dst, SInt32 * src, VirtualMachine * registry);
template <> void PackValue <float>(VMValue * dst, float * src, VirtualMachine * vm);
template <> void PackValue <bool>(VMValue * dst, bool * src, VirtualMachine * vm);
template <> void PackValue <BSFixedString>(VMValue * dst, BSFixedString * src, VirtualMachine * vm);
template <> void PackValue <VMVariable>(VMValue * dst, VMVariable * src, VirtualMachine * vm);
template <> void PackValue <VMObject>(VMValue * dst, VMObject * src, VirtualMachine * vm);
template <> void PackValue <VMRefOrInventoryObj>(VMValue * dst, VMRefOrInventoryObj * src, VirtualMachine * vm);

void PackHandle(VMValue * dst, void * src, UInt32 typeID, VirtualMachine * registry);

bool GetIdentifier(VMValue * dst, UInt64 handle, VMObjectTypeInfo * typeInfo, VirtualMachine * vm);
bool GetIdentifier(VMValue * dst, UInt64 handle, const BSFixedString * typeName, VirtualMachine * vm);

template <typename T>
void PackValue(VMValue * dst, T ** src, VirtualMachine * vm)
{
	typedef std::remove_pointer <T>::type	BaseType;
	PackHandle(dst, *src, BaseType::kTypeID, vm);
}
template <> void PackValue <VMRefOrInventoryObj>(VMValue * dst, VMRefOrInventoryObj ** src, VirtualMachine * vm);

template <> void UnpackValue <float>(float * dst, VMValue * src);
template <> void UnpackValue <UInt32>(UInt32 * dst, VMValue * src);
template <> void UnpackValue <SInt32>(SInt32 * dst, VMValue * src);
template <> void UnpackValue <bool>(bool * dst, VMValue * src);
template <> void UnpackValue <BSFixedString>(BSFixedString * dst, VMValue * src);

template <> void UnpackValue <VMArray<float>>(VMArray<float> * dst, VMValue * src);
template <> void UnpackValue <VMArray<UInt32>>(VMArray<UInt32> * dst, VMValue * src);
template <> void UnpackValue <VMArray<SInt32>>(VMArray<SInt32> * dst, VMValue * src);
template <> void UnpackValue <VMArray<bool>>(VMArray<bool> * dst, VMValue * src);
template <> void UnpackValue <VMArray<BSFixedString>>(VMArray<BSFixedString> * dst, VMValue * src);
template <> void UnpackValue <VMArray<VMVariable>>(VMArray<VMVariable> * dst, VMValue * src);
template <> void UnpackValue <VMArray<VMObject>>(VMArray<VMObject> * dst, VMValue * src);

void * UnpackHandle(VMValue * src, UInt32 typeID);

template <typename T>
void UnpackValue(T ** dst, VMValue * src)
{
	*dst = (T *)UnpackHandle(src, T::kTypeID);
}

template <typename T>
void DestroyValue(T ** dst)
{
	// Dummy implementation, used by VMObject to destroy the temp pointer
}

template <> void UnpackValue(VMObject ** dst, VMValue * src);
template <> void DestroyValue(VMObject ** dst);

template <> void UnpackValue(VMRefOrInventoryObj ** dst, VMValue * src);
template <> void DestroyValue(VMRefOrInventoryObj ** dst);

template <typename T>
void UnpackArray(VMArray<T> * dst, VMValue * src, const UInt64 type)
{
	dst->UnpackArray(src, type);
}

UInt64 GetTypeIDFromFormTypeID(UInt32 formTypeID, VirtualMachine * vm);
UInt64 GetTypeIDFromStructName(const char * name, VirtualMachine * vm);

template <> UInt64 GetTypeID <void>(VirtualMachine * vm);
template <> UInt64 GetTypeID <UInt32>(VirtualMachine * vm);
template <> UInt64 GetTypeID <SInt32>(VirtualMachine * vm);
template <> UInt64 GetTypeID <int>(VirtualMachine * vm);
template <> UInt64 GetTypeID <float>(VirtualMachine * vm);
template <> UInt64 GetTypeID <bool>(VirtualMachine * vm);
template <> UInt64 GetTypeID <BSFixedString>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMVariable>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMObject>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMRefOrInventoryObj>(VirtualMachine * vm);

template <> UInt64 GetTypeID <VMArray<UInt32>>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMArray<SInt32>>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMArray<int>>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMArray<float>>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMArray<bool>>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMArray<BSFixedString>>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMArray<VMVariable>>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMArray<VMObject>>(VirtualMachine * vm);
template <> UInt64 GetTypeID <VMArray<VMRefOrInventoryObj>>(VirtualMachine * vm);

template <typename T>
struct IsObjectType
{
	static const bool value = false;
};

template<>
struct IsObjectType<VMObject>
{
	static const bool value = true;
};


template<typename T>
struct IsArrayType
{
	enum { value = 0 };
	typedef T TypedArg;
};

template<typename T>
struct IsArrayType<VMArray<T>>
{
	enum { value = 1 };
	typedef T TypedArg;
};

template <typename T>
UInt64 GetTypeID <T>(VirtualMachine * vm)
{
	UInt64		result;

	if(IsArrayType<T>::value)
	{
		typedef IsArrayType<T>::TypedArg BaseType;
		if(IsStructType<BaseType>::value)
		{
			result = GetTypeIDFromStructName(IsStructType<BaseType>::name(), vm) | VMValue::kType_Identifier;
		}
		else if(std::is_pointer<BaseType>::value)
		{
			typedef std::remove_pointer <BaseType>::type	ObjectType;
			result = GetTypeIDFromFormTypeID(ObjectType::kTypeID, vm) | VMValue::kType_Identifier;
		}
	}
	else if(IsStructType<T>::value)
	{
		result = GetTypeIDFromStructName(IsStructType<T>::name(), vm);
	}
	else if(std::is_pointer<T>::value)
	{
		typedef std::remove_pointer <T>::type	ObjectType;
		result = GetTypeIDFromFormTypeID(ObjectType::kTypeID, vm);
	}

	return result;
}

template <class T>
struct IsStaticType
{
	enum { value = 0 };
};

template <>
struct IsStaticType <StaticFunctionTag>
{
	enum { value = 1 };
};
