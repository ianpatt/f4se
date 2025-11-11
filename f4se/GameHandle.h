#pragma once

class TESObjectREFR;

#include "f4se_common/Relocation.h"
#include "f4se/NiTypes.h"

typedef bool(*_CreateHandleByREFR)(UInt32 & handleOut, const TESObjectREFR * ref);
extern RelocAddr <_CreateHandleByREFR> CreateHandleByREFR;

typedef bool(*_LookupREFRByHandle)(const UInt32 & handleIn, NiPointer<TESObjectREFR>& ref);
extern RelocAddr <_LookupREFRByHandle> LookupREFRByHandle;

extern RelocPtr <UInt32> g_invalidRefHandle;

template <UInt32 INDEX_BITS = 20, UInt32 FLAG_BITS = 6>
class BSUntypedPointerHandle
{
public:
	typedef UInt32 value_type;


	BSUntypedPointerHandle() :
		_handle(0)
	{
		_handle = get_null_handle();
	}


	BSUntypedPointerHandle(const BSUntypedPointerHandle& a_rhs) :
		_handle(a_rhs._handle)
	{}


	BSUntypedPointerHandle(BSUntypedPointerHandle&& a_rhs) :
		_handle(std::move(a_rhs._handle))
	{
		a_rhs.reset();
	}


	BSUntypedPointerHandle(value_type a_handle) :
		_handle(a_handle)
	{}


	~BSUntypedPointerHandle()
	{
		reset();
	}


	BSUntypedPointerHandle& operator=(const BSUntypedPointerHandle& a_rhs)
	{
		_handle = a_rhs._handle;
		return *this;
	}


	BSUntypedPointerHandle& operator=(BSUntypedPointerHandle&& a_rhs)
	{
		_handle = std::move(a_rhs._handle);
		a_rhs.reset();
		return *this;
	}


	BSUntypedPointerHandle& operator=(value_type a_rhs)
	{
		_handle = a_rhs;
		return *this;
	}


	operator bool() const
	{
		has_value();
	}


	bool has_value() const
	{
		return _handle != get_null_handle();
	}


	value_type value() const
	{
		return _handle;
	}

	operator UInt32&()
	{
		return _handle;
	}


	void reset()
	{
		_handle = get_null_handle();
	}


	friend bool operator==(const BSUntypedPointerHandle& a_lhs, const BSUntypedPointerHandle& a_rhs)
	{
		return a_lhs.value() == a_rhs.value();
	}


	friend bool operator!=(const BSUntypedPointerHandle& a_lhs, const BSUntypedPointerHandle& a_rhs)
	{
		return !(a_lhs == a_rhs);
	}

private:
	static UInt32 get_null_handle()
	{
		return *g_invalidRefHandle;
	}

	// members
	UInt32 _handle;	 // 0
};
STATIC_ASSERT(sizeof(BSUntypedPointerHandle<>) == 0x4);

template <class T, class Handle = BSUntypedPointerHandle<>>
class BSPointerHandle : protected Handle
{
public:
	typedef typename Handle::value_type native_handle_type;


	BSPointerHandle() :
		Handle()
	{}


	BSPointerHandle(const BSPointerHandle& a_rhs) :
		Handle(a_rhs)
	{}


	BSPointerHandle(BSPointerHandle&& a_rhs) :
		Handle(std::move(a_rhs))
	{}


	template <class Y>
	explicit BSPointerHandle(const Y* a_rhs) :
		Handle()
	{
		if (a_rhs && a_rhs->BSHandleRefObject::QRefCount() > 0) {
			create(a_rhs);
		}
	}


	template <class Y>
	BSPointerHandle(const BSPointerHandle<Y, Handle>& a_rhs) :
		Handle(a_rhs)
	{}


	template <class Y>
	BSPointerHandle(BSPointerHandle<Y, Handle>&& a_rhs) :
		Handle(std::move(a_rhs))
	{}


	BSPointerHandle& operator=(const BSPointerHandle& a_rhs)
	{
		Handle::operator=(a_rhs);
		return *this;
	}


	BSPointerHandle& operator=(BSPointerHandle&& a_rhs)
	{
		Handle::operator=(std::move(a_rhs));
		return *this;
	}


	template <class Y>
	BSPointerHandle& operator=(const Y* a_rhs)
	{
		if (a_rhs && a_rhs->handleRefObject.QRefCount() > 0) {
			create(a_rhs);
		}
		else {
			reset();
		}
		return *this;
	}


	template <class Y>
	BSPointerHandle& operator=(const BSPointerHandle<Y, Handle>& a_rhs)
	{
		Handle::operator=(static_cast<const Handle&>(a_rhs));
		return *this;
	}

	void reset()
	{
		Handle::reset();
	}

	NiPointer<T> get() const
	{
		NiPointer<T> ptr;
		lookup(ptr);
		return ptr;
	}


	native_handle_type native_handle()
	{
		return Handle::value();
	}


	operator bool() const
	{
		return Handle::has_value();
	}


	friend bool operator==(const BSPointerHandle& a_lhs, const BSPointerHandle& a_rhs)
	{
		return static_cast<const Handle&>(a_lhs) == static_cast<const Handle&>(a_rhs);
	}


	friend bool operator!=(const BSPointerHandle& a_lhs, const BSPointerHandle& a_rhs)
	{
		return !(a_lhs == a_rhs);
	}

private:
	void create(const T* a_ptr)
	{
		CreateHandleByREFR(*this, a_ptr);
	}


	bool lookup(NiPointer<T>& a_refPtr) const
	{
		return LookupREFRByHandle(*this, a_refPtr);
	}
};

template <class T>
class BSPointerHandleManager
{
public:
};
STATIC_ASSERT(sizeof(BSPointerHandleManager<void*>) == 0x1);

class HandleManager : public BSPointerHandleManager<BSUntypedPointerHandle<>>
{
public:
};
STATIC_ASSERT(sizeof(HandleManager) == 0x1);

template <class T, class Manager = HandleManager>
class BSPointerHandleManagerInterface
{
public:
	typedef T value_type;

	static bool GetSmartPointer(NiPointer<T>& spSmartPointer, const BSPointerHandle<T>& aHandle)
	{
		using _GetSmartPointer = bool(*)(NiPointer<T>& spSmartPointer, const BSPointerHandle<T>& aHandle);
		RelocAddr <_GetSmartPointer> GetSmartPointerAddr(0x0021B210);
		return GetSmartPointerAddr(spSmartPointer, aHandle);
	}
};

template <class T>
class BSPointerHandleSmartPointer : public NiPointer<typename T::value_type>
{
public:
};
