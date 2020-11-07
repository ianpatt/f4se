#include "f4se/PapyrusActorBase.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

#include "f4se/GameObjects.h"
#include "f4se/GameData.h"

DECLARE_STRUCT(BodyWeight, "ActorBase")

namespace papyrusActorBase
{	
	TESNPC* GetTemplate(TESNPC* thisNPC, bool topMost)
	{
		TESNPC * node = nullptr;
		if(topMost) {
			node = thisNPC->templateNPC;
			if (node) {
				while (node->templateNPC)
					node = node->templateNPC;
			}
		}
		else
			node = thisNPC->templateNPC;

		return node;
	}

	bool HasHeadPartOverlays(TESNPC* thisNPC)
	{
		return CALL_MEMBER_FN(thisNPC, HasOverlays)();
	}

	VMArray<BGSHeadPart*> GetHeadParts(TESNPC* thisNPC, bool overlays)
	{
		VMArray<BGSHeadPart*> result;
		UInt32 numHeadParts = thisNPC->numHeadParts;
		BGSHeadPart ** headParts = thisNPC->headParts;
		if(CALL_MEMBER_FN(thisNPC, HasOverlays)() && overlays)
		{
			numHeadParts = CALL_MEMBER_FN(thisNPC, GetNumOverlayHeadParts)();
			headParts = CALL_MEMBER_FN(thisNPC, GetOverlayHeadParts)();
		}

		for(UInt32 i = 0; i < numHeadParts; i++)
		{
			result.Push(&headParts[i]);
		}
		return result;
	}

	BGSOutfit* GetOutfit(TESNPC* thisNPC, bool bSleepOutfit)
	{
		if (!thisNPC)
			return NULL;
		return bSleepOutfit ? thisNPC->outfit[1] : thisNPC->outfit[0];
	}

	void SetBodyWeight(TESNPC* thisNPC, BodyWeight weight)
	{
		if(thisNPC) {
			weight.Get<float>("thin", &thisNPC->weightThin);
			weight.Get<float>("muscular", &thisNPC->weightMuscular);
			weight.Get<float>("large", &thisNPC->weightLarge);

			thisNPC->MarkChanged(0x4000);
		}
	}

	BodyWeight GetBodyWeight(TESNPC* thisNPC)
	{
		BodyWeight weight;
		if(thisNPC) {
			weight.Set<float>("thin", thisNPC->weightThin);
			weight.Set<float>("muscular", thisNPC->weightMuscular);
			weight.Set<float>("large", thisNPC->weightLarge);
		}

		return weight;
	}
}

void papyrusActorBase::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction1 <TESNPC, TESNPC*, bool>("GetTemplate", "ActorBase", papyrusActorBase::GetTemplate, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESNPC, bool>("HasHeadPartOverlays", "ActorBase", papyrusActorBase::HasHeadPartOverlays, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESNPC, VMArray<BGSHeadPart*>, bool>("GetHeadParts", "ActorBase", papyrusActorBase::GetHeadParts, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESNPC, BGSOutfit*, bool>("GetOutfit", "ActorBase", papyrusActorBase::GetOutfit, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESNPC, void, BodyWeight>("SetBodyWeight", "ActorBase", papyrusActorBase::SetBodyWeight, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESNPC, BodyWeight>("GetBodyWeight", "ActorBase", papyrusActorBase::GetBodyWeight, vm));

	vm->SetFunctionFlags("ActorBase", "GetTemplate", IFunction::kFunctionFlag_NoWait);
}
