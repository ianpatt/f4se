#include "f4se/papyrusComponent.h"

#include "f4se/GameExtraData.h"
#include "f4se/GameForms.h"
#include "f4se/GameObjects.h"
#include "f4se/GameRTTI.h"

namespace papyrusComponent
{
	TESObjectMISC * GetScrapItem(BGSComponent * thisObject)
	{
		return thisObject ? thisObject->scrapItem : nullptr;
	}

	void SetScrapItem(BGSComponent * thisObject, TESObjectMISC * scrapItem)
	{
		if(thisObject && scrapItem) {
			thisObject->scrapItem = scrapItem;
		}
	}

	TESGlobal * GetScrapScalar(BGSComponent * thisObject)
	{
		return thisObject ? thisObject->scrapScalar : nullptr;
	}

	void SetScrapScalar(BGSComponent * thisObject, TESGlobal * scrapScalar)
	{
		if(thisObject && scrapScalar) {
			thisObject->scrapScalar = scrapScalar;
		}
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

void papyrusComponent::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <BGSComponent, TESObjectMISC*>("GetScrapItem", "Component", papyrusComponent::GetScrapItem, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSComponent, void, TESObjectMISC*>("SetScrapItem", "Component", papyrusComponent::SetScrapItem, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSComponent, TESGlobal*>("GetScrapScalar", "Component", papyrusComponent::GetScrapScalar, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSComponent, void, TESGlobal*>("SetScrapScalar", "Component", papyrusComponent::SetScrapScalar, vm));
}
