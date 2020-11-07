#pragma once

#include "f4se/Serialization.h"
#include "f4se/PluginAPI.h"

class GFxValue;

namespace Serialization
{
	template <>
	bool WriteData<GFxValue>(const F4SESerializationInterface* intfc, const GFxValue* val);

	template <>
	bool ReadData<GFxValue>(const F4SESerializationInterface* intfc, GFxValue* val);
};
