#if NUM_PARAMS > 10
#error PapyrusNativeFunctionDef: too many params
#endif

template <typename T_Base

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

	<T_Base, void

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
	: public NativeFunction
{
public:

	typedef
#if LATENT_SPEC
		bool
#elif VOID_SPEC
		void
#else
		T_Result
#endif
		(* CallbackType)(
#if LATENT_SPEC
		VirtualMachine * vm, UInt32 stackId, T_Base * base
#else
		T_Base * base
#endif
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

	CLASS_NAME(const char * fnName, const char * className, CallbackType callback, VirtualMachine * vm)
		: NativeFunction(fnName, className, IsStaticType <T_Base>::value, NUM_PARAMS)
	{
		// store callback
		m_callback = (void *)callback;

		InitParams(vm);
	}
	
	void InitParams(VirtualMachine * vm)
	{
#if NUM_PARAMS >= 1
		m_params.data[0].type64 = GetTypeID <T_Arg0>(vm);
#endif
#if NUM_PARAMS >= 2
		m_params.data[1].type64 = GetTypeID <T_Arg1>(vm);
#endif
#if NUM_PARAMS >= 3
		m_params.data[2].type64 = GetTypeID <T_Arg2>(vm);
#endif
#if NUM_PARAMS >= 4
		m_params.data[3].type64 = GetTypeID <T_Arg3>(vm);
#endif
#if NUM_PARAMS >= 5
		m_params.data[4].type64 = GetTypeID <T_Arg4>(vm);
#endif
#if NUM_PARAMS >= 6
		m_params.data[5].type64 = GetTypeID <T_Arg5>(vm);
#endif
#if NUM_PARAMS >= 7
		m_params.data[6].type64 = GetTypeID <T_Arg6>(vm);
#endif
#if NUM_PARAMS >= 8
		m_params.data[7].type64 = GetTypeID <T_Arg7>(vm);
#endif
#if NUM_PARAMS >= 9
		m_params.data[8].type64 = GetTypeID <T_Arg8>(vm);
#endif
#if NUM_PARAMS >= 10
		m_params.data[9].type64 = GetTypeID <T_Arg9>(vm);
#endif

#if LATENT_SPEC
		m_isLatent = true;
#endif

#if VOID_SPEC
		m_retnType = GetTypeID <void>(vm);
#else
		m_retnType = GetTypeID <T_Result>(vm);
#endif
	}

	virtual ~CLASS_NAME()	{ }

	virtual bool	Run(VMValue * baseValue, VirtualMachine * vm, UInt32 stackId, VMValue * resultValue, VMState * state)
	{
		// get argument list
		UInt32	argOffset = CALL_MEMBER_FN(state->argList, GetOffset)(state);

		T_Base	* base = NULL;

		// extract base object pointer for non-static types
		if (! IsStaticType <T_Base>::value)
		{
			UnpackValue(&base, baseValue);
			if (!base) return false;
		}

		// extract parameters
#if NUM_PARAMS >= 1
		T_Arg0	arg0;
		UnpackValue(&arg0, CALL_MEMBER_FN(state->argList, Get)(state, 0, argOffset));
#endif
#if NUM_PARAMS >= 2
		T_Arg1	arg1;
		UnpackValue(&arg1, CALL_MEMBER_FN(state->argList, Get)(state, 1, argOffset));
#endif
#if NUM_PARAMS >= 3
		T_Arg2	arg2;
		UnpackValue(&arg2, CALL_MEMBER_FN(state->argList, Get)(state, 2, argOffset));
#endif
#if NUM_PARAMS >= 4
		T_Arg3	arg3;
		UnpackValue(&arg3, CALL_MEMBER_FN(state->argList, Get)(state, 3, argOffset));
#endif
#if NUM_PARAMS >= 5
		T_Arg4	arg4;
		UnpackValue(&arg4, CALL_MEMBER_FN(state->argList, Get)(state, 4, argOffset));
#endif
#if NUM_PARAMS >= 6
		T_Arg5	arg5;
		UnpackValue(&arg5, CALL_MEMBER_FN(state->argList, Get)(state, 5, argOffset));
#endif
#if NUM_PARAMS >= 7
		T_Arg6	arg6;
		UnpackValue(&arg6, CALL_MEMBER_FN(state->argList, Get)(state, 6, argOffset));
#endif
#if NUM_PARAMS >= 8
		T_Arg7	arg7;
		UnpackValue(&arg7, CALL_MEMBER_FN(state->argList, Get)(state, 7, argOffset));
#endif
#if NUM_PARAMS >= 9
		T_Arg8	arg8;
		UnpackValue(&arg8, CALL_MEMBER_FN(state->argList, Get)(state, 8, argOffset));
#endif
#if NUM_PARAMS >= 10
		T_Arg9	arg9;
		UnpackValue(&arg9, CALL_MEMBER_FN(state->argList, Get)(state, 9, argOffset));
#endif


#if LATENT_SPEC
		bool		result =
#elif !VOID_SPEC
		T_Result	result =
#endif
			((CallbackType)m_callback)(
#if LATENT_SPEC
			vm, stackId, base
#else
			base
#endif
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
#if LATENT_SPEC
		resultValue->SetBool(result);
#elif VOID_SPEC
		resultValue->SetNone();
#else
		PackValue(resultValue, &result, vm);
#endif
		if (! IsStaticType <T_Base>::value)
		{
			DestroyValue(&base);
		}
		return true;
	}

private:
	// hide
	CLASS_NAME();
};

#undef VOID_SPEC
