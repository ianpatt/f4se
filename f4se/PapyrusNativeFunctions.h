#pragma once

#include "f4se/GameTypes.h"
#include "f4se/PapyrusArgs.h"

class VirtualMachine;
class VMState;
class VMValue;

struct StaticFunctionTag
{
	enum { kTypeID = 0 };
};

class IFunction
{
public:
	enum
	{
		kFunctionFlag_NoWait = 0x01	// set this only if your function is thread-safe
	};

	IFunction()				{ }
	virtual ~IFunction()	{ }

//	void	** _vtbl;	// 00
	UInt32	refCount;	// 08 BSIntrusiveRefCounted
	UInt32	pad0C;		// 0C

	virtual BSFixedString *	GetName() = 0;
	virtual BSFixedString *	GetClassName() = 0;
	virtual BSFixedString *	GetStr20() = 0;
	virtual UInt64 *		GetReturnType(UInt64 * dst) = 0;
	virtual UInt64			GetNumParams() = 0;
	virtual UInt64			GetParam(UInt32 idx, BSFixedString * outName, UInt64 * outType) = 0;
	virtual UInt64			GetNumParams2() = 0;
	virtual bool			IsNative() = 0;
	virtual bool			IsStatic() = 0;
	virtual bool			Unk_0A() = 0;
	virtual UInt32			Unk_0B() = 0;
	virtual UInt32			GetUnk44() = 0;
	virtual BSFixedString *	GetStr48() = 0;
	virtual void			Unk_0E() = 0;
	virtual UInt32			Invoke(void * arg0, void * arg1, VirtualMachine * arg2, VMState * arg3) = 0;
	virtual BSFixedString *	GetSourceFile() = 0;	// guess
	virtual bool			Unk_11(UInt32 arg0, UInt32 * arg1) = 0;
	virtual bool			GetParamName(UInt32 idx, BSFixedString * out) = 0;
	virtual UInt32			GetUnk41() = 0;
	virtual void			SetUnk41(UInt8 arg) = 0;
};

STATIC_ASSERT(sizeof(IFunction) == 0x10);

// 50
class NativeFunctionBase : public IFunction
{
public:
	NativeFunctionBase()			{ }
	virtual ~NativeFunctionBase()	{ }

	// 10
	struct ParameterInfo
	{
		// 10
		struct Entry
		{
			BSFixedString	name;	// 00

			union // 08
			{
				UInt32	type;		// 08
				UInt64	type64;		// 08
			};
		};

		Entry		* data;	// new []
		UInt16		numParams;
		UInt16		realNumParams;
		UInt32		pad0C;

		MEMBER_FN_PREFIX(ParameterInfo);
		DEFINE_MEMBER_FN(GetParam, UInt64, 0x0270DE00, UInt32 idx, BSFixedString * outName, UInt64 * outType);
	};

	virtual BSFixedString *	GetName()							{ return &m_fnName; }
	virtual BSFixedString * GetClassName()						{ return &m_className; }
	virtual BSFixedString * GetStr20()							{ return &m_unk20; }
	virtual UInt64 *		GetReturnType(UInt64 * dst)			{ *dst = m_retnType; return dst; }
	virtual UInt64			GetNumParams()						{ return m_params.realNumParams; }
	virtual UInt64			GetParam(UInt32 idx, BSFixedString * outName, UInt64 * outType)
																{ return CALL_MEMBER_FN(&m_params, GetParam)(idx, outName, outType); }
	virtual UInt64			GetNumParams2()						{ return m_params.numParams; }
	virtual bool			IsNative()							{ return true; }
	virtual bool			IsStatic()							{ return m_isStatic; }
	virtual bool			Unk_0A()							{ return false; }
	virtual UInt32			Unk_0B()							{ return 0; }
	virtual UInt32			GetUnk44()							{ return m_unk44; }
	virtual BSFixedString *	GetStr48()							{ return &m_unk48; }
	virtual void			Unk_0E()							{ }
	virtual UInt32			Invoke(void * arg0, void * arg1, VirtualMachine * arg2, VMState * arg3)
																{ return CALL_MEMBER_FN(this, Impl_Invoke)(arg0, arg1, arg2, arg3); }
	virtual BSFixedString *	GetSourceFile()						{ return CALL_MEMBER_FN(this, Impl_GetSourceFile)(); }
	virtual bool			Unk_11(UInt32 arg0, UInt32 * arg1)	{ *arg1 = 0; return false; }
	virtual bool			GetParamName(UInt32 idx, BSFixedString * out)
																{ return CALL_MEMBER_FN(this, Impl_GetParamName)(idx, out); }
	virtual UInt32			GetUnk41()							{ return m_unk41; }
	virtual void			SetUnk41(UInt8 arg)					{ m_unk41 = arg; }
	virtual bool			HasCallback() = 0;
	virtual bool			Run(VMValue * baseValue, VirtualMachine * vm, UInt32 arg2, VMValue * resultValue, VMState * state) = 0;

	MEMBER_FN_PREFIX(NativeFunctionBase);
	DEFINE_MEMBER_FN(Impl_Invoke, UInt32, 0x0270D550, void * arg0, void * arg1, VirtualMachine * arg2, VMState * arg3);
	DEFINE_MEMBER_FN(Impl_GetSourceFile, BSFixedString *, 0x0270D420);
	DEFINE_MEMBER_FN(Impl_GetParamName, bool, 0x0270D440, UInt32 idx, BSFixedString * out);

	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free);

protected:
	BSFixedString	m_fnName;		// 10
	BSFixedString	m_className;	// 18
	BSFixedString	m_unk20;		// 20
	UInt64			m_retnType;		// 28
	ParameterInfo	m_params;		// 30
	bool			m_isStatic;		// 40
	UInt8			m_unk41;		// 41
	bool			m_isLatent;		// 42 - is latent
	UInt8			m_pad43;		// 43
	UInt32			m_unk44;		// 44
	BSFixedString	m_unk48;		// 48
};

STATIC_ASSERT(sizeof(NativeFunctionBase) == 0x50);

// 58
class NativeFunction : public NativeFunctionBase
{
public:
	NativeFunction(const char * fnName, const char * className, bool isStatic, UInt32 numParams)
									{ CALL_MEMBER_FN(this, Impl_ctor)(fnName, className, isStatic, numParams); }
	virtual ~NativeFunction()		{ CALL_MEMBER_FN(this, Impl_dtor)(); }

	virtual bool	HasCallback()	{ return m_callback != nullptr; }
	virtual bool	Run(VMValue * baseValue, VirtualMachine * vm, UInt32 arg2, VMValue * resultValue, VMState * state) = 0;

	MEMBER_FN_PREFIX(NativeFunction);
	DEFINE_MEMBER_FN(Impl_ctor, NativeFunction *, 0x0270DA50, const char * fnName, const char * className, UInt32 unk0, UInt32 numParams);
	DEFINE_MEMBER_FN(Impl_dtor, void, 0x0270DC70);

protected:
	void	* m_callback;	// 50

	// hide
	NativeFunction();
};

STATIC_ASSERT(sizeof(NativeFunction) == 0x58);

#define NUM_PARAMS 0
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 1
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 2
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 3
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 4
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 5
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 6
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 7
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 8
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 9
#include "PapyrusNativeFunctionDef.inl"

#define NUM_PARAMS 10
#include "PapyrusNativeFunctionDef.inl"
