#include "f4se/PapyrusEvents.h"
#include "f4se/PapyrusUtilities.h"

#include "f4se/GameReferences.h"

RelocAddr <_SendCustomEvent> SendCustomEvent_Internal(0x013D9460);
RelocAddr <_CallFunctionNoWait> CallFunctionNoWait_Internal(0x013D69D0);
RelocAddr <_CallGlobalFunctionNoWait> CallGlobalFunctionNoWait_Internal(0x01451A80);

RegistrationMapHolder<UInt32>									g_inputKeyEventRegs;
RegistrationMapHolder<BSFixedString>							g_inputControlEventRegs;
RegistrationMapHolder<BSFixedString, ExternalEventParameters>	g_externalEventRegs;
RegistrationSetHolder<NullParameters>							g_cameraEventRegs;
RegistrationSetHolder<FormParameters>							g_furnitureEventRegs;

F4SEFurnitureEventSink g_furnitureEventSink;

EventResult	F4SEFurnitureEventSink::ReceiveEvent(TESFurnitureEvent * evn, void * dispatcher)
{
	g_furnitureEventRegs.ForEach(
		[&evn](const EventRegistration<FormParameters> & reg)
	{
		if(reg.params.NoFilter() || reg.params.HasFilter(evn->actor) || reg.params.HasFilter(evn->furniture))
			SendPapyrusEvent3<Actor*, TESObjectREFR*, bool>(reg.handle, reg.scriptName, "OnFurnitureEvent", evn->actor, evn->furniture, evn->isGettingUp);
	}
	);
#if 0
	UInt64 handle = PapyrusVM::GetHandleFromObject(evn->furniture, TESObjectREFR::kTypeID);
	// Do not use, function signature is not correct
	CALL_MEMBER_FN(*g_gameVM, SendPapyrusEvent)(handle, "OnFurnitureEvent", [handle](void * scriptVariable)
	{
		DumpClass(scriptVariable, 8);
		return true;
	});
#endif
	return kEvent_Continue;
}
