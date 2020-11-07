#pragma once

#include "f4se/GameUtilities.h"
#include "f4se/GameTypes.h"

#include "f4se/PapyrusInterfaces.h"
#include "f4se/PapyrusValue.h"

// skipping the BSScript::Internal namespace stuff
#include <functional>

class IFunction;

// 08
class Lock
{
public:
	UInt32	m_threadID;
	UInt32	m_lockCount;
};

// BSScript::Internal::VirtualMachine: ??_7VirtualMachine@Internal@BSScript@@6B@
class VirtualMachine
{
public:
	VirtualMachine();
	virtual ~VirtualMachine();

	virtual void	Unk_01();	// take m_lockC0 and then calls this+C8
	virtual void	Unk_02(UInt64 unk);	// take m_lockC0 and then sets m_unkE8
	virtual void	Unk_03();
	virtual void	Unk_04();
	virtual void	Unk_05(UInt8 unk);	// set m_unk82BC
	virtual bool	Unk_06();	// take m_lockBDC4, return m_BDF4 & 2
	virtual void	RegisterForm(UInt32 typeID, const char * papyrusClassName);
	virtual void	Unk_08();
	virtual bool	GetObjectTypeInfo(UInt32 formType, VMObjectTypeInfo ** outTypeInfo);	// Must release outTypeInfo explicitly
	virtual bool	GetObjectTypeInfoByName(const BSFixedString * name, VMObjectTypeInfo ** outTypeInfo); // Must release outTypeInfo explicitly
	virtual void	Unk_0B();
	virtual bool	Unk_0C(BSFixedString * typeName0, UInt32 * unk);
	virtual void	Unk_0D();
	virtual void	Unk_0E();
	virtual void	Unk_0F();
	virtual void	Unk_10();
	virtual void	Unk_11();
	virtual void	Unk_12();
	virtual bool	GetStructTypeInfo(const BSFixedString * name, VMStructTypeInfo ** outTypeInfo);	// Must release outTypeInfo explicitly
	virtual void	Unk_14();
	virtual void	Unk_15();
	virtual void	Unk_16();
	virtual bool	CreateObjectIdentifier(const BSFixedString * className, VMIdentifier ** identifier);
	virtual bool	CreateStruct(const BSFixedString * name, VMValue::StructData ** value);
	virtual bool	CreateArray(VMValue * value, UInt32 size, VMValue::ArrayData ** data);
	virtual void	Unk_1A();
	virtual void	RegisterFunction(IFunction * fn);
	virtual void	SetFunctionFlagsEx(const char * className, UInt32 unk0, const char * fnName, UInt32 flags);
	virtual void	SetFunctionFlags(const char * className, const char * fnName, UInt32 flags);
#if _MSC_VER == 1700
	virtual void	ForEachIdentifier(UInt64 handle, const std::function<UInt32(VMIdentifier*)> & functor); // return 1 to continue
#else
	virtual void	ForEachIdentifier(UInt64 handle, void * stdFunction);
#endif
	virtual bool	GetObjectIdentifier(UInt64 handle, const char * typeName, UInt64 unk1, VMIdentifier ** identifier, UInt8 unk2);
	virtual void	Unk_20();
	virtual void	Unk_21();
	virtual void	CastAs(VMIdentifier** idInOut, VMObjectTypeInfo ** typeAs, UInt64 unk1); // checks (typeAs->unk40 & 3) == 3 first
	virtual bool	SetPropertyValue(VMIdentifier** identifier, const char* propertyName, VMValue * newValue, UInt64* unk4);
	virtual bool	GetPropertyValue(VMIdentifier** identifier, const char* propertyName, VMValue * result);
	virtual bool	GetPropertyValueByIndex(VMIdentifier** identifier, SInt32 idx, VMValue* outValue);
	virtual void	Unk_26();
	virtual void	Unk_27();
	virtual void	Unk_28();
	virtual void	Unk_29();
	virtual void	Unk_2A();
	virtual void	QueueEvent(UInt64 handle, BSFixedString * eventName, void * unk1);
	virtual void	Unk_2C();
	virtual void	Unk_2D();
	virtual void	Unk_2E();
	virtual void	Unk_2F();
	virtual void	Unk_30();
	virtual void	ResumeStack(UInt32 stackId, VMValue* result);
	virtual void	Unk_32();
	virtual IObjectHandlePolicy *	GetHandlePolicy(void);
	virtual void	Unk_34();
	virtual IObjectBindPolicy *		GetObjectBindPolicy(void);
	virtual void	Unk_36();
	virtual void	Unk_37();
	virtual void	Unk_38();
	virtual void	Unk_39();
	virtual void	Unk_3A();
	virtual void	Unk_3B();
	virtual void	Unk_3C();
	virtual void	Unk_3D();

private:

//	void	** _vtbl;					// 00
	UInt64	pad08[(0xC0 - 0x08) >> 3];	// 08
	Lock	m_lockC0;					// C0
	UInt64	padC8[(0xE8 - 0xC8) >> 3];	// C8
	UInt64	m_unkE8;					// E8
	UInt64	unkF0[(0x168 - 0xF0) >> 3];

public:
	class ComplexTypeInfoItem
	{
	public:
		BSFixedString		name;			// 00
		IComplexType		* typeInfo;		// 08

		bool operator==(const ComplexTypeInfoItem & rhs) const	{ return name == rhs.name; }
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
			_MESSAGE("\t\tinstance: %08X", typeInfo);
		}
	};
	class FormTypeName
	{
	public:
		UInt32				typeId;			// 00
		BSFixedString		name;			// 08

		bool operator==(const FormTypeName & rhs) const	{ return typeId == rhs.typeId; }
		operator UInt32() const	{ return typeId; }

		static inline UInt32 GetHash(UInt32 * key)
		{
			UInt32 hash;
			CalculateCRC32_32(&hash, (UInt32)*key, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\ttypeId: %d", typeId);
			_MESSAGE("\t\tname: %s", name.c_str());
		}
	};

	class StackTableItem
	{
	public:
		UInt32				stackId;
		void				*	data;

		bool operator==(const StackTableItem & rhs) const	{ return stackId == rhs.stackId; }
		operator UInt32() const	{ return stackId; }

		static inline UInt32 GetHash(UInt32* pStackId)
		{
			UInt32 hash;
			CalculateCRC32_32(&hash, (UInt32)*pStackId, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\tstack: %d", stackId);
			_MESSAGE("\t\tdata: %016I64X", data);
		}
	};

	class IdentifierItem
	{
	public:
		UInt64					handle;	// 00
		UInt32					count;	// 08
		UInt32					pad0C;	// 0C
		union
		{
			VMIdentifier * one;
			VMIdentifier ** many;
		} identifier;

		class IScriptVisitor
		{
		public:
			virtual ~IScriptVisitor() { }

			virtual bool Visit(VMIdentifier * obj) = 0;
		};

		inline VMIdentifier * GetScriptObject(VMIdentifier * identifier)
		{
			return reinterpret_cast<VMIdentifier*>(reinterpret_cast<uintptr_t>(identifier) & ~1LL);
		}

		void ForEachScript(IScriptVisitor * visitor)
		{
			if(count == 1)
			{
				visitor->Visit(GetScriptObject(identifier.one));
			}
			else
			{
				for(UInt32 i = 0; i < count; ++i)
				{
					if(!visitor->Visit(GetScriptObject(identifier.many[i])))
						return;
				}
			}
		}

		bool operator==(const IdentifierItem & rhs) const	{ return handle == rhs.handle; }
		operator UInt64() const	{ return handle; }

		static inline UInt64 GetHash(UInt64* pHandle)
		{
			UInt32 hash;
			CalculateCRC32_64(&hash, (UInt64)*pHandle, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\thandle: %016I64X", handle);
			_MESSAGE("\t\tscripts:");
			
			class DumpVisitor : public IScriptVisitor
			{
			public:
				virtual bool Visit(VMIdentifier* obj)
				{
					_MESSAGE("\t\t\t%s", obj->m_typeInfo->m_typeName.c_str());
					return true;
				}
			};
			DumpVisitor visitor;
			ForEachScript(&visitor);
		}
	};

	tHashSet<ComplexTypeInfoItem, BSFixedString> m_objectTypes;		// 168
	tHashSet<ComplexTypeInfoItem, BSFixedString> m_structTypes;		// 198
	tHashSet<FormTypeName, UInt32> m_typeNames;	// 1C8

	bool HasStack(UInt32 stackId);

	UInt64	unk1F8[(0xBD58 - 0x1F8) >> 3];
	BSReadWriteLock stackLock;						// BD58
	tHashSet<StackTableItem, UInt32> m_allStacks;	// BD60
	tHashSet<StackTableItem, UInt32> m_waitStacks;	// BD90
	UInt64	unkBDC0[(0xBDF8 - 0xBDC0) >> 3];		// BDC0
	BSReadWriteLock scriptsLock;					// BDF8
	tHashSet<IdentifierItem, UInt64> m_attachedScripts;	// BE00
};
STATIC_ASSERT(offsetof(VirtualMachine, m_objectTypes) == 0x168);
STATIC_ASSERT(offsetof(VirtualMachine, m_structTypes) == 0x198);
STATIC_ASSERT(offsetof(VirtualMachine, m_typeNames) == 0x1C8);

// 87A0?
class GameVM : public IClientVM
{
public:
	virtual ~GameVM();

	virtual void	Unk_01();
	virtual void	Unk_02();

	IStackCallbackSaveInterface m_callbackSaveInterface;	// 08

	enum
	{
		kEventSink_Stats = 0,
		kEventSink_InputEnableLayerDestroyed,
		kEventSink_PositionPlayer,
		kEventSink_FormDelete,
		kEventSink_FormIDRemap,
		kEventSink_InitScript,
		kEventSink_ResolveNPCTemplates,
		kEventSink_UniqueIDChange,
		kEventSink_NumEvents
	};

	void			* m_eventSinks[kEventSink_NumEvents];	// 10
	UInt64			unk50[(0xB0 - 0x50) >> 3];				// 50

	VirtualMachine			* m_virtualMachine;				// B0
	IVMSaveLoadInterface	* m_saveLoadInterface;			// B8
	IVMDebugInterface		* m_debugInterface;				// C0
	SimpleAllocMemoryPagePolicy	m_memoryPagePolicy;			// C8

	UInt64			unkF0[(0x1E0 - 0xF0) >> 3];				// F0
	IObjectHandlePolicy		* m_objectHandlePolicy;			// 1E0
	// ...

	MEMBER_FN_PREFIX(GameVM);
	DEFINE_MEMBER_FN(SendPapyrusEvent, void, 0x01374FB0, UInt64 handle, const BSFixedString & eventName, std::function<bool(void*)> functor); // Signature not correct yet
};

extern RelocPtr <GameVM *> g_gameVM;
