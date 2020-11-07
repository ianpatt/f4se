#pragma once

namespace PapyrusVM
{
UInt64 GetHandleFromObject(void * src, UInt32 typeID);
void * GetObjectFromHandle(UInt64 handle, UInt32 typeID);
};
