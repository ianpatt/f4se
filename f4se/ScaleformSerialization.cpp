#include "f4se/ScaleformSerialization.h"

#include "f4se/ScaleformCallbacks.h"
#include "f4se/ScaleformValue.h"

namespace Serialization
{
	template <>
	bool WriteData<GFxValue>(const F4SESerializationInterface* intfc, const GFxValue* val)
	{
		UInt32 type = val->GetType();
		if (! WriteData(intfc, &type))
			return false;

		switch (type)
		{
		case GFxValue::kType_Bool:
			{
				bool t = val->GetBool();
				return WriteData(intfc, &t);
			}
		case GFxValue::kType_Number:
			{
				double t = val->GetNumber();
				return WriteData(intfc, &t);
			}
		case GFxValue::kType_String:
			{
				const char* t = val->GetString();
				return WriteData(intfc, &t);
			}
		default:
			// Unsupported
			return false;
		}

		return false;
	}

	template <>
	bool ReadData<GFxValue>(const F4SESerializationInterface* intfc, GFxValue* val)
	{
		UInt32 type;
		if (! ReadData(intfc, &type))
			return false;

		switch (type)
		{
		case GFxValue::kType_Bool:
			{
				bool t;
				if (! ReadData(intfc, &t))
					return false;
				val->SetBool(t);
				return true;
			}
		case GFxValue::kType_Number:
			{
				double t;
				if (! ReadData(intfc, &t))
					return false;
				val->SetNumber(t);
				return true;
			}
		case GFxValue::kType_String:
			{
				// As usual, using string cache to manage strings
				BSFixedString t;
				if (! ReadData(intfc, &t))
					return false;
				val->SetString(t.c_str());
				return true;
			}
		default:
			// Unsupported
			return false;
		}

		return false;
	}
}
