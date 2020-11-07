#pragma once

#include "f4se_common/Utilities.h"

#include "f4se/ScaleformTypes.h"
#include "f4se/ScaleformMovie.h"

#include <typeinfo>
#include <map>

class GFxValue;
class GFxMovieView;

class GFxFunctionHandler : public GRefCountBase
{
public:
	GFxFunctionHandler();
	virtual ~GFxFunctionHandler();

	// 38
	class Args
	{
	public:
		GFxValue		* result;	// 00
		GFxMovieView	* movie;	// 08
		GFxValue		* thisObj;	// 10
		GFxValue		* unk18;	// 18
		GFxValue		* args;		// 20
		UInt32			numArgs;	// 28
		UInt32			pad2C;		// 2C
		UInt32			optionID;	// 30 pUserData
	};

	virtual void	Invoke(Args * args) = 0;
};

typedef std::map <const std::type_info *, GFxFunctionHandler *>	FunctionHandlerCache;
extern FunctionHandlerCache g_functionHandlerCache;

template <typename T>
void CreateFunction(GFxValue * dst, GFxMovieRoot * movie)
{
	// either allocate the object or retrieve an existing instance from the cache
	GFxFunctionHandler	* fn = nullptr;

	// check the cache
	FunctionHandlerCache::iterator iter = g_functionHandlerCache.find(&typeid(T));
	if(iter != g_functionHandlerCache.end())
		fn = iter->second;

	if(!fn)
	{
		// not found, allocate a new one
		fn = new T;

		// add it to the cache
		// cache now owns the object as far as refcounting goes
		g_functionHandlerCache[&typeid(T)] = fn;
	}

	// create the function object
	movie->CreateFunction(dst, fn);
}

template <typename T>
void RegisterFunction(GFxValue * dst, GFxMovieRoot * movie, const char * name)
{
	// either allocate the object or retrieve an existing instance from the cache
	GFxValue fnValue;
	CreateFunction<T>(&fnValue, movie);
	dst->SetMember(name, &fnValue);
}

class BSGFxFunctionHandler : public GFxFunctionHandler
{
public:
	virtual ~BSGFxFunctionHandler() { };
};

class SWFToCodeFunctionHandler : public GFxFunctionHandler
{
public:
	virtual ~SWFToCodeFunctionHandler() { }
	virtual void	RegisterFunctions() = 0;	// 02

	DEFINE_MEMBER_FN_2(RegisterNativeFunction, void, 0x02110390, const char * name, UInt32 index);
};