#include "f4se/PapyrusValue.h"
#include "f4se/PapyrusInterfaces.h"

VMValue::VMValue(const VMValue & other)
{
	if(&other != this)
	{
		type.value = kType_None;
		data.p = nullptr;
		CALL_MEMBER_FN(this, Set)(&other);
	}
}
VMValue& VMValue::operator=(const VMValue& other)
{
	if(&other == this)
		return *this;

	CALL_MEMBER_FN(this, Set)(&other);
	return *this;
}

void VMValue::SetNone(void)
{
	CALL_MEMBER_FN(this, Destroy)();

	type.value = kType_None;
	data.u = 0;
}

void VMValue::SetInt(SInt32 i)
{
	CALL_MEMBER_FN(this, Destroy)();

	type.value = kType_Int;
	data.i = i;
}

void VMValue::SetFloat(float f)
{
	CALL_MEMBER_FN(this, Destroy)();

	type.value = kType_Float;
	data.f = f;
}

void VMValue::SetBool(bool b)
{
	CALL_MEMBER_FN(this, Destroy)();

	type.value = kType_Bool;
	data.b = b;
}


void VMValue::SetString(const char * str)
{
	CALL_MEMBER_FN(this, Destroy)();

	type.value = kType_String;
	CALL_MEMBER_FN(data.GetStr(), Set)(str);
}

void VMValue::SetVariable(VMValue * value)
{
	CALL_MEMBER_FN(this, Destroy)();

	type.value = kType_Variable;
	data.var = value;
}

void VMValue::SetComplexType(IComplexType * typeInfo)
{
	CALL_MEMBER_FN(this, Destroy)();

	type.id = typeInfo;
	data.p = nullptr;
}

void VMValue::SetIdentifier(VMIdentifier	** identifier)
{
	IComplexType * typeInfo = GetComplexType();
	if(typeInfo && typeInfo->GetType() == kType_Identifier)
	{
		CALL_MEMBER_FN(this, Destroy)();

		if(*identifier)
			(*identifier)->IncrementLock();

		data.id = *identifier;
	}
}

bool VMValue::IsIntegralType() const
{
	return type.value >= kType_String && type.value <= kType_Variable;
}

bool VMValue::IsIntegralArrayType() const
{
	return type.value >= kType_IntegralStart && type.value <= kType_IntegralEnd;
}

bool VMValue::IsComplexArrayType() const
{
	return (IsComplexType() && (type.value & 0x01LL));
}

bool VMValue::IsArrayType() const
{
	return IsIntegralArrayType() || IsComplexArrayType();
}

bool VMValue::IsComplexType() const
{
	return type.value >= kType_ArrayEnd;
}

bool VMValue::IsIdentifier()
{
	IComplexType * typeInfo = GetComplexType();
	return typeInfo ? typeInfo->GetType() == kType_Identifier : false;
}

IComplexType * VMValue::GetComplexType()
{
	return IsComplexType() ? reinterpret_cast<IComplexType *>(type.value & ~0x01LL) : nullptr;
}

IComplexType * VMValue::GetComplexType() const
{
	return IsComplexType() ? reinterpret_cast<IComplexType *>(type.value & ~0x01LL) : nullptr;
}

UInt8 VMValue::GetTypeEnum() const // Returns the sanitized number
{
	IComplexType * typeInfo = GetComplexType();
	if(typeInfo)
	{
		UInt32 typeId = typeInfo->GetType();
		if(IsArrayType())
			typeId += kType_ArrayOffset;
		return typeId;
	}

	return type.value;
}

UInt64 VMIdentifier::GetHandle(void)
{
	UInt64	result = (*g_objectHandlePolicy)->GetInvalidHandle();

	SInt32	oldLock = Lock();
	result = m_handle;
	Unlock(oldLock);

	return result;
}

SInt32 VMIdentifier::Lock(void)
{
	UInt32	spinCount = 0;
	SInt32	lockValue;

	while(true)
	{
		lockValue = m_lock;

		if(lockValue & kLockBit)
		{
			// someone else is holding the lock, sleep
			if(spinCount <= kFastSpinThreshold)
			{
				Sleep(0);
				spinCount++;
			}
			else
			{
				Sleep(1);
			}
		}
		else
		{
			// try to take the lock
			if(InterlockedCompareExchange(&m_lock, lockValue | kLockBit, lockValue) == lockValue)
				break;
		}
	}

	return lockValue;
}

void VMIdentifier::Unlock(SInt32 oldLock)
{
	m_lock = oldLock;
}

// try to increment the lock
void VMIdentifier::IncrementLock(void)
{
	UInt32	spinCount = 0;
	
	while(true)
	{
		SInt32	lockValue = m_lock;

		if(lockValue & kLockBit)
		{
			if(spinCount <= kFastSpinThreshold)
			{
				Sleep(0);
				spinCount++;
			}
			else
			{
				Sleep(1);
			}
		}
		else
		{
			if(lockValue == 1)
			{
				if(InterlockedCompareExchange(&m_lock, lockValue | kLockBit, lockValue) == lockValue)
				{
					(*g_objectHandlePolicy)->AddRef(m_handle);
					m_lock = 2;
					break;
				}
			}
			else
			{
				if(InterlockedCompareExchange(&m_lock, lockValue + 1, lockValue) == lockValue)
					break;
			}
		}
	}
}

// try to decrement the lock
SInt32 VMIdentifier::DecrementLock(void)
{
	UInt32	spinCount = 0;

	while(true)
	{
		SInt32	lockValue = m_lock;

		if(lockValue & kLockBit)
		{
			if(spinCount <= kFastSpinThreshold)
			{
				Sleep(0);
				spinCount++;
			}
			else
			{
				Sleep(1);
			}
		}
		else
		{
			if(lockValue == 2)
			{
				if(InterlockedCompareExchange(&m_lock, lockValue | kLockBit, lockValue) == lockValue)
				{
					(*g_objectHandlePolicy)->Release(m_handle);
					m_lock = 1;
					return 1;
				}
			}
			else
			{
				if(InterlockedCompareExchange(&m_lock, lockValue - 1, lockValue) == lockValue)
					return lockValue - 1;
			}
		}
	}
}

void VMIdentifier::Destroy(void)
{
	CALL_MEMBER_FN(this, Destroy_Internal)();
	Heap_Free(this);
}

UInt64 GetArrayType(UInt64 type)
{
	if(type > VMValue::kType_StructArray)
	{
		return type & ~0x01;
	}
	else if(type > VMValue::kType_IntegralStart)
	{
		return type - VMValue::kType_ArrayOffset;
	}

	return type;
}
