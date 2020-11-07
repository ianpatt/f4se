#if NUM_PARAMS > 10
#error PapyrusDelayFunctorsDef: too many params
#endif

template <const char* T_functorName, void * T_runFunc, typename T_Base
#if !VOID_SPEC
	, typename T_Result
#endif
#if NUM_PARAMS >= 1
	,typename T_Arg0
#endif
#if NUM_PARAMS >= 2
	,typename T_Arg1
#endif
#if NUM_PARAMS >= 3
	,typename T_Arg2
#endif
#if NUM_PARAMS >= 4
	,typename T_Arg3
#endif
#if NUM_PARAMS >= 5
	,typename T_Arg4
#endif
#if NUM_PARAMS >= 6
	,typename T_Arg5
#endif
#if NUM_PARAMS >= 7
	,typename T_Arg6
#endif
#if NUM_PARAMS >= 8
	,typename T_Arg7
#endif
#if NUM_PARAMS >= 9
	,typename T_Arg8
#endif
#if NUM_PARAMS >= 10
	,typename T_Arg9
#endif
>

class CLASS_NAME
#if VOID_SPEC
	<T_functorName, T_runFunc, T_Base, void
#if NUM_PARAMS >= 1
	, T_Arg0
#endif
#if NUM_PARAMS >= 2
	, T_Arg1
#endif
#if NUM_PARAMS >= 3
	, T_Arg2
#endif
#if NUM_PARAMS >= 4
	, T_Arg3
#endif
#if NUM_PARAMS >= 5
	, T_Arg4
#endif
#if NUM_PARAMS >= 6
	, T_Arg5
#endif
#if NUM_PARAMS >= 7
	, T_Arg6
#endif
#if NUM_PARAMS >= 8
	, T_Arg7
#endif
#if NUM_PARAMS >= 9
	, T_Arg8
#endif
#if NUM_PARAMS >= 10
	, T_Arg9
#endif
	>
#endif
	: public LatentF4SEDelayFunctor
{
public:
	typedef
#if VOID_SPEC
		void
#else
		T_Result
#endif
		(* CallbackType)(UInt32 stackId,
		T_Base * base
#if NUM_PARAMS >= 1
		, T_Arg0 arg0
#endif
#if NUM_PARAMS >= 2
		, T_Arg1 arg1
#endif
#if NUM_PARAMS >= 3
		, T_Arg2 arg2
#endif
#if NUM_PARAMS >= 4
		, T_Arg3 arg3
#endif
#if NUM_PARAMS >= 5
		, T_Arg4 arg4
#endif
#if NUM_PARAMS >= 6
		, T_Arg5 arg5
#endif
#if NUM_PARAMS >= 7
		, T_Arg6 arg6
#endif
#if NUM_PARAMS >= 8
		, T_Arg7 arg7
#endif
#if NUM_PARAMS >= 9
		, T_Arg8 arg8
#endif
#if NUM_PARAMS >= 10
		, T_Arg9 arg9
#endif
		);

	// Function passed to Class isn't actually kept by the object, it's used to verify parameters author-end
	explicit CLASS_NAME(CallbackType func, VirtualMachine * vm, UInt32 stackId, 
		T_Base * base
#if NUM_PARAMS >= 1
		, T_Arg0 arg0
#endif
#if NUM_PARAMS >= 2
		, T_Arg1 arg1
#endif
#if NUM_PARAMS >= 3
		, T_Arg2 arg2
#endif
#if NUM_PARAMS >= 4
		, T_Arg3 arg3
#endif
#if NUM_PARAMS >= 5
		, T_Arg4 arg4
#endif
#if NUM_PARAMS >= 6
		, T_Arg5 arg5
#endif
#if NUM_PARAMS >= 7
		, T_Arg6 arg6
#endif
#if NUM_PARAMS >= 8
		, T_Arg7 arg7
#endif
#if NUM_PARAMS >= 9
		, T_Arg8 arg8
#endif
#if NUM_PARAMS >= 10
		, T_Arg9 arg9
#endif
		) : LatentF4SEDelayFunctor( stackId )
	{
		if (! IsStaticType <T_Base>::value)
		{
			PackValue(&_base, &base, vm);
		}

#if NUM_PARAMS >= 1
		PackValue(&_arg0, &arg0, vm);
#endif
#if NUM_PARAMS >= 2
		PackValue(&_arg1, &arg1, vm);
#endif
#if NUM_PARAMS >= 3
		PackValue(&_arg2, &arg2, vm);
#endif
#if NUM_PARAMS >= 4
		PackValue(&_arg3, &arg3, vm);
#endif
#if NUM_PARAMS >= 5
		PackValue(&_arg4, &arg4, vm);
#endif
#if NUM_PARAMS >= 6
		PackValue(&_arg5, &arg5, vm);
#endif
#if NUM_PARAMS >= 7
		PackValue(&_arg6, &arg6, vm);
#endif
#if NUM_PARAMS >= 8
		PackValue(&_arg7, &arg7, vm);
#endif
#if NUM_PARAMS >= 9
		PackValue(&_arg8, &arg8, vm);
#endif
#if NUM_PARAMS >= 10
		PackValue(&_arg9, &arg9, vm);
#endif
	}
	explicit CLASS_NAME(SerializationTag tag) : LatentF4SEDelayFunctor( tag ) {}

	virtual const char* ClassName() const override		{ return T_functorName; }
	virtual UInt32		ClassVersion() const override	{ return 1; }

	virtual bool Save(const F4SESerializationInterface* intfc) override
	{
		using namespace Serialization;

		if(!LatentF4SEDelayFunctor::Save(intfc))
			return false;

		if (! IsStaticType <T_Base>::value)
		{
			if(!WriteVMData(intfc, &_base))
				return false;
		}

#if NUM_PARAMS >= 1
		if(!WriteVMData(intfc, &_arg0))
			return false;
#endif
#if NUM_PARAMS >= 2
		if(!WriteVMData(intfc, &_arg1))
			return false;
#endif
#if NUM_PARAMS >= 3
		if(!WriteVMData(intfc, &_arg2))
			return false;
#endif
#if NUM_PARAMS >= 4
		if(!WriteVMData(intfc, &_arg3))
			return false;
#endif
#if NUM_PARAMS >= 5
		if(!WriteVMData(intfc, &_arg4))
			return false;
#endif
#if NUM_PARAMS >= 6
		if(!WriteVMData(intfc, &_arg5))
			return false;
#endif
#if NUM_PARAMS >= 7
		if(!WriteVMData(intfc, &_arg6))
			return false;
#endif
#if NUM_PARAMS >= 8
		if(!WriteVMData(intfc, &_arg7))
			return false;
#endif
#if NUM_PARAMS >= 9
		if(!WriteVMData(intfc, &_arg8))
			return false;
#endif
#if NUM_PARAMS >= 10
		if(!WriteVMData(intfc, &_arg9))
			return false;
#endif

		return true;
	}

	virtual bool Load(const F4SESerializationInterface* intfc, UInt32 version) override
	{
		using namespace Serialization;

		if(!LatentF4SEDelayFunctor::Load(intfc, version))
			return false;

		if (! IsStaticType <T_Base>::value)
		{
			if(!ReadVMData(intfc, &_base))
				return false;
		}

#if NUM_PARAMS >= 1
		if(!ReadVMData(intfc, &_arg0))
			return false;
#endif
#if NUM_PARAMS >= 2
		if(!ReadVMData(intfc, &_arg1))
			return false;
#endif
#if NUM_PARAMS >= 3
		if(!ReadVMData(intfc, &_arg2))
			return false;
#endif
#if NUM_PARAMS >= 4
		if(!ReadVMData(intfc, &_arg3))
			return false;
#endif
#if NUM_PARAMS >= 5
		if(!ReadVMData(intfc, &_arg4))
			return false;
#endif
#if NUM_PARAMS >= 6
		if(!ReadVMData(intfc, &_arg5))
			return false;
#endif
#if NUM_PARAMS >= 7
		if(!ReadVMData(intfc, &_arg6))
			return false;
#endif
#if NUM_PARAMS >= 8
		if(!ReadVMData(intfc, &_arg7))
			return false;
#endif
#if NUM_PARAMS >= 9
		if(!ReadVMData(intfc, &_arg8))
			return false;
#endif
#if NUM_PARAMS >= 10
		if(!ReadVMData(intfc, &_arg9))
			return false;
#endif

		return true;
	}

	virtual bool Run(VMValue& resultValue) override
	{
		T_Base	* base = NULL;

		// extract base object pointer for non-static types
		if (! IsStaticType <T_Base>::value)
		{
			UnpackValue(&base, &_base);
			if (!base) return false;
		}

		// extract parameters
#if NUM_PARAMS >= 1
		T_Arg0	arg0;
		UnpackValue(&arg0, &_arg0);
#endif
#if NUM_PARAMS >= 2
		T_Arg1	arg1;
		UnpackValue(&arg1, &_arg1);
#endif
#if NUM_PARAMS >= 3
		T_Arg2	arg2;
		UnpackValue(&arg2, &_arg2);
#endif
#if NUM_PARAMS >= 4
		T_Arg3	arg3;
		UnpackValue(&arg3, &_arg3);
#endif
#if NUM_PARAMS >= 5
		T_Arg4	arg4;
		UnpackValue(&arg4, &_arg4);
#endif
#if NUM_PARAMS >= 6
		T_Arg5	arg5;
		UnpackValue(&arg5, &_arg5);
#endif
#if NUM_PARAMS >= 7
		T_Arg6	arg6;
		UnpackValue(&arg6, &_arg6);
#endif
#if NUM_PARAMS >= 8
		T_Arg7	arg7;
		UnpackValue(&arg7, &_arg7);
#endif
#if NUM_PARAMS >= 9
		T_Arg8	arg8;
		UnpackValue(&arg8, &_arg8);
#endif
#if NUM_PARAMS >= 10
		T_Arg9	arg9;
		UnpackValue(&arg9, &_arg9);
#endif

#if !VOID_SPEC
		T_Result	result =
#endif
			((CallbackType)T_runFunc)(stackId_, base
#if NUM_PARAMS >= 1
			, arg0
#endif
#if NUM_PARAMS >= 2
			, arg1
#endif
#if NUM_PARAMS >= 3
			, arg2
#endif
#if NUM_PARAMS >= 4
			, arg3
#endif
#if NUM_PARAMS >= 5
			, arg4
#endif
#if NUM_PARAMS >= 6
			, arg5
#endif
#if NUM_PARAMS >= 7
			, arg6
#endif
#if NUM_PARAMS >= 8
			, arg7
#endif
#if NUM_PARAMS >= 9
			, arg8
#endif
#if NUM_PARAMS >= 10
			, arg9
#endif
			);

		// pack the result
#if VOID_SPEC
		resultValue.SetNone();
#else
		PackValue(&resultValue, &result, (*g_gameVM)->m_virtualMachine);
#endif
		if (! IsStaticType <T_Base>::value)
		{
			DestroyValue(&base);
		}
		return true;
	}

private:
	VMValue _base;
#if NUM_PARAMS >= 1
	VMValue _arg0;
#endif
#if NUM_PARAMS >= 2
	VMValue _arg1;
#endif
#if NUM_PARAMS >= 3
	VMValue _arg2;
#endif
#if NUM_PARAMS >= 4
	VMValue _arg3;
#endif
#if NUM_PARAMS >= 5
	VMValue _arg4;
#endif
#if NUM_PARAMS >= 6
	VMValue _arg5;
#endif
#if NUM_PARAMS >= 7
	VMValue _arg6;
#endif
#if NUM_PARAMS >= 8
	VMValue _arg7;
#endif
#if NUM_PARAMS >= 9
	VMValue _arg8;
#endif
#if NUM_PARAMS >= 10
	VMValue _arg9;
#endif
};
#undef VOID_SPEC