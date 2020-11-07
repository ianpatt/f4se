#pragma once

class VirtualMachine;
struct StaticFunctionTag;

#include "f4se/GameTypes.h"

namespace papyrusF4SE
{
	void RegisterFuncs(VirtualMachine* vm);

	UInt32 GetVersion(StaticFunctionTag* base);
	UInt32 GetVersionMinor(StaticFunctionTag* base);
	UInt32 GetVersionBeta(StaticFunctionTag* base);
	UInt32 GetVersionRelease(StaticFunctionTag* base);
	UInt32 GetPluginVersion(StaticFunctionTag* base, BSFixedString name);
}
