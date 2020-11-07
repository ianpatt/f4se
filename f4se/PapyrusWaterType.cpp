#include "f4se/PapyrusWaterType.h"

#include "f4se/GameObjects.h"
#include "f4se/GameData.h"

#include "f4se/PapyrusStruct.h"

namespace papyrusWaterType
{
	SpellItem* GetConsumeSpell(TESWaterForm* thisWater)
	{
		return thisWater ? thisWater->consumeSpell : nullptr;
	}

	void SetConsumeSpell(TESWaterForm* thisWater, SpellItem* spell)
	{
		if(thisWater) {
			thisWater->consumeSpell = spell;
		}
	}

	SpellItem* GetContactSpell(TESWaterForm* thisWater)
	{
		return thisWater ? thisWater->contactSpell : nullptr;
	}

	void SetContactSpell(TESWaterForm* thisWater, SpellItem* spell)
	{
		if(thisWater) {
			thisWater->contactSpell = spell;
		}
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

void papyrusWaterType::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterForm(TESWaterForm::kTypeID, "WaterType");

	vm->RegisterFunction(
		new NativeFunction0 <TESWaterForm, SpellItem*>("GetConsumeSpell", "WaterType", papyrusWaterType::GetConsumeSpell, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESWaterForm, void, SpellItem*>("SetConsumeSpell", "WaterType", papyrusWaterType::SetConsumeSpell, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESWaterForm, SpellItem*>("GetContactSpell", "WaterType", papyrusWaterType::GetContactSpell, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESWaterForm, void, SpellItem*>("SetContactSpell", "WaterType", papyrusWaterType::SetContactSpell, vm));
}
