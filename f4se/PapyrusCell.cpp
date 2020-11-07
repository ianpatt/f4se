#include "f4se/PapyrusCell.h"

#include "f4se/GameExtraData.h"
#include "f4se/GameForms.h"
#include "f4se/GameRTTI.h"

namespace papyrusCell
{
	TESWaterForm * GetWaterType(TESObjectCELL* thisCell)
	{
		if(!thisCell)
			return nullptr;

		// If there is a water override there should be one in its ExtraData
		if((thisCell->flags & TESObjectCELL::kFlag_HasWater) == TESObjectCELL::kFlag_HasWater) {
			auto pExtraData = thisCell->extraDataList;
			if(pExtraData) {
				ExtraCellWaterType * extraWaterType = (ExtraCellWaterType *)pExtraData->GetByType(kExtraData_CellWaterType);
				if(extraWaterType)
					return extraWaterType->waterForm;
			}
		}

		// Exteriors have implied water, the default water if theres no override
		if((thisCell->flags & TESObjectCELL::kFlag_IsInterior) != TESObjectCELL::kFlag_IsInterior)
			return DYNAMIC_CAST(LookupFormByID(0x18), TESForm, TESWaterForm);

		return nullptr;
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

void papyrusCell::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <TESObjectCELL, TESWaterForm*>("GetWaterType", "Cell", papyrusCell::GetWaterType, vm));
}
