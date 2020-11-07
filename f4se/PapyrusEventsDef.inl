
#define EVENT_OBJECT 1
#define EVENT_NAME __MACRO_JOIN__(SendPapyrusEvent, NUM_PARAMS)
#include "PapyrusEventsDef_Base.inl"
#undef EVENT_NAME
#undef EVENT_OBJECT

#define EVENT_OBJECT 0
#define EVENT_NAME __MACRO_JOIN__(CallGlobalFunctionNoWait, NUM_PARAMS)
#include "PapyrusEventsDef_Base.inl"
#undef EVENT_NAME
#undef EVENT_OBJECT
#undef NUM_PARAMS
