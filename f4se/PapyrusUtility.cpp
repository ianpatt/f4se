#include "f4se/PapyrusUtility.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

namespace papyrusUtility
{
	VMArray<VMVariable> VarToVarArray(StaticFunctionTag * tag, VMVariable var)
	{
		VMArray<VMVariable> result;
		var.Get<VMArray<VMVariable>>(&result);
		return result;
	}

	VMVariable VarArrayToVar(StaticFunctionTag * tag, VMArray<VMVariable> vars)
	{
		VMVariable result;
		result.Set<VMArray<VMVariable>>(&vars);
		return result;
	}
}

void papyrusUtility::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, VMArray<VMVariable>, VMVariable>("VarToVarArray", "Utility", papyrusUtility::VarToVarArray, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, VMVariable, VMArray<VMVariable>>("VarArrayToVar", "Utility", papyrusUtility::VarArrayToVar, vm));

	vm->SetFunctionFlags("Utility", "VarToVarArray", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Utility", "VarArrayToVar", IFunction::kFunctionFlag_NoWait);
}
