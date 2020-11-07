#pragma once

#include "f4se/Serialization.h"
#include "f4se/PluginAPI.h"
#include "f4se/PapyrusArgs.h"

class VMVariable;

namespace Serialization
{
	bool WriteVMData(const F4SESerializationInterface* intfc, const VMValue * val);
	bool ReadVMData(const F4SESerializationInterface* intfc, VMValue * val);
};
