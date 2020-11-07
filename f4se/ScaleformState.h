#pragma once

#include "f4se/ScaleformAPI.h"
#include "f4se/ScaleformTypes.h"

// 18
class GFxState : public GRefCountBase
{
public:
	enum
	{
		kInterface_None =			0x00,
		kInterface_Translator =		0x01,
		kInterface_Log =			0x02,
		kInterface_FileOpener =		0x0C,
		kInterface_ImageLoader =	0x0E,
		kInterface_ZlibSupport =	0x1D,
	};

	UInt64	interfaceType;	// 10

	// redirect new/delete to the scaleform heap
	static void * operator new(std::size_t size)
	{
		return ScaleformHeap_Allocate(size);
	}

	static void * operator new(std::size_t size, const std::nothrow_t &)
	{
		return ScaleformHeap_Allocate(size);
	}

	// placement new
	static void * operator new(std::size_t size, void * ptr)
	{
		return ptr;
	}

	static void operator delete(void * ptr)
	{
		ScaleformHeap_Free(ptr);
	}

	// placement delete
	static void operator delete(void *, void *)
	{
		//
	}

	void	AddRef(void)
	{
		InterlockedIncrement(&refCount);
	}

	void	Release(void)
	{
		if(InterlockedDecrement(&refCount) == 0)
		{
			delete this;
		}
	}
};

class GFxStateBag
{
public:
	virtual				~GFxStateBag();

	virtual GFxStateBag *	GetStateBagImpl(void);
	virtual void		SetState(UInt32 type, void * ptr);
	virtual void *		GetStateAddRef(UInt32 type);
	virtual void		Unk_04(void);
	virtual void		Unk_05(void);
};

class GFxLogBase
{
public:
	virtual ~GFxLogBase () { }

	virtual bool    IsVerboseActionErrors() const   { return true; }
};

class GFxLogState : public GFxState
{
public:
	GFxLogState() { }
	virtual ~GFxLogState () { }

	virtual void Unk_01(void);

	GFxLogBase	* logBase;	// 18
	void		* logger;	// 20
};

class F4SEGFxLogger : public GRefCountBase
{
public: 
	F4SEGFxLogger() { }
	virtual ~F4SEGFxLogger () { }

	enum
	{
		Log_Channel_General         = 0x10,
		Log_Channel_Script          = 0x20,
		Log_Channel_Parse           = 0x30,
		Log_Channel_Action          = 0x40,
		Log_Channel_Debug           = 0x50,
		Log_Channel_Mask            = 0xF0,
		Log_MessageType_Error       = 0,
		Log_MessageType_Warning     = 1,
		Log_MessageType_Message     = 2,
		Log_Error                   = Log_Channel_General | Log_MessageType_Error,
		Log_Warning                 = Log_Channel_General | Log_MessageType_Warning,
		Log_Message                 = Log_Channel_General | Log_MessageType_Message,
		Log_ScriptError             = Log_Channel_Script | Log_MessageType_Error,
		Log_ScriptWarning           = Log_Channel_Script | Log_MessageType_Warning,
		Log_ScriptMessage           = Log_Channel_Script | Log_MessageType_Message,
		Log_Parse                   = Log_Channel_Parse | 0,
		Log_ParseShape              = Log_Channel_Parse | 1,
		Log_ParseMorphShape         = Log_Channel_Parse | 2,
		Log_ParseAction             = Log_Channel_Parse | 3,
		Log_Action                  = Log_Channel_Action | 0
	};

	virtual void LogMessageVarg(UInt32 messageType, const char* fmt, va_list args);
};
