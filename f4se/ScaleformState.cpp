#include "f4se/ScaleformState.h"

void F4SEGFxLogger::LogMessageVarg(UInt32 messageType, const char* fmt, va_list args)
{
	gLog.LogNNL(IDebugLog::kLevel_Message, fmt, args);
}
