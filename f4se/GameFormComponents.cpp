#include "f4se/GameFormComponents.h"
#include "f4se/GameForms.h"

RelocAddr <_EvaluationConditions> EvaluationConditions(0x0072AF80);

#ifdef _DEBUG
#include "f4se/GameExtraData.h"

void BGSInventoryItem::Dump()
{
	_MESSAGE("%016I64X %s", form->formID, GetObjectClassName(form));
	gLog.Indent();
	stack->Dump();
	gLog.Outdent();
}

void BGSInventoryItem::Stack::Dump()
{
	_MESSAGE("Count: %d", count);
	if(extraData)
		extraData->Dump();

	if(next) {
		gLog.Indent();
		next->Dump();
		gLog.Outdent();
	}
	
}
#endif
