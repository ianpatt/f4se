#include "f4se/PapyrusMath.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

#include <math.h>

namespace papyrusMath
{
	UInt32 LeftShift(StaticFunctionTag* base, UInt32 value, UInt32 shiftBy)
	{
		return (shiftBy >= 32) ? 0 : value << shiftBy;
	}

	UInt32 RightShift(StaticFunctionTag* base, UInt32 value, UInt32 shiftBy)
	{
		return (shiftBy >= 32) ? 0 : value >> shiftBy;
	}

	UInt32 LogicalAnd(StaticFunctionTag* base, UInt32 arg1, UInt32 arg2)
	{
		return arg1 & arg2;
	}

	UInt32 LogicalOr(StaticFunctionTag* base, UInt32 arg1, UInt32 arg2)
	{
		return arg1 | arg2;
	}

	UInt32 LogicalXor(StaticFunctionTag* base, UInt32 arg1, UInt32 arg2)
	{
		return arg1 ^ arg2;
	}

	UInt32 LogicalNot(StaticFunctionTag* base, UInt32 arg1)
	{
		return ~arg1;
	}

	float Log(StaticFunctionTag* base, float arg1)
	{
		return log(arg1);
	}

	float Exp(StaticFunctionTag* base, float arg1)
	{
		return exp(arg1);
	}
}

void papyrusMath::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, UInt32, UInt32, UInt32>("LeftShift", "Math", papyrusMath::LeftShift, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, UInt32, UInt32, UInt32>("RightShift", "Math", papyrusMath::RightShift, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, UInt32, UInt32, UInt32>("LogicalAnd", "Math", papyrusMath::LogicalAnd, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, UInt32, UInt32, UInt32>("LogicalOr", "Math", papyrusMath::LogicalOr, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, UInt32, UInt32, UInt32>("LogicalXor", "Math", papyrusMath::LogicalXor, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, UInt32>("LogicalNot", "Math", papyrusMath::LogicalNot, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, float>("Log", "Math", papyrusMath::Log, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, float>("Exp", "Math", papyrusMath::Exp, vm));

	vm->SetFunctionFlags("Math", "LeftShift", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Math", "RightShift", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Math", "LogicalAnd", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Math", "LogicalOr", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Math", "LogicalXor", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Math", "LogicalNot", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Math", "Log", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Math", "Exp", IFunction::kFunctionFlag_NoWait);
}
