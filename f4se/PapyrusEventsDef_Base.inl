#if NUM_PARAMS > 10
#error PapyrusEventsDef: too many params
#endif

// Avoid passing VMArrays as arguments, behavior may be undefined as Papyrus does not normally support multi-dimensional arrays
template<
#if NUM_PARAMS >= 1
	typename T_arg1
#endif
#if NUM_PARAMS >= 2
	, typename T_arg2
#endif
#if NUM_PARAMS >= 3
	, typename T_arg3
#endif
#if NUM_PARAMS >= 4
	, typename T_arg4
#endif
#if NUM_PARAMS >= 5
	, typename T_arg5
#endif
#if NUM_PARAMS >= 6
	, typename T_arg6
#endif
#if NUM_PARAMS >= 7
	, typename T_arg7
#endif
#if NUM_PARAMS >= 8
	, typename T_arg8
#endif
#if NUM_PARAMS >= 9
	, typename T_arg9
#endif
#if NUM_PARAMS >= 10
	, typename T_arg10
#endif
>
void EVENT_NAME(
#if EVENT_OBJECT
	UInt64 handle,
#endif
	const BSFixedString & className, const BSFixedString & eventName
#if NUM_PARAMS >= 1
	, T_arg1 & t1
#endif
#if NUM_PARAMS >= 2
	, T_arg2 & t2
#endif
#if NUM_PARAMS >= 3
	, T_arg3 & t3
#endif
#if NUM_PARAMS >= 4
	, T_arg4 & t4
#endif
#if NUM_PARAMS >= 5
	, T_arg5 & t5
#endif
#if NUM_PARAMS >= 6
	, T_arg6 & t6
#endif
#if NUM_PARAMS >= 7
	, T_arg7 & t7
#endif
#if NUM_PARAMS >= 8
	, T_arg8 & t8
#endif
#if NUM_PARAMS >= 9
	, T_arg9 & t9
#endif
#if NUM_PARAMS >= 10
	, T_arg10 & t10
#endif
)
{
	VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;
#if EVENT_OBJECT
	VMValue receiver;
	if(GetIdentifier(&receiver, handle, &className, vm)) {		
#endif
		// Build the VM arguments for the CallFunctionNoWait
		VMArray<VMVariable> arguments;
#if NUM_PARAMS >= 1
		VMVariable var1;
		var1.Set<T_arg1>(&t1);
		arguments.Push(&var1);
#endif
#if NUM_PARAMS >= 2
		VMVariable var2;
		var2.Set<T_arg2>(&t2);
		arguments.Push(&var2);
#endif
#if NUM_PARAMS >= 3
		VMVariable var3;
		var3.Set<T_arg3>(&t3);
		arguments.Push(&var3);
#endif
#if NUM_PARAMS >= 4
		VMVariable var4;
		var4.Set<T_arg4>(&t4);
		arguments.Push(&var4);
#endif
#if NUM_PARAMS >= 5
		VMVariable var5;
		var5.Set<T_arg5>(&t5);
		arguments.Push(&var5);
#endif
#if NUM_PARAMS >= 6
		VMVariable var6;
		var6.Set<T_arg6>(&t6);
		arguments.Push(&var6);
#endif
#if NUM_PARAMS >= 7
		VMVariable var7;
		var7.Set<T_arg7>(&t7);
		arguments.Push(&var7);
#endif
#if NUM_PARAMS >= 8
		VMVariable var8;
		var8.Set<T_arg8>(&t8);
		arguments.Push(&var8);
#endif
#if NUM_PARAMS >= 9
		VMVariable var9;
		var9.Set<T_arg9>(&t9);
		arguments.Push(&var9);
#endif
#if NUM_PARAMS >= 10
		VMVariable var10;
		var10.Set<T_arg10>(&t10);
		arguments.Push(&var10);
#endif
		// Pack the VMArray
		VMValue args;
		PackValue(&args, &arguments, vm);

		// This should eventually replaced by an actual call to the Event Queue (VM+0x158), this is a workaround for now
#if EVENT_OBJECT
		if(receiver.IsIdentifier()) {
			VMIdentifier * identifier = receiver.data.id;
			if(identifier) {
				CallFunctionNoWait_Internal(vm, 0, identifier, &eventName, &args);
			}
		}
#else
		CallGlobalFunctionNoWait_Internal(vm, 0, 0, &className, &eventName, &args);
#endif
#if EVENT_OBJECT
	}
#endif
}
