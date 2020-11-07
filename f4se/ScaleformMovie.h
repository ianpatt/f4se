#pragma once

class GFxFunctionHandler;
class GFxValue;

// UNTESTED!!!
class GFxMovieRoot
{
public:
	virtual ~GFxMovieRoot();

	virtual void Unk_01();
	virtual void Unk_02();
	virtual void Unk_03();
	virtual void Unk_04();
	virtual void Unk_05();
	virtual void Unk_06();
	virtual void Unk_07();
	virtual void Unk_08();
	virtual void Unk_09();
	virtual void Unk_0A();
	virtual void Unk_0B();
	virtual void Unk_0C();
	virtual void Unk_0D();
	virtual void Unk_0E();
	virtual void Unk_0F();
	virtual void Unk_10();
	virtual void Unk_11();
	virtual void Unk_12();
	virtual void Unk_13();
	virtual void Unk_14();
	virtual void Unk_15();
	virtual void Unk_16();
	virtual void Unk_17();
	virtual void Unk_18();
	virtual void Unk_19();
	virtual void Unk_1A();
	virtual void Unk_1B();
	virtual void Unk_1C();
	virtual void Unk_1D();
	virtual void Unk_1E();
	virtual void Unk_1F();
	virtual void Unk_20();
	virtual void Unk_21();
	virtual void Unk_22();
	virtual void Unk_23();
	virtual void Unk_24();
	virtual void Unk_25();
	virtual void Unk_26();
	virtual void Unk_27();
	virtual void Unk_28();
	virtual void Unk_29();
	virtual void Unk_2A();
	virtual void Unk_2B();
	virtual void CreateString(GFxValue* pValue, const char* pString);
	virtual void CreateStringW(GFxValue* pValue, const wchar_t* pString);
	virtual void CreateObject(GFxValue* pValue, const char* className = NULL, const GFxValue* pArgs = NULL, UInt32 nArgs = 0);
	virtual void CreateArray(GFxValue* pValue);
	virtual void CreateFunction(GFxValue* pValue, GFxFunctionHandler* pFunc, void* puserData = NULL);
	virtual bool SetVariable(const char* pVarPath, const GFxValue * value, UInt32 setType = 0);
	virtual bool GetVariable(GFxValue *pValue, const char* pVarPath) const;
	virtual void Unk_33();
	virtual void Unk_34();
	virtual void Unk_35();
	virtual bool GetVariableArray(UInt32 type, const char* pPathToVar, UInt32 index, void* pData, UInt32 count);
	virtual void Unk_37();
	virtual void Unk_38();
	virtual bool Invoke(const char* pPathToMethod, GFxValue *pResult, const GFxValue* pArgs, UInt32 nArgs);
	virtual bool InvokeArgs(const char* pPathToMethod, GFxValue *presult, const char* pArgFmt, va_list args);

	bool Invoke(const char* pPathToMethod, GFxValue *presult, const char* pArgFmt, ...)
	{
		// Delegate to InvokeArgs
		va_list args;
		va_start(args, pArgFmt);
		bool retVal = InvokeArgs(pPathToMethod, presult, pArgFmt, args);
		va_end(args);
		return retVal;
	}
};

class GFxMovieView
{
public:
	virtual ~GFxMovieView();

	UInt32	unk08;			// 08
	UInt32	unk0C;			// 0C
	void	* unk10;		// 10
	GFxMovieRoot	* movieRoot;	// 18
	// ...
};
