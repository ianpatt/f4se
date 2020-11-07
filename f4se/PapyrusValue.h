#pragma once

#include "f4se_common/Utilities.h"
#include "f4se/GameTypes.h"
#include "f4se/GameUtilities.h"

#include "f4se/PapyrusInterfaces.h"

class VMIdentifier;

// 58
class VMObjectTypeInfo : public IComplexType
{
public:
	virtual ~VMObjectTypeInfo();

	virtual UInt32	GetType();	// returns 1 (kType_Identifier)

	void				* unk20;	// 20
	UInt64				unk28;		// 28
	UInt64				unk30;		// 30
	UInt64				unk38;		// 38
	struct MemberData
	{
		unsigned unk00 : 3; // This == 3 is usually always checked before accessing properties
		unsigned unk03 : 5;
		unsigned numMembers : 10; // Variables + Properties
		unsigned unk19 : 14;
	} memberData;
	struct PropertyData
	{
		unsigned numVariables : 10; // Sometimes this is 0 and member != numProperties
		unsigned numProperties : 10; // Excludes variables
		unsigned unk21: 12;
	} propertyData;
	UInt32				numFunc;	// 48
	UInt32				unk4C;		// 4C

	struct PropertyElement
	{
		BSFixedString		propertyName;	// 00

		union // Can be number or IComplexType or IComplexType | 1 (array)
		{
			UInt64			value;
			IComplexType	* id;
		} type;								// 08
		UInt64	unk10;						// 10 - Not sure what this is, maybe a hash?
	};

	struct Properties
	{
		BSFixedString		unk00;
		BSFixedString		unk08;
		BSFixedString		unk10;
		BSFixedString		unk18;
		BSFixedString		unk20;
		BSFixedString		unk28;
		PropertyElement		defs[0];
	};
	Properties			* properties;		// 50
};
STATIC_ASSERT(offsetof(VMObjectTypeInfo, memberData) == 0x40);
STATIC_ASSERT(offsetof(VMObjectTypeInfo, propertyData) == 0x44);
STATIC_ASSERT(offsetof(VMObjectTypeInfo, properties) == 0x50);

// 70
class VMStructTypeInfo : public IComplexType
{
public:
	virtual ~VMStructTypeInfo();

	virtual UInt32	GetType();	// returns 7 (kType_Struct)

	struct StructData
	{
		UInt64	unk00;		// 00
		UInt64	unk08;		// 08
		UInt64	m_type;		// 10
		void	* unk18;	// 18
		UInt64	unk20;		// 20
	};
	tArray<StructData>	m_data;		// 20

	class MemberItem
	{
	public:
		BSFixedString		name;			// 00
		UInt32				index;			// 08

		bool operator==(const MemberItem & rhs) const	{ return name == rhs.name; }
		bool operator==(const BSFixedString a_name) const	{ return name == a_name; }
		operator UInt64() const								{ return (UInt64)name.data->Get<char>(); }

		static inline UInt32 GetHash(BSFixedString * key)
		{
			UInt32 hash;
			CalculateCRC32_64(&hash, (UInt64)key->data, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\tname: %s", name.data->Get<char>());
			_MESSAGE("\t\tinstance: %08X", index);
		}
	};

	tHashSet<MemberItem, BSFixedString> m_members;
};

// 10
class VMValue
{
public:
	VMValue() { type.value = kType_None; data.p = nullptr; }
	~VMValue() { CALL_MEMBER_FN(this, Destroy)(); }

	VMValue(const VMValue & other);
	VMValue& operator=(const VMValue& other);

	enum
	{
		kType_None			= 0,
		kType_Identifier	= 1,	// Number not actually used by VMValue
		kType_String		= 2,
		kType_Int			= 3,
		kType_Float			= 4,
		kType_Bool			= 5,
		kType_Variable		= 6,	// Points to a VMValue
		kType_Struct		= 7,	// Number not actually used by VMValue

		kType_ArrayOffset		= 10,

		kType_IdentifierArray	= 11,
		kType_StringArray		= 12,
		kType_IntArray			= 13,
		kType_FloatArray		= 14,
		kType_BoolArray			= 15,
		kType_VariableArray		= 16,
		kType_StructArray		= 17,	// Number not actually used by VMValue
		kType_ArrayEnd			= 18,

		kType_IntegralStart		= kType_StringArray,
		kType_IntegralEnd		= kType_VariableArray,
	};

	struct ArrayData
	{
		SInt32				m_refCount;	// 00
		UInt32				unk04;		// 04
		UInt64				m_type;		// 08 - base types 1-6
		UInt64				unk10;		// 10
		tArray<VMValue>		arr;	// 18

		MEMBER_FN_PREFIX(ArrayData);
		DEFINE_MEMBER_FN(Destroy, void, 0x02718050);
	};

	struct StructData
	{
		SInt32				m_refCount;	// 00
		UInt32				unk04;		// 04
		UInt64				unk08;		// 08
		VMStructTypeInfo	* m_type;	// 10
		UInt8				unk18;		// 18 - set to 1 if unk19 is 1 (3EFCF27952D674A8FA959AABC29A0FE3E726FA91)
		UInt8				unk19;		// 19 - set to 1 when type+0x68 == 3
		UInt16				unk1A;		// 1A
		UInt32				unk1C;		// 1C
		char				m_value[0];	// 20

		VMValue * GetStruct() { return (VMValue *)&m_value[0]; }
	};

	union // Can be number or IComplexType or IComplexType | 1 (array)
	{
		UInt64			value;
		IComplexType	* id;
	} type;
	
	union
	{
		SInt32				i;
		UInt32				u;
		float				f;
		bool				b;
		void				* p;
		StructData			* strct;
		ArrayData			* arr;
		VMValue				* var;
		VMIdentifier		* id;
		StringCache::Entry	* str;
		BSFixedString *	GetStr(void)		{ return (BSFixedString *)(&str); }
		BSFixedString *	GetStr(void) const	{ return (BSFixedString *)(&str); }
	} data;	

	void	SetNone(void);
	void	SetInt(SInt32 i);
	void	SetFloat(float f);
	void	SetBool(bool b);
	void	SetString(const char * str);

	void	SetVariable(VMValue * value);
	void	SetComplexType(IComplexType * typeInfo);
	void	SetIdentifier(VMIdentifier	** identifier);

	bool	IsIntegralType() const;
	bool	IsIntegralArrayType() const;
	bool	IsComplexArrayType() const;
	bool	IsArrayType() const;
	bool	IsComplexType() const;
	bool	IsIdentifier();

	IComplexType * GetComplexType();
	IComplexType * GetComplexType() const;

	UInt8 GetTypeEnum() const;

	MEMBER_FN_PREFIX(VMValue);
	DEFINE_MEMBER_FN(Set, void, 0x026F0FB0, const VMValue * src);
	DEFINE_MEMBER_FN(Destroy, void, 0x026F0960);
	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)
};

// 30 - Sized based on number of properties
class VMIdentifier
{
public:
	enum
	{
		kLockBit = 0x80000000,
		kFastSpinThreshold = 10000
	};

	SInt32				m_refCount;		// 00
	UInt32				unk04;			// 04
	VMObjectTypeInfo	* m_typeInfo;	// 08
	void				* unk10;		// 10
	UInt64				unk18;			// 18
	volatile UInt64		m_handle;		// 20
	volatile SInt32		m_lock;			// 28
	UInt32				unk2C;			// 2C
	VMValue				properties[0];	// 30

	UInt64	GetHandle(void);

	SInt32	Lock(void);
	void	Unlock(SInt32 oldLock);

	// lock and refcount?
	void	IncrementLock(void);
	SInt32	DecrementLock(void);

	void	Destroy(void);

	MEMBER_FN_PREFIX(VMIdentifier);
	DEFINE_MEMBER_FN(Destroy_Internal, void, 0x026EBFA0);
};

UInt64 GetArrayType(UInt64 type);
