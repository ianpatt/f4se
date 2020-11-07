#pragma once

class VirtualMachine;
struct StaticFunctionTag;

#include "f4se/GameTypes.h"

namespace papyrusGame
{
	void RegisterFuncs(VirtualMachine* vm);
}
